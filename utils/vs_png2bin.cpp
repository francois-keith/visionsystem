#include <vision/io/imageio.h>
#include <boost/filesystem.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace vision;
using namespace boost::filesystem;

void usage(const char * exec_name)
{
    std::cerr << "[Usage] " << exec_name << " [path to png files] [MONO|COLOR]" << std::endl;
}

void convert_mono(std::vector<std::string> & bin_files, std::vector<std::string> & png_files);
void convert_color(std::vector<std::string> & bin_files, std::vector<std::string> & png_files);

enum C_MODE
{
    C_MONO,
    C_COLOR
};

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        usage(argv[0]);
        return 1;
    }
    C_MODE mode = C_MONO;
    if(argc > 2 && std::string(argv[2]) == "COLOR")
    {
        mode = C_COLOR;
    }

    path png_path = path(argv[1]);
    if( exists(png_path) )
    {
        std::vector<std::string> bin_files;
        std::vector<std::string> png_files;
        if( is_regular_file(png_path) )
        {
            std::string ext = extension(png_path);
            if(ext != ".png")
            {
                std::cerr << "[Error] " << argv[1] << " isn't a .png file" << std::endl;
                return 1;
            }
            else
            {
                png_files.push_back(png_path.string());
                bin_files.push_back(change_extension(png_path, ".bin").string());
            }
        }
        else if( is_directory(png_path) )
        {
            std::vector<path> ls_path;
            copy(directory_iterator(png_path), directory_iterator(), back_inserter(ls_path));
            for(std::vector<path>::const_iterator it = ls_path.begin(); it != ls_path.end(); ++it)
            {
                if( is_regular_file(*it) && extension(*it) == ".png" )
                {
                    png_files.push_back((*it).string());
                    bin_files.push_back(change_extension(*it, ".bin").string());
                }
            }
        }
        else
        {
            std::cerr << "[Error] " << argv[1] << " is neither a directory nor a regular file" << std::endl;
            return 1;
        }
        
        std::cout << "[png2bin] Will now convert " <<  png_files.size() << " png files to bin." << std::endl;
        if(mode == C_MONO)
        {
            convert_mono(bin_files, png_files);
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
        Image<unsigned char, MONO> * in_img = new Image<unsigned char, MONO>();
        for(size_t i = 0; i < bin_files.size(); ++i)
        {
            in_img = load_mono<unsigned char, MONO>(png_files[i]);
            serialize(bin_files[i], *in_img);
        }
        delete in_img;
}
void convert_color(std::vector<std::string> & bin_files, std::vector<std::string> & png_files)
{
        Image<uint32_t, RGB> * in_img = new Image<uint32_t, RGB>();
        for(size_t i = 0; i < bin_files.size(); ++i)
        {
            in_img = load_color<uint32_t, RGB>(png_files[i]);
            serialize(bin_files[i], *in_img);
        }
        delete in_img;
}

