#ifndef CAMERAV4L2_H
#define CAMERAV4L2_H

#include <configparser/configparser.h>
#include <vision/image/imageref.h>
#include <visionsystem/genericcamera.h>
#include <visionsystem/frame.h>
#include <string>

#include <linux/videodev2.h>

using namespace std ;
using namespace configparser ;
using namespace visionsystem ;
using namespace vision;

class CameraV4L2 : public GenericCamera, public WithConfigFile
{

	public:

	CameraV4L2( string dev ) ;
	~CameraV4L2() ;

	ImageRef    get_size() ;
	bool        is_active() ;
	FrameCoding get_coding() ;
	float       get_fps() ;
	string      get_name() ;
	
	public:

	bool    init_camera() ;
	int     read_frame() ;		
	bool    stop_camera() ;	
	
	private:

	void parse_config_line( vector<string> &line ) ;

	private:

	bool	 	_active ;
	float		_fps ;
	string		_name ;

	private:


	typedef enum {
		IO_METHOD_READ,
		IO_METHOD_MMAP,
		IO_METHOD_USERPTR,
	} io_method;


	struct buffer {
        	void *                  start;
	        size_t                  length;
	};


	string 		   _dev_name ; 	        // Name of the device
	io_method	   _io ;		// IO method
	int                _fd ;                // File descriptor
        
	struct v4l2_capability _cap;
        struct v4l2_cropcap    _cropcap;
        struct v4l2_crop       _crop;
	struct v4l2_format     _fmt;

	struct buffer*	 buffers ;        
	unsigned int     n_buffers ;

	bool init_read() ;
	bool init_mmap() ;
	bool init_userp() ;
	void process_image( const void* p) ;
};


#endif
