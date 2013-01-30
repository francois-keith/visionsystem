#include <vision/io/imageio.h>
#include <vision/io/MKVEncoder.h>
#include <boost/filesystem.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

        Image<uint32_t, RGB> * in_img = new Image<uint32_t, RGB>();
        MKVEncoder * encoder = 0;

        for(size_t i = 0; i < bin_files.size(); ++i)
        {
            deserialize(bin_files[i], *in_img);
            if(!encoder)
            {
                encoder = new MKVEncoder(movie_name, in_img->width, in_img->height, fps, bin_files.size());
            }
            encoder->EncodeFrame(*in_img);
        }

        delete in_img;
        delete encoder;
    }
    else
    {
        std::cerr << "[Error] Cannot find path: " << argv[1] << std::endl;
        return 1;
    }

    return 0;
}

