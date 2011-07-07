#include "camerav4l2.h"

#include <iostream>
#include <visionsystem/frame.h>
#include <fcntl.h>             
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>

#include <asm/types.h>          /* for videodev2.h */

#include <cstring>

// -------------------
// ---
// ---  Misc tools
// ---
// -------------------


template<typename T>
inline void CLEAR(T x)
{
    memset((void*)&x, 0, sizeof(x));
}

static int xioctl ( int fd, int request, void * arg ) {
        
	int r;
        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;

}

// -----------------------
// ---
// --- CameraV4L2 methods
// ---
// -----------------------


CameraV4L2::CameraV4L2( string dev )
{
	_name = dev ;
	_fps = 0 ;
	_active = 1 ;

	_dev_name = dev ;
	_io = IO_METHOD_MMAP;
	_fd = -1;

	buffers = NULL;
	n_buffers = 0;

	CLEAR (_fmt);
        _fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	_fmt.fmt.pix.width       = 640; 
        _fmt.fmt.pix.height      = 480;
	_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV ;
	_fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED ;


}

CameraV4L2::~CameraV4L2() {

}


ImageRef CameraV4L2::get_size() {
	return ImageRef( _fmt.fmt.pix.width, _fmt.fmt.pix.height ) ;
}


bool CameraV4L2::is_active() {
	return _active ;
}	


FrameCoding CameraV4L2::get_coding() {
	
	switch( _fmt.fmt.pix.pixelformat ) {
	
		case V4L2_PIX_FMT_YUYV :

			return VS_YUV422_YUYV ;
			break ;

        default:
            return VS_YUV422_YUYV;
            break;
	}
}


float CameraV4L2::get_fps() {
	return _fps ;
}


string CameraV4L2::get_name() {
	return _name ;
}

void CameraV4L2::parse_config_line ( vector<string> &line ) {
	fill_member<string>( line, "Name" ,  _name ) ;
	fill_member<bool>( line, "Active", _active ) ;
}

bool CameraV4L2::init_camera() {

	struct stat st; 
	unsigned int min;
        
	if (-1 == stat ( _dev_name.c_str(), &st ) ) {
                cerr << "[camv4l2] ERROR: Cannot identify " << _dev_name << " : " << strerror( errno ) << endl ;
                return false ;
        }

        if (!S_ISCHR (st.st_mode)) {
                cerr << "[camv4l2] ERROR: " << _dev_name << " is no device " << endl ;
        	return false ;
	}

        _fd = open ( _dev_name.c_str() , O_RDWR | O_NONBLOCK, 0);

        if (-1 == _fd) {

                cerr << "[camv4l2] ERROR: Cannot open " << _dev_name << " " << errno << " : " << strerror(errno) << endl ; 
                return false ;
        }

        if ( -1 == xioctl ( _fd, VIDIOC_QUERYCAP, &_cap) ) {
                if (EINVAL == errno) {
                        cerr << "[camv4l2] ERROR: " << _dev_name << " is no V4L2 device " << endl ;
                        return false ;
                } else {
                        cerr << "[camv4l2] ERROR: VIDIOC_QUERYCAP" << endl ;
			return false ;
                }
        }

        if ( !(_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ) {
                cerr << "[camv4l2] ERROR: " << _dev_name << " is not a video capture device " << endl ;
                return false ;
        }

	switch ( _io ) {
		
		case IO_METHOD_READ:
			
			if ( !(_cap.capabilities & V4L2_CAP_READWRITE) ) {
				cerr << "[camv4l2] ERROR: " << _dev_name << " does not support read i/o" << endl ;
				return false ;
			}

			break;

		case IO_METHOD_MMAP:
		case IO_METHOD_USERPTR:
			
			if ( !(_cap.capabilities & V4L2_CAP_STREAMING) ) {
				cerr << "[camvv4l2] ERROR: " << _dev_name << " does not support streaming i/o" << endl ;
				return false ;
			}

			break;
	}


        /* Select video input, video standard and tune here. */

	CLEAR ( _cropcap );

        _cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if ( 0 == xioctl (_fd, VIDIOC_CROPCAP, &_cropcap) ) {
                
		_crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                _crop.c = _cropcap.defrect; 

                if (-1 == xioctl ( _fd, VIDIOC_S_CROP, &_crop)) {
                        
			switch (errno) {
                  	      case EINVAL:
                                					// FIXME Cropping not supported
					break;
                        
				default:
                                					// FIXME Errors ignored
                                	break;
                        }
                }

        } else {	
                
		// FIXME Errors ignored. 
        }


                if ( -1 == xioctl ( _fd, VIDIOC_S_FMT, &_fmt) ) {
               
	    	cerr << "[camv4l2] ERROR: VIDIOC_S_FMT failed for " << _dev_name << endl ;
		return false ;
	}

	// Note VIDIOC_S_FMT may change width and height. 

	// Buggy driver paranoia. 
	
	min = _fmt.fmt.pix.width * 2;
	if (_fmt.fmt.pix.bytesperline < min)
		_fmt.fmt.pix.bytesperline = min;
	
	min = _fmt.fmt.pix.bytesperline * _fmt.fmt.pix.height;
	if (_fmt.fmt.pix.sizeimage < min)
		_fmt.fmt.pix.sizeimage = min;

	// initialize v4L2 buffers

	switch ( _io ) {
	
		case IO_METHOD_READ:
			
			if ( !init_read () )
				return false ;
			break;

		case IO_METHOD_MMAP:
			
			if ( !init_mmap () )
				return false ;
			break;

		case IO_METHOD_USERPTR:
			
			if ( !init_userp() )  
				return false ;
			break;
	
	}

	// Preparing the buffer ;
	
	_buffer.clear() ;

	for ( int i=0; i< 50; i++ ) {
	
		Frame* frm = new Frame( get_coding(), get_size() ) ;
		_buffer.enqueue( frm ) ;

	}

	// Start capturing
	
        unsigned int i;
        enum v4l2_buf_type type;

	switch ( _io ) {
		case IO_METHOD_READ:
				
				/* Nothing to do. */
				break;

		case IO_METHOD_MMAP:
	
				for (i = 0; i < n_buffers; ++i) {
					
					struct v4l2_buffer buf;
					CLEAR (buf);
					buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					buf.memory = V4L2_MEMORY_MMAP;
					buf.index = i;

					if (-1 == xioctl ( _fd, VIDIOC_QBUF, &buf)) {
						cerr << "[camv4l2] ERROR : VIDIOC_QBUF failed." << endl ;
						return false ;
					}
				}	
				
				type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

				if (-1 == xioctl (_fd, VIDIOC_STREAMON, &type)) {
					cerr << "[camv4l2] ERROR : VIDIOC_STREAMON failed." << endl ;
					return false ;
				}
				
				break;

		case IO_METHOD_USERPTR:
	
				for (i = 0; i < n_buffers; ++i) {

					struct v4l2_buffer buf;

					CLEAR (buf);

					buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					buf.memory = V4L2_MEMORY_USERPTR;
					buf.index = i;
					buf.m.userptr = (unsigned long) buffers[i].start;
					buf.length = buffers[i].length;

					if (-1 == xioctl ( _fd, VIDIOC_QBUF, &buf)) {
						cerr << "[camv4l2] ERROR : VIDIOC_QBUF failed." << endl ;
						return false ;
					}
				}

				type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

				if ( -1 == xioctl ( _fd, VIDIOC_STREAMON, &type) ) {
					cerr << "[camv4l2] ERROR : VIDIOC_STREAMON failed." << endl ;
					return false ;
				}

				break;
	}

	return true ;

}


bool CameraV4L2::init_mmap() {

	struct v4l2_requestbuffers req;

        CLEAR (req);

        req.count               = 4;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_MMAP;

	if (-1 == xioctl ( _fd, VIDIOC_REQBUFS, &req)) {
                
		if (EINVAL == errno) {

                        cerr << "[camv4l2] ERROR: " << _dev_name <<  " does not support IO_METHOD_MMAP" << endl ;
                 	return false ;
		 } else {
                        
			cerr <<  "[camv4l2] ERROR: error occured in VIDIOC_REQBUFS" << endl ;
   			return false ;             
		 }
        }

        if ( req.count < 2 ) {

		cerr << "[camv4l2] ERROR: Insufficient buffer memory on " << _dev_name << endl ;
		return false ;

	}

        buffers = (struct buffer*) calloc ( req.count, sizeof (*buffers) );

        if ( !buffers ) {
                cerr << "[camv4l2] ERROR: Out of memory" << endl ;
                return false ;
        }

        for ( n_buffers = 0; n_buffers < req.count; ++n_buffers ) {
                
		struct v4l2_buffer buf;
                CLEAR (buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if ( -1 == xioctl ( _fd, VIDIOC_QUERYBUF, &buf ) ) {
                        cerr << "[camv4l2] ERROR: error occured in VIDIOC_QUERYBUF call" << endl ;
			return false ;
		}

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
			                        mmap (NULL /* start anywhere */,
                        			      buf.length,
			                              PROT_READ | PROT_WRITE /* required */,
                        			      MAP_SHARED /* recommended */,
                             			      _fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start) {
                        cerr << "[camv4l2] ERROR: mmap failed. " << endl ;
			return false ;
		}
        }

	return true ;

}



bool CameraV4L2::init_read ()
{
        buffers = (struct buffer*) calloc (1, sizeof (*buffers));

        if (!buffers) {
                cerr << "[camv4l2] ERROR: init_read() Out of memory" << endl ;
                return false ;
        }

	buffers[0].length = _fmt.fmt.pix.sizeimage ;
	buffers[0].start = malloc ( _fmt.fmt.pix.sizeimage );

	if ( !buffers[0].start ) {
                cerr << "[camv4l2] ERROR: init_read() Out of memory" << endl ;
		return false ;
	}

	return true ;
}


bool CameraV4L2::init_userp()
{
	struct v4l2_requestbuffers req;
        unsigned int page_size;

        page_size = getpagesize ();
        unsigned int buffer_size = ( _fmt.fmt.pix.sizeimage + page_size - 1) & ~(page_size - 1);

        CLEAR (req);

        req.count               = 4;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_USERPTR;

        if ( -1 == xioctl ( _fd, VIDIOC_REQBUFS, &req ) ) {
                if (EINVAL == errno) {

                        cerr << "[camv4l2] ERROR: " << _dev_name << "does not support IO_METHOD_USERPTR" << endl ; 
                        return false ;
                
		} else {
                
			cerr << "[camv4l2] ERROR: " << _dev_name << " Error in VIDIOC_REQBUFS " << endl ;
			return false ;
                }
        }

        buffers = (struct buffer*) calloc (4, sizeof (*buffers));

        if (!buffers) {
                cerr << "[camv4l2] ERROR in init_userp() : Out of memory" << endl ;
                return false ;
        }

        for ( n_buffers = 0; n_buffers < 4; ++n_buffers ) {
               
	        buffers[n_buffers].length = buffer_size;
                buffers[n_buffers].start = memalign ( page_size, buffer_size );

                if ( !buffers[n_buffers].start) {
    			cerr << "[camv4l2] ERROR in init_userp() : Out of memory " << endl ;
            		return false ;
		}
        }

	return true ;
}


int CameraV4L2::read_frame () {

	struct v4l2_buffer buf;
	unsigned int i;

	switch (_io) {
	
		case IO_METHOD_READ:
    			if (-1 == read (_fd, buffers[0].start, buffers[0].length)) {
            			switch (errno) {
            				case EAGAIN:
                    				return 0;

					case EIO:
						
						/* Could ignore EIO, see spec. */
						/* fall through */

					default:
						cerr << "[camv4l2] ERROR in read_frame() " << endl ;
						return -1 ;
				}
			}

    			process_image (buffers[0].start);

			break;


		case IO_METHOD_MMAP:
			
			CLEAR (buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;

			if (-1 == xioctl (_fd, VIDIOC_DQBUF, &buf)) {
				switch (errno) {
					case EAGAIN:
						return 0;

					case EIO:
						/* Could ignore EIO, see spec. */
						/* fall through */

					default:
						cerr << "[camv4l2] ERROR in read_frame() : VIDIOC_DQBUF" << endl ;
						return -1 ;
				}
			}

			assert (buf.index < n_buffers);

			process_image (buffers[buf.index].start);

			if (-1 == xioctl (_fd, VIDIOC_QBUF, &buf)) {
				cerr << "[camv4l2] ERROR in read_frame() : VIDIOC_QBUF" << endl ;
				return -1 ;
			}

			break;


		case IO_METHOD_USERPTR:
			
			CLEAR (buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_USERPTR;

			if (-1 == xioctl (_fd, VIDIOC_DQBUF, &buf)) {
				switch (errno) {
					case EAGAIN:
						return 0;

					case EIO:
						/* Could ignore EIO, see spec. */
						/* fall through */

					default:
						cerr << "[camv4l2] ERROR in read_frame() : VIDIOC_DQBUF" << endl ;
						return -1 ;
				}
			}

			for (i = 0; i < n_buffers; ++i)
				if (  ( buf.m.userptr == (unsigned long) buffers[i].start )
				   && ( buf.length == buffers[i].length ) )
					break;

			assert (i < n_buffers);

			process_image ((void *) buf.m.userptr);

			if ( -1 == xioctl (_fd, VIDIOC_QBUF, &buf) ) {
				cerr << "[camv4l2] ERROR in read_frame() : VIDIOC_DQBUF" << endl ;
				return -1 ;
}

			break;
	}

	return 1 ;

}


void CameraV4L2::process_image ( const void* p ) {

	Frame* frm = _buffer.pull() ;

	memcpy ( frm->_data, p, frm->_data_size ) ;	

	_buffer.push( frm ) ;


}

bool CameraV4L2::stop_camera() {

// Stop capturing

	enum v4l2_buf_type type;

	switch ( _io ) {
		
		case IO_METHOD_READ:
		
			/* Nothing to do. */
			break;

		case IO_METHOD_MMAP:
		case IO_METHOD_USERPTR:
		
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			if (-1 == xioctl ( _fd, VIDIOC_STREAMOFF, &type)) {
				cerr << "[camv4l2] ERROR in stop_camera() : VIDIOC_STREAMOFF" << endl ;
				return 0 ;
			}

			break;
	}

// Uninit Camera

	unsigned int i;

	switch ( _io ) {
	
		case IO_METHOD_READ:
			
			free (buffers[0].start);
			break;

		case IO_METHOD_MMAP:
		
			for (i = 0; i < n_buffers; ++i)
				if (-1 == munmap (buffers[i].start, buffers[i].length)) {
					cerr << "[camv4l2] ERROR in stop_camera() : munmap failed." << endl ;
					return 0 ;
				}
			break;

		case IO_METHOD_USERPTR:

			for (i = 0; i < n_buffers; ++i)
				free (buffers[i].start);
			break;
	}

	free (buffers);

// Close Camera


	if ( -1 == close (_fd) ) {
		cerr << "[camv4l2] ERROR : cannot close " << _dev_name << endl ;
		return 0 ;
	}

	_fd = -1;

    return true;
}
