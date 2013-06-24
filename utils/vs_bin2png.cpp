#include <vision/io/imageio.h>
#include <boost/filesystem.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define MAX_DEPTH 10000

using namespace vision;
using namespace boost::filesystem;

void usage(const char * exec_name)
{
    std::cerr << "[Usage] " << exec_name << " [path to bin files] [MONO|COLOR|DEPTH]" << std::endl;
    std::cerr << "(default to MONO)" << std::endl;
}

void convert_mono(std::vector<std::string> & bin_files, std::vector<std::string> & png_files);
void convert_color(std::vector<std::string> & bin_files, std::vector<std::string> & png_files);
void convert_depth(std::vector<std::string> & bin_files, std::vector<std::string> & png_files);

enum C_MODE
{
    C_MONO,
    C_COLOR,
    C_DEPTH
};

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        usage(argv[0]);
        return 1;
    }
    C_MODE mode = C_MONO;
    if(argc > 2)
    {
        if(std::string(argv[2]) == "COLOR")
        {
            mode = C_COLOR;
        }
        if(std::string(argv[2]) == "DEPTH")
        {
            mode = C_DEPTH;
        }
    }

    path bin_path = path(argv[1]);
    if( exists(bin_path) )
    {
        std::vector<std::string> bin_files;
        std::vector<std::string> png_files;
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
                png_files.push_back(change_extension(bin_path, ".png").string());
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
                    png_files.push_back(change_extension(*it, ".png").string());
                }
            }
        }
        else
        {
            std::cerr << "[Error] " << argv[1] << " is neither a directory nor a regular file" << std::endl;
            return 1;
        }

        std::cout << "[bin2png] Will now convert " <<  bin_files.size() << " bin files to png." << std::endl;
        if(mode == C_MONO)
        {
            convert_mono(bin_files, png_files);
        }
        else if(mode == C_DEPTH)
        {
            convert_depth(bin_files, png_files);
        }
        else
        {
            convert_color(bin_files, png_files);
        }
    }
    else
    {
        std::cerr << "[Error] Cannot find path: " << argv[1] << std::endl;
        return 1;
    }

    return 0;
}

void convert_mono(std::vector<std::string> & bin_files, std::vector<std::string> & png_files)
{
        #pragma omp parallel for
        for(size_t i = 0; i < bin_files.size(); ++i)
        {
            Image<unsigned char, MONO> * in_img = new Image<unsigned char, MONO>();
            deserialize(bin_files[i], *in_img);
            save_mono(png_files[i], in_img);
            delete in_img;
            #pragma omp critical
            {
                std::cout << "\r" << i+1 << "/" << bin_files.size() << std::flush;
            }
        }
        std::cout << std::endl;
}
void convert_color(std::vector<std::string> & bin_files, std::vector<std::string> & png_files)
{
        #pragma omp parallel for
        for(size_t i = 0; i < bin_files.size(); ++i)
        {
            Image<uint32_t, RGB> * in_img = new Image<uint32_t, RGB>();
            deserialize(bin_files[i], *in_img);
            save_color(png_files[i], in_img);
            delete in_img;
            #pragma omp critical
            {
                std::cout << "\r" << i+1 << "/" << bin_files.size() << std::flush;
            }
        }
        std::cout << std::endl;
}

void convert_depth(std::vector<std::string> & bin_files, std::vector<std::string> & png_files)
{
    #pragma omp parallel for
    for(size_t i = 0; i < bin_files.size(); ++i)
    {
        Image<uint16_t, DEPTH> * in = new Image<uint16_t, DEPTH>();
        deserialize(bin_files[i], *in);
        save_color(png_files[i], in);
        delete in;
        #pragma omp critical
        {
            std::cout << "\r" << i+1 << "/" << bin_files.size() << std::flush;
        }
    }
    std::cout << std::endl;
}
