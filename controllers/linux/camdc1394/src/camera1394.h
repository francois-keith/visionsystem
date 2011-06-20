#ifndef CAMERADC1394_H
#define CAMERADC1394_H

#include <vector>
#include <string>

#include <stdint.h>

#include <vision/image/imageref.h>

#include <visionsystem/genericcamera.h>
#include <visionsystem/frame.h>

#include <configparser/configparser.h>

#include <dc1394/dc1394.h>

using namespace std ;
using namespace configparser ;
using namespace visionsystem ;
using namespace vision;

class Camera1394 : public GenericCamera, public WithConfigFile
{

	public:

	Camera1394( dc1394_t* d, uint64_t gid ) ;
	~Camera1394() ;

	ImageRef    get_size() ;
	bool        is_active() ;
	FrameCoding get_coding() ;
	float       get_fps() ;
	string      get_name() ;

	void save_capa ( string filename ) ;
	
	private:

	void parse_config_line ( vector<string> &line ) ;

	bool	 	_active ;
	string		_name;
	uint64_t	_gid ;
	
	dc1394_t 				*handler ;
	dc1394camera_t 				    *cam ;	       
	dc1394video_mode_t 			    mode ;	
	dc1394framerate_t 		       framerate ;
	dc1394speed_t     			   speed ;
	int 					   flags ; 
	
	std::vector< std::vector<std::string> > features ;




};


#endif
