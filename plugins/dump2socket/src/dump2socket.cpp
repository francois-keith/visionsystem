#include "dump2socket.h"

#include "config.h"
#if VS_HAS_ZLIB == 1

#include <zlib.h>

unsigned int compress(unsigned char * data_in, unsigned int data_in_size, unsigned char * data_out)
{
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;
    int ret = deflateInit(&strm, Z_BEST_SPEED);

    strm.avail_in = data_in_size;
    strm.next_in  = data_in;

    strm.avail_out = data_in_size;
    strm.next_out  = data_out;

    int flush = Z_FINISH;
    ret = deflate(&strm, flush);
    unsigned int zsize = data_in_size - strm.avail_out;
    deflateEnd(&strm);
    return zsize;
}

#else

unsigned int compress(unsigned char * data_in, unsigned int data_in_size, unsigned char * data_out)
{
    memcpy(data_out, data_in, data_in_size);
    return data_in_size;
}

#endif // ZLIB specific function

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

Dump2Socket::Dump2Socket( visionsystem::VisionSystem * vs, std::string sandbox )
: Plugin( vs, "dump2socket", sandbox ),
  io_service_(), io_service_th_(0),
  sockets_(0),
  ports_(0), chunkIDs_(0),
  cam_names_(0), cams_(0), is_mono_(true), compress_data_(false),
  send_imgs_mono_(0), send_imgs_rgb_(0), imgs_lock_(0)
{}

Dump2Socket::~Dump2Socket()
{
    for(size_t i = 0; i < send_imgs_mono_.size(); ++i)
    {
        delete send_imgs_mono_[i];
    }
    for(size_t i = 0; i < send_imgs_rgb_.size(); ++i)
    {
        delete send_imgs_rgb_[i];
    }
    for(size_t i = 0; i < clients_data_.size(); ++i)
    {
        delete[] clients_data_[i];
    }
    for(size_t i = 0; i < send_buffers_.size(); ++i)
    {
        delete[] send_buffers_[i];
    }
    for(size_t i = 0; i < sockets_.size(); ++i)
    {
        delete sockets_[i];
    }
}

bool Dump2Socket::pre_fct()
{
    std::string filename = get_sandbox() + std::string("/dump2socket.conf");

    try
    {
        read_config_file(filename.c_str());
    }
    catch(std::string msg)
    {
        throw(std::string("dump2socket will not work without a correct dump2socket.conf config file"));
    }

    size_t nb_cams_to_stream = ports_.size();
    cams_.resize(nb_cams_to_stream);
    size_t next_camera = 0;
    /* First camera strategy: get camera from names */
    if( cam_names_.size() != 0 )
    {
        for(size_t i = 0; i < cam_names_.size() && next_camera < nb_cams_to_stream; ++i)
        {
            Camera * cam = get_camera(cam_names_[i]);
            if(cam == 0)
            {
                std::stringstream err;
                err << "Camera " << cam_names_[i] << " not registered with vs_core" << std::endl;
                throw(err.str());
            }
            cams_[next_camera] = cam;
            next_camera++;
        }
    }
    else
    {
        /* Second strategy: get any camera  */
        std::vector<Camera *> cameras = get_all_cameras();
        for(size_t i = 0; i < cameras.size(); ++i)
        {
            if(cameras[i]->is_active())
            {
                cams_[next_camera] = cameras[i];
                next_camera++;
                if(next_camera == nb_cams_to_stream) { i = cameras.size(); }
            }
        }
    }
    if(next_camera != nb_cams_to_stream)
    {
        throw(std::string("[dump2socket] Not enough cameras streamable"));
    }

    if(is_mono_)
    {
        for(size_t i = 0; i < cams_.size(); ++i)
        {
            register_to_cam< vision::Image<unsigned char, vision::MONO> >( cams_[i], 10 ) ;
            send_imgs_mono_.push_back(new vision::Image<unsigned char, vision::MONO>(cams_[i]->get_size()));
            send_imgs_data_size_.push_back(send_imgs_mono_[i]->data_size);
            send_imgs_raw_data_.push_back(send_imgs_mono_[i]->raw_data);
        }
    }
    else
    {
        for(size_t i = 0; i < cams_.size(); ++i)
        {
            register_to_cam< vision::Image<uint32_t, vision::RGB> >( cams_[i], 10 ) ;
            send_imgs_rgb_.push_back(new vision::Image<uint32_t, vision::RGB>(cams_[i]->get_size()));
            send_imgs_data_size_.push_back(3 * send_imgs_rgb_[i]->pixels);
            send_imgs_raw_data_.push_back((unsigned char*)(send_imgs_rgb_[i]->raw_data));
        }
    }

    for(size_t i = 0; i < ports_.size(); ++i)
    {
        sockets_.push_back(new udp::socket(io_service_));
        sockets_[i]->open(udp::v4());
        sockets_[i]->bind(udp::endpoint(udp::v4(), ports_[i]));
        clients_data_.push_back(new char[max_request_]);
        send_buffers_.push_back(new unsigned char[send_size_]);
    }

    sender_endpoints_.resize(ports_.size());
    imgs_lock_.resize(ports_.size());
    chunkIDs_.resize(ports_.size());

    for(size_t i = 0; i < sockets_.size(); ++i)
    {
        sockets_[i]->async_receive_from(
            boost::asio::buffer(clients_data_[i], max_request_), sender_endpoints_[i],
            boost::bind(&Dump2Socket::handle_receive_from, this, i,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    return true ;
}

void Dump2Socket::preloop_fct()
{
    io_service_th_ = new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_));
}

void Dump2Socket::loop_fct()
{
    if(is_mono_)
    {
        for(size_t i = 0; i < ports_.size(); ++i)
        {
            vision::Image<unsigned char, vision::MONO> * img = dequeue_image< vision::Image<unsigned char, vision::MONO> > (cams_[i]);

            if(imgs_lock_[i])
            {
                if(compress_data_)
                {
                    send_imgs_data_size_[i] = compress(img->raw_data, img->data_size, send_imgs_raw_data_[i]);
                }
                else
                {
                    send_imgs_mono_[i]->copy(img);
                }
                imgs_lock_[i] = false;
            }

            enqueue_image< vision::Image<unsigned char, vision::MONO> >(cams_[i], img);
        }
    }
    else
    {
        for(size_t i = 0; i < ports_.size(); ++i)
        {
            vision::Image<uint32_t, vision::RGB> * img = dequeue_image< vision::Image<uint32_t, vision::RGB> > (cams_[i]);

            if(imgs_lock_[i])
            {
                if(compress_data_)
                {
                    send_imgs_data_size_[i] = compress((unsigned char *)(img->raw_data), img->data_size, send_imgs_raw_data_[i]);
                }
                else
                {
                    remove_alpha((unsigned char *)(img->raw_data), img->pixels, send_imgs_raw_data_[i]);
                }
                imgs_lock_[i] = false;
            }

            enqueue_image< vision::Image<uint32_t, vision::RGB> >(cams_[i], img);
        }
    }
}

bool Dump2Socket::post_fct()
{
    if(is_mono_)
    {
        for(size_t i = 0; i < cams_.size(); ++i)
        {
            unregister_to_cam< vision::Image<unsigned char, vision::MONO> >(cams_[i]);
        }
    }
    else
    {
        for(size_t i = 0; i < cams_.size(); ++i)
        {
            unregister_to_cam< vision::Image<uint32_t, vision::RGB> >(cams_[i]);
        }
    }

    for(size_t i = 0; i < sockets_.size(); ++i)
    {
        sockets_[i]->close();
    }
    io_service_.stop();
    io_service_th_->join();
    delete io_service_th_;
    io_service_th_ = 0;

    return true;
}

void Dump2Socket::handle_receive_from(size_t i, const boost::system::error_code & error,
                                 size_t bytes_recvd)
{
    if(!error && bytes_recvd > 0)
    {
        std::string client_message(clients_data_[i]);
        if(client_message == "get")
        {
            /* Client request: reset sending vision::Image */
            chunkIDs_[i] = 0;
            imgs_lock_[i] = true;
            while(imgs_lock_[i]) { usleep(100); }
        }
        send_buffers_[i][0] = chunkIDs_[i];
        size_t send_size = 0;
        if( (chunkIDs_[i] + 1)*(send_size_ - 1) > send_imgs_data_size_[i] )
        {
            send_size = send_imgs_data_size_[i] - chunkIDs_[i]*(send_size_ - 1) + 1;
        }
        else
        {
            send_size = send_size_;
        }
        std::memcpy(&(send_buffers_[i][1]), &(send_imgs_raw_data_[i][chunkIDs_[i]*(send_size_ - 1)]), send_size - 1);
        sockets_[i]->async_send_to(
            boost::asio::buffer(send_buffers_[i], send_size), sender_endpoints_[i],
            boost::bind(&Dump2Socket::handle_send_to, this, i,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        sockets_[i]->async_receive_from(
            boost::asio::buffer(clients_data_[i], max_request_), sender_endpoints_[i],
            boost::bind(&Dump2Socket::handle_receive_from, this, i,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
}

void Dump2Socket::handle_send_to(size_t i, const boost::system::error_code & error,
                            size_t bytes_send)
{
    if(error) { std::cerr << error.message() << std::endl; }
    if(bytes_send < send_size_) // after last packet sent
    {
        sockets_[i]->async_receive_from(
            boost::asio::buffer(clients_data_[i], max_request_), sender_endpoints_[i],
            boost::bind(&Dump2Socket::handle_receive_from, this, i,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        return;
    }
    (chunkIDs_[i])++;
    send_buffers_[i][0] = chunkIDs_[i];
    size_t send_size = 0;
    if( (chunkIDs_[i] + 1)*(send_size_ - 1) > send_imgs_data_size_[i] )
    {
        send_size = send_imgs_data_size_[i] - chunkIDs_[i]*(send_size_ - 1) + 1;
    }
    else
    {
        send_size = send_size_;
    }
    std::memcpy(&(send_buffers_[i][1]), &(send_imgs_raw_data_[i][chunkIDs_[i]*(send_size_ - 1)]), send_size - 1);
    sockets_[i]->async_send_to(
        boost::asio::buffer(send_buffers_[i], send_size), sender_endpoints_[i],
        boost::bind(&Dump2Socket::handle_send_to, this, i,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void Dump2Socket::parse_config_line( std::vector<std::string> & line )
{
    if( fill_member(line, "Port", ports_) )
        return;

    if( fill_member(line, "Names", cam_names_) )
        return;

    std::string mode;
    if( fill_member(line, "ColorMode", mode) )
    {
        is_mono_ = (mode == "MONO");
        return;
    }
    if( fill_member(line, "Compress", compress_data_) )
    {
#if VS_HAS_ZLIB != 1
        if(compress_data_)
        {
            std::cerr << "[WARNING] You configured dump2socket to compress data without ZLIB support" << std::endl;
        }
#endif
        return;
    }
}

} // namespace visionsystem

