#include <vision/io/imageio.h>
#include <vision/io/H264Encoder.h>
#include <boost/filesystem.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

extern "C"
{
#include "output/output.h"
}

#define MAX_DEPTH 10000

using namespace vision;
using namespace boost::filesystem;

void usage(const char * exec_name)
{
    std::cerr << "[Usage] " << exec_name << " [path to bin files] [out] [fps]" << std::endl;
    std::cerr << "out defaults to out.mkv ; fps defaults to 15" << std::endl;
}

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        usage(argv[0]);
        return 1;
    }
    std::string movie_name = "out.mkv";
    if(argc > 2)
    {
        movie_name = argv[2];
    }

    int fps = 15;
    if(argc > 3)
    {
        fps = atoi(argv[3]);
    }

    path bin_path = path(argv[1]);
    if( exists(bin_path) )
    {
        std::vector<std::string> bin_files;
        if( is_regular_file(bin_path) )
        {
            std::string ext = extension(bin_path);
            if(ext != ".bin")
            {
                std::cerr << "[Error] " << argv[1] << " isn't a .bin file from vision library" << std::endl;
                return 1;
            }
            else
            {
                bin_files.push_back(bin_path.string());
            }
        }
        else if( is_directory(bin_path) )
        {
            std::vector<path> ls_path;
            copy(directory_iterator(bin_path), directory_iterator(), back_inserter(ls_path));
            for(std::vector<path>::const_iterator it = ls_path.begin(); it != ls_path.end(); ++it)
            {
                if( is_regular_file(*it) && extension(*it) == ".bin" )
                {
                    bin_files.push_back((*it).string());
                }
            }
        }
        else
        {
            std::cerr << "[Error] " << argv[1] << " is neither a directory nor a regular file" << std::endl;
            return 1;
        }
        std::sort(bin_files.begin(), bin_files.end());

        hnd_t hout = 0;
        cli_output_opt_t output_opt;
        memset( &output_opt, 0, sizeof(cli_output_opt_t) );

        if( mkv_output.open_file( const_cast<char*>(movie_name.c_str()), &hout, &output_opt ) )
        {
            std::cerr << "Failed to open " << movie_name << std::endl;
            return -1;
        }

        Image<uint32_t, RGB> * in_img = new Image<uint32_t, RGB>();
        H264Encoder * encoder = 0;
        x264_param_t param;
        for(size_t i = 0; i < bin_files.size(); ++i)
        {
            deserialize(bin_files[i], *in_img);
            if(!encoder)
            {
                x264_param_default_preset(&param, "veryfast", "zerolatency");
                param.i_threads = 0;
                param.b_intra_refresh = 0;
                param.b_repeat_headers = 0;
                param.b_annexb = 0;
                param.i_csp = X264_CSP_I420;
                param.i_width = in_img->width;
                param.i_height = in_img->height;
                param.i_fps_num = fps;
                param.i_fps_den = 1;
                // Intra refres:
                param.i_keyint_max = fps;
                param.b_intra_refresh = 1;
                //Rate control:
                param.rc.i_rc_method = X264_RC_CRF;
                param.rc.f_rf_constant = 25;
                param.rc.f_rf_constant_max = 35;

                // Specific for video encoding
                param.b_vfr_input = 1;
                param.i_fps_num = fps;
                param.i_fps_den = 1;
                param.i_timebase_num = 1;
                param.i_timebase_den = fps;
                param.vui.i_sar_width  = 1;
                param.vui.i_sar_height = 1;
                param.i_frame_total = bin_files.size();

                encoder = new H264Encoder(&param);
                x264_param_apply_fastfirstpass( &param );
                x264_param_apply_profile(&param, "baseline");
                if( mkv_output.set_param(hout, &param) )
                {
                    std::cerr << "Failed to set_param no output" << std::endl;
                    return -1;
                }
                // Write SPS/PPS/SEI
                x264_nal_t *headers;
                int i_nal;

                if( x264_encoder_headers( encoder->GetEncoder(), &headers, &i_nal ) < 0 )
                {
                    std::cerr <<  "x264_encoder_headers failed" << std::endl;
                    return -1;
                }
                if( mkv_output.write_headers( hout, headers ) < 0 )
                {
                    std::cerr <<  "error writing headers to output file" << std::endl;
                    return -1;
                }
            }
            H264EncoderResult res = encoder->Encode(*in_img, i);
            if(res.frame_size)
            {
                mkv_output.write_frame( hout, res.frame_data, res.frame_size, encoder->GetPicOut() );
            }
            std::cout << "\rEncoded " << i << " frames out of " << bin_files.size() << std::flush;
        }
        std::cout << std::endl;

        delete in_img;
        delete encoder;
        if(hout) { mkv_output.close_file(hout, 0, 0); }
        
    }
    else
    {
        std::cerr << "[Error] Cannot find path: " << argv[1] << std::endl;
        return 1;
    }

    return 0;
}

