#include "camerafilestream.h"

#include <vision/io/imageio.h>

#include <boost/filesystem.hpp>
#include <algorithm>

namespace bfs = boost::filesystem;

namespace visionsystem
{

CameraFilestream::CameraFilestream()
:   _img_size(0,0), _active(false), _img_coding(VS_MONO8), _fps(30), _frame(0), _name("filestream-unconfigured"), 
    _bin_files(0), _current_frame(0), _img_mono(0), _img_rgb(0), _img_depth(0),
    _buffersize(100)
{
    _previous_frame_t.tv_sec = 0;
    _previous_frame_t.tv_usec = 0;
}

CameraFilestream::~CameraFilestream()
{
    delete _img_mono;
    delete _img_rgb;
}

bool CameraFilestream::init_camera()
{
    bfs::path _img_path(_path);
    if( bfs::is_directory(_path) )
    {
        std::vector<bfs::path> ls_path;
        copy(bfs::directory_iterator(_img_path), bfs::directory_iterator(), back_inserter(ls_path));
        std::sort(ls_path.begin(), ls_path.end());
        for(std::vector< bfs::path >::const_iterator it = ls_path.begin(); it != ls_path.end(); ++it)
        {
            if( bfs::is_regular_file(*it) && bfs::extension(*it) == ".bin" )
            {
                _bin_files.push_back((*it).string());
            }
        }
    }
    else
    {
        std::stringstream errorss;
        errorss << "[CameraFilestream] Input path : " << _path << " is not a directory!";
        throw(errorss.str()); 
    }
    _buffer.clear();
    for( unsigned int i = 0; i < _buffersize; ++i )
    {
        _buffer.enqueue( new Frame( get_coding(), get_size() ) );
    }
    if( _img_coding == VS_MONO8 )
    {
        _img_mono = new vision::Image<unsigned char, vision::MONO>(get_size());
    }
    if( _img_coding == VS_RGB32  )
    {
        _img_rgb = new vision::Image<uint32_t, vision::RGB>(get_size());
    }
    if( _img_coding == VS_DEPTH16 )
    {
        _img_depth = new vision::Image<uint16_t, vision::DEPTH>(get_size());
    }
    return true;
}

bool CameraFilestream::has_data()
{
    timeval now;
    gettimeofday(&now, 0);
    unsigned int elapsed_time = 0;
    if(now.tv_usec < _previous_frame_t.tv_usec)
    {
        elapsed_time = 1000000*(now.tv_sec - _previous_frame_t.tv_sec) - (_previous_frame_t.tv_usec - now.tv_usec);
    }
    else
    {
        elapsed_time = 1000000*(now.tv_sec - _previous_frame_t.tv_sec) + (now.tv_usec - _previous_frame_t.tv_usec);
    }
    if(elapsed_time > _fps)
    {
        _previous_frame_t = now;
        return true;
    }
    return false;
}

unsigned char * CameraFilestream::get_data()
{
    _frame++;
    if( _img_coding == VS_MONO8 )
    {
        vision::deserialize(_bin_files[_current_frame], *_img_mono);
        _current_frame = (_current_frame + 1) % _bin_files.size();
        return _img_mono->raw_data;
    }
    else if( _img_coding == VS_RGB32 )
    {
        vision::deserialize(_bin_files[_current_frame], *_img_rgb);
        _current_frame = (_current_frame + 1) % _bin_files.size();
        return (unsigned char*)(_img_rgb->raw_data);
    }
    else( _img_coding == VS_DEPTH16 )
    {
        vision::deserialize(_bin_files[_current_frame], *_img_depth);
        _current_frame = (_current_frame + 1) % _bin_files.size();
        return (unsigned char*)(_img_rgb->raw_data);
    }
}

void CameraFilestream::parse_config_line( std::vector<std::string> & line )
{
    if( fill_member( line, "Name", _name ) )
        return;

    if( fill_member( line, "Active", _active ) )
        return;

    if( fill_member( line, "FPS", _fps ) )
    {
        _fps = 1000000/_fps;
        return;
    }

    std::vector<int> resolution(0);
    if( fill_member( line, "Resolution", resolution) )
    {
        if(resolution.size() > 1)
        {
            _img_size = vision::ImageRef(resolution[0], resolution[1]);
        }
        else
        {
            throw("[CameraFilestream] Error when reading resolution");
        }
        return;
    }

    std::string coding;
    if( fill_member( line, "ColorMode", coding ) )
    {
        if( coding == "MONO" )
        {
            _img_coding = VS_MONO8;
        }
        else if( coding == "RGB" )
        {
            _img_coding = VS_RGB32;
        }
        else if( coding == "DEPTH" )
        {
            _img_coding = VS_DEPTH16;
        }
        else
        {
            throw("[CameraFilestream] ColorMode not valid, set MONO or RGB (case sensitive)");
        }
        return;
    }

    if( fill_member( line, "Path", _path ) )
    {
        return;
    }

    if( fill_member( line, "BufferSize", _buffersize ) )
        return;
}

}

