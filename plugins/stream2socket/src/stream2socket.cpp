#ifdef VS_HAS_CONTROLLER_SOCKET
    #include "camerasocket.h"
#endif

#include "stream2socket.h"

inline void remove_alpha(unsigned char * data_in, unsigned int nb_pixels, unsigned char * data_out)
{
    for(unsigned int i = 0; i < nb_pixels; ++i)
    {
        memcpy(&(data_out[3*i]), &(data_in[4*i]), 3);
    }
}

using boost::asio::ip::udp;

namespace visionsystem
{

Stream2SocketProcess::Stream2SocketProcess( boost::asio::io_service & io_service, short port, unsigned int active_cam, Camera * cam, 
                            bool compress, bool raw, bool reverse_connection, const std::string & server_name,
                            bool verbose)
: socket_(0), server_name_(server_name), port_(port), chunkIDs_(0),
  encoder_(0), ready_(false), active_cam_(active_cam), cam_(cam), send_img_(0), send_img_raw_data_(0), send_img_data_size_(0),
  img_lock_(false), next_cam_(false), request_cam_(false), request_name_(""),
  verbose_(verbose)
{
    if(compress)
    {
        encoder_ = new vision::H264Encoder(cam->get_size().x, cam->get_size().y, cam->get_fps());
        #if Vision_HAS_LIBAVCODEC != 1
        std::cerr << "[stream2socket] H.264 support not built in vision library, Compress option is not usable" << std::endl;
        #endif
    }

    vision::ImageRef image_size = cam->get_size();
    send_img_ = new vision::Image<uint32_t, vision::RGB>(image_size);
    send_img_data_size_ = 3 * send_img_->pixels;
    send_img_raw_data_ = (unsigned char *)(send_img_->raw_data);

    socket_ = new udp::socket(io_service);
    socket_->open(udp::v4());
    send_buffer_ = new unsigned char[send_size_];
    client_data_ = new char[max_request_];

    if(reverse_connection)
    {
        /* DNS Resolution */
        {
            udp::resolver resolver(io_service);
            std::stringstream ss;
            ss << port;
            udp::resolver::query query(udp::v4(), server_name_, ss.str());
            receivers_endpoint_.push_back(*resolver.resolve(query));
            chunkIDs_.push_back(0);
        }
        if(verbose_)
        {
            std::cout << "[stream2socket] connected to " << server_name_ << ":" << port_ << " to stream images" << std::endl;
        }
        ready_ = true;
        /* Get ready for eventual request */
        socket_->async_receive_from(
           boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
           boost::bind(&Stream2SocketProcess::handle_receive_from, this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        socket_->bind(udp::endpoint(udp::v4(), port_));
        if(verbose_)
        {
            std::cout << "[stream2socket] Waiting for request now" << std::endl;
        }
        socket_->async_receive_from(
           boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
           boost::bind(&Stream2SocketProcess::handle_receive_from, this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred));
    }
}

Stream2SocketProcess::~Stream2SocketProcess()
{
    delete send_img_;
    delete[] client_data_;
    delete[] send_buffer_;
    delete socket_;
    delete encoder_;
}

Stream2Socket::Stream2Socket( visionsystem::VisionSystem * vs, std::string sandbox )
: Plugin( vs, "stream2socket", sandbox ),
  io_service_(), io_service_th_(0),
  raw_(false),
  reverse_connection_(false),
  cams_(0),
  compress_data_(false),
  ports_(0),
  cam_names_(0),
  processes_(0),
  verbose_(false)
{}

Stream2Socket::~Stream2Socket()
{
    for(size_t i = 0; i < processes_.size(); ++i)
    {
        delete processes_[i];
    }
}

void Stream2SocketProcess::handle_send_to(const boost::system::error_code & error,
                            size_t bytes_send, size_t id)
{
    if(error) { std::cerr << error.message() << std::endl; }
    if(bytes_send < send_size_) // after last packet sent
    {
        img_lock_ = false;
        if(verbose_)
        {
            std::cout << "[stream2socket] Image sent, waiting for next image" << std::endl;
        }
        return;
    }
    chunkIDs_[id]++;
    send_buffer_[0] = chunkIDs_[id];
    size_t send_size = 0;
    if( (chunkIDs_[id] + 1)*(send_size_ - 1) > send_img_data_size_ )
    {
        send_size = send_img_data_size_ - chunkIDs_[id]*(send_size_ - 1) + 1;
    }
    else
    {
        send_size = send_size_;
    }
    std::memcpy(&(send_buffer_[1]), &(send_img_raw_data_[chunkIDs_[id]*(send_size_ - 1)]), send_size - 1);
    if(verbose_)
    {
        std::cout << "[stream2socket] More data to send, next packet size: " << send_size << std::endl;
    }
    socket_->async_send_to(
        boost::asio::buffer(send_buffer_, send_size), receivers_endpoint_[id],
        boost::bind(&Stream2SocketProcess::handle_send_to, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred, id));
}

bool Stream2Socket::pre_fct()
{
    std::string filename = get_sandbox() + std::string("/stream2socket.conf");

    try
    {
        read_config_file(filename.c_str());
    }
    catch(std::string msg)
    {
        throw(std::string("stream2socket will not work without a correct stream2socket.conf config file"));
    }

    if( ports_.size() > 1 && (reverse_connection_ && server_names_.size() != ports_.size()) )
    {
        throw(std::string("stream2socket configured for reverse connection but mismatching ports and servers"));
    }
    if( cam_names_.size() != 0 && cam_names_.size() != ports_.size() )
    {
        throw(std::string("stream2socket configured for specific streaming but mismatching ports and cameras"));
    }

    /* Store all active cameras */
    std::vector<Camera *> cameras = get_all_cameras();
    for(size_t i = 0; i < cameras.size(); ++i)
    {
        if(cameras[i]->is_active())
        {
            cams_.push_back(cameras[i]);
        }
    }
    if(cams_.size() == 0)
    {
        throw(std::string("No active cameras in the server, stream2socket cannot operate"));
    }

    if(cam_names_.size() != 0 && cams_.size() < cam_names_.size() )
    {
        throw(std::string("stream2socket configured for specific streaming but not enough cameras to stream"));
    }

    for(size_t i = 0; i < ports_.size(); ++i)
    {
        Camera * cam = cams_[0];
        std::string server_name = "";
        if(cam_names_.size())
        {
            cam = get_camera(cam_names_[i]);
        }
        if(server_names_.size())
        {
            server_name = server_names_[i];
        }
        if(!cam)
        {
            std::stringstream ss; ss << "stream2socket cannot find camera named " << cam_names_[i] << std::endl;
            throw(ss.str());
        }
        Stream2SocketProcess * process = new Stream2SocketProcess(io_service_, ports_[i], 0, cam, compress_data_, raw_, reverse_connection_, server_name, verbose_);
        register_to_cam< vision::Image<uint32_t, vision::RGB> >( cam, 10 ) ;
        processes_.push_back(process);
    }

    return true ;
}

void Stream2Socket::preloop_fct()
{
    io_service_th_ = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_));
}

void Stream2Socket::loop_fct()
{
    for(size_t i = 0; i < processes_.size(); ++i)
    {
        Stream2SocketProcess * process = processes_[i];
        vision::Image<uint32_t, vision::RGB> * img = dequeue_image< vision::Image<uint32_t, vision::RGB> > (process->cam_);

        if(!process->img_lock_ && process->ready_)
        {
            if(compress_data_)
            {
                vision::H264EncoderResult res = process->encoder_->Encode(*img);
                process->send_img_data_size_ = res.frame_size;
                process->send_img_raw_data_  = res.frame_data;
            }
            else if(raw_)
            {
                memcpy(process->send_img_raw_data_, img->raw_data, img->data_size);
                process->send_img_data_size_ = img->data_size;
            }
            else
            {
                remove_alpha((unsigned char*)(img->raw_data), img->pixels, process->send_img_raw_data_);
            }
            process->img_lock_ = true;
            process->send_buffer_[0] = 0;
            size_t send_size = 0;
            if( (0 + 1)*(process->send_size_ - 1) > process->send_img_data_size_ )
            {
                send_size = process->send_img_data_size_ - 0*(process->send_size_ - 1) + 1;
            }
            else
            {
                send_size = process->send_size_;
            }
            std::memcpy(&(process->send_buffer_[1]), &(process->send_img_raw_data_[0*(process->send_size_ - 1)]), send_size - 1);
            if(verbose_)
            {
                std::cout << "[stream2socket] Sending data to client, data size: " << send_size << std::endl;
            }
            for(size_t id = 0; id < process->receivers_endpoint_.size(); ++id)
            {
                process->chunkIDs_[id] = 0;
                process->socket_->async_send_to(
                    boost::asio::buffer(process->send_buffer_, send_size), process->receivers_endpoint_[id],
                    boost::bind(&Stream2SocketProcess::handle_send_to, process,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred, id));
            }
        }

        enqueue_image< vision::Image<uint32_t, vision::RGB> >(process->cam_, img);

        if(process->next_cam_)
        {
            unregister_to_cam< vision::Image<uint32_t, vision::RGB> > (process->cam_);
            process->active_cam_ = (process->active_cam_ + 1) % cams_.size();
            register_to_cam< vision::Image<uint32_t, vision::RGB> > (cams_[process->active_cam_], 10);
            process->cam_ = cams_[process->active_cam_];
            process->next_cam_ = false;
        }
        if(process->request_cam_)
        {
            if(raw_)
            {
    #ifdef VS_HAS_CONTROLLER_SOCKET
                CameraSocket * cam = dynamic_cast<CameraSocket*>(process->cam_);
                if(cam && cam->from_stream())
                {
                    cam->request_cam(process->request_name_);
                }
    #endif
            }
            else
            {
                for(size_t i = 0; i < cams_.size(); ++i)
                {
                    if(cams_[i]->get_name() == process->request_name_)
                    {
                        unregister_to_cam< vision::Image<uint32_t, vision::RGB> > (process->cam_);
                        process->active_cam_ = i;
                        register_to_cam< vision::Image<uint32_t, vision::RGB> > (cams_[process->active_cam_], 10);
                        process->cam_ = cams_[process->active_cam_];
                    }
                }
            }
            process->request_cam_ = false;
        }
    }
}

bool Stream2Socket::post_fct()
{
    for(size_t i = 0; i < processes_.size(); ++i)
    {
        Stream2SocketProcess * process = processes_[i];

        unregister_to_cam< vision::Image<uint32_t, vision::RGB> >(process->cam_);

        process->socket_->close();
    }
    io_service_.stop();
    io_service_th_->join();
    delete io_service_th_;
    io_service_th_ = 0;

    return true;
}

void Stream2SocketProcess::handle_receive_from(const boost::system::error_code & error,
                                 size_t bytes_recvd)
{
    if(!error && bytes_recvd > 0)
    {
        std::string client_message(client_data_);
        if(verbose_)
        {
            std::cout << "[stream2socket] Got request " << client_message << std::endl;
        }
        if(client_message == "get")
        {
            /* Client request: reset sending vision::Image */
            bool new_client = true;
            for(size_t id = 0; id < receivers_endpoint_.size(); ++id)
            {
                if(sender_endpoint_ == receivers_endpoint_[id])
                {
                    new_client = false;
                    break;
                }
            }
            if(new_client)
            {
                receivers_endpoint_.push_back(sender_endpoint_);
                chunkIDs_.push_back(0);
            }
            ready_ = true;
        }
        else if(client_message == "next")
        {
            /* Client requested to change camera stream */
            next_cam_ = true;
        }
        else if(client_message.substr(0, 8) == "request ")
        {
            request_cam_ = true;
            request_name_ = client_message.substr(8);
        }
    }
    else
    {
        if(verbose_)
        {
            std::cout << "[stream2socket] Reception error, waiting for next message" << std::endl;
        }
        if(error) { std::cerr << error.message() << std::endl; }
    }
    socket_->async_receive_from(
        boost::asio::buffer(client_data_, max_request_), sender_endpoint_,
        boost::bind(&Stream2SocketProcess::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Stream2Socket::parse_config_line( std::vector<std::string> & line )
{
    if( fill_member(line, "Port", ports_) )
        return;

    if( fill_member(line, "Compress", compress_data_) )
    {
#if Vision_HAS_LIBAVCODEC != 1
        if(compress_data_)
        {
            std::cerr << "[WARNING] You configured stream2socket to compress data without H.264 support" << std::endl;
        }
#endif
        return;
    }

    if( fill_member(line, "Raw", raw_) )
        return;

    if( fill_member(line, "ReverseConnection", reverse_connection_) )
        return;

    if( fill_member(line, "ServerName", server_names_) )
        return;

    if( fill_member(line, "ServerPort", ports_) )
        return;

    if( fill_member(line, "Verbose", verbose_) )
        return;

    if( fill_member(line, "Cameras", cam_names_) )
        return;
}

} // namespace visionsystem

