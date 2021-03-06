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
    std::cerr << "[Usage] " << exec_name << " [path to bin files] [MONO|COLOR]" << std::endl;
}

void convert_mono(std::vector<std::string> & bin_files, std::vector<std::string> & png_files);
void convert_color(std::vector<std::string> & bin_files, std::vector<std::string> & png_files);

void save_to_pgm(const std::string & fname, vision::Image<unsigned char, vision::MONO> * img)
{
    std::ofstream fs(fname.c_str(), std::ios::binary);
    fs << "P5" << std::endl;
    fs << img->width << " " << img->height << std::endl;
    fs << 255 << std::endl;
    for(unsigned int i = 0; i < img->pixels; ++i)
    {
        fs << img->raw_data[i];
    }
    fs.close();
}

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
                png_files.push_back(change_extension(bin_path, ".pgm").string());
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
                    png_files.push_back(change_extension(*it, ".pgm").string());
                }
            }
        }
        else
        {
            std::cerr << "[Error] " << argv[1] << " is neither a directory nor a regular file" << std::endl;
            return 1;
        }

        std::cout << "[bin2pgm] Will now convert " <<  bin_files.size() << " bin files to pgm." << std::endl;
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
            deserialize(bin_files[i], *in_img);
            save_to_pgm(png_files[i], in_img);
        }
        delete in_img;
}
void convert_color(std::vector<std::string> & bin_files, std::vector<std::string> & png_files)
{
        Image<uint32_t, RGB> * in_img = new Image<uint32_t, RGB>();
        for(size_t i = 0; i < bin_files.size(); ++i)
        {
            deserialize(bin_files[i], *in_img);
            save_color(png_files[i], in_img);
        }
        delete in_img;
}

