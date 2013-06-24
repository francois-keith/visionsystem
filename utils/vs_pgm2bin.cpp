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
    std::cerr << "[Usage] " << exec_name << " [path to pgm files]" << std::endl;
}

void convert_mono(std::vector<std::string> & bin_files, std::vector<std::string> & pgm_files);

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    path pgm_path = path(argv[1]);
    if( exists(pgm_path) )
    {
        std::vector<std::string> bin_files;
        std::vector<std::string> pgm_files;
        if( is_regular_file(pgm_path) )
        {
            std::string ext = extension(pgm_path);
            if(ext != ".pgm")
            {
                std::cerr << "[Error] " << argv[1] << " isn't a .pgm file" << std::endl;
                return 1;
            }
            else
            {
                pgm_files.push_back(pgm_path.string());
                bin_files.push_back(change_extension(pgm_path, ".bin").string());
            }
        }
        else if( is_directory(pgm_path) )
        {
            std::vector<path> ls_path;
            copy(directory_iterator(pgm_path), directory_iterator(), back_inserter(ls_path));
            for(std::vector<path>::const_iterator it = ls_path.begin(); it != ls_path.end(); ++it)
            {
                if( is_regular_file(*it) && extension(*it) == ".pgm" )
                {
                    pgm_files.push_back((*it).string());
                    bin_files.push_back(change_extension(*it, ".bin").string());
                }
            }
        }
        else
        {
            std::cerr << "[Error] " << argv[1] << " is neither a directory nor a regular file" << std::endl;
            return 1;
        }

        std::cout << "[pgm2bin] Will now convert " <<  pgm_files.size() << " pgm files to bin." << std::endl;
        convert_mono(bin_files, pgm_files);
    }
    else
    {
        std::cerr << "[Error] Cannot find path: " << argv[1] << std::endl;
        return 1;
    }

    return 0;
}

void convert_mono(std::vector<std::string> & bin_files, std::vector<std::string> & pgm_files)
{
        for(size_t i = 0; i < bin_files.size(); ++i)
        {
            std::ifstream fs(pgm_files[i].c_str(), std::ios::binary);
            std::string line;
            fs >> line;
            if(line != "P5")
            {
                std::cerr << "pgm format " << line << " in " << pgm_files[i] << " not handled by this util" << std::endl;
                continue;
            }
            unsigned int width, height;
            fs >> width;
            fs >> height;
            Image<unsigned char, MONO> * in_img = new Image<unsigned char, MONO>(width, height);
            unsigned int colordepth;
            fs >> colordepth;
            fs.read((char*)in_img->raw_data, width*height);
            serialize(bin_files[i], *in_img);
            delete in_img;
        }
}

