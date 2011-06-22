#include "camera1394.h"
#include "utils.h"

// **************************************************
// ****
// **** Save Capabilities to file
// ****
// **************************************************


void Camera1394::save_capa ( string filename ) {

    dc1394video_modes_t modes;
    dc1394error_t err ;
	    
    bool framerates ;
    string modename ;
    string frate ;

    err=dc1394_video_get_supported_modes(cam, &modes);

    if ( err != DC1394_SUCCESS ) {
    	cerr << "[vision_server]	Could not get list of modes" ;
	return ;
    }

    ofstream capafile ( filename.c_str() , ios::out ) ;

    for ( int i=0; i<modes.num; i++) {

	framerates = true ;

	if ( ( modes.modes[i] ==  DC1394_VIDEO_MODE_EXIF ) ||
		 ( modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_0 ) ||
		 ( modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_1 ) ||
		 ( modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_2 ) ||
		 ( modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_3 ) ||
		 ( modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_4 ) ||
		 ( modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_5 ) ||
		 ( modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_6 ) ||
		 ( modes.modes[i] == DC1394_VIDEO_MODE_FORMAT7_7 ) ) 
			framerates = false ;


	capafile << "[Mode] " << mode_2_string ( modes.modes[i] ) << endl ;


	if (framerates) {

		dc1394framerates_t framerates;
	
		err = dc1394_video_get_supported_framerates (cam, modes.modes[i], &framerates);
		if ( err != DC1394_SUCCESS ) {	
			cerr << "[vision_server] ERROR : could not retrieve framerate" << endl ;
			exit(0) ;
		}
			
		for ( int j = 0; j < framerates.num; j++) {
			dc1394framerate_t rate = framerates.framerates[j];
			switch (rate) {
				case DC1394_FRAMERATE_1_875 :
					frate = "DC1394_FRAMERATE_1_875" ;
					break ;
				case DC1394_FRAMERATE_3_75 :
					frate = "DC1394_FRAMERATE_3_75" ;
					break ;
				case DC1394_FRAMERATE_7_5 :
					frate = "DC1394_FRAMERATE_7_5" ;
					break ;
				case DC1394_FRAMERATE_15 :
					frate = "DC1394_FRAMERATE_15" ;
					break ;
				case DC1394_FRAMERATE_30 :
					frate = "DC1394_FRAMERATE_30" ;
					break ;
				case DC1394_FRAMERATE_60 :
					frate = "DC1394_FRAMERATE_60" ;
					break ;
				case DC1394_FRAMERATE_120 :
					frate = "DC1394_FRAMERATE_120" ;
					break ;
				case DC1394_FRAMERATE_240 :
					frate = "DC1394_FRAMERATE_240" ;
					break ;
				default :
					frate ="Unknown framerate" ;
					break ;

			}

			capafile << "[Framerate] " << frate << endl ;
		}
	}

	capafile << endl ;

}

	// Enumerates features    

	dc1394featureset_t  features ;

	dc1394_feature_get_all( cam, &features );

	for ( int i=0; i<DC1394_FEATURE_NUM; i++ ) {
		if ( features.feature[i].available ) {

				capafile << std::endl << "[Feature] " << feature_2_string ( features.feature[i].id ) << std::endl ;

				for (int j=0; j<features.feature[i].modes.num; j++ ) {
					
					capafile << "  Mode : " << featuremode_2_string ( features.feature[i].modes.modes[j] ) << std::endl ;

					if (features.feature[i].modes.modes[j] == DC1394_FEATURE_MODE_MANUAL ) {				
						
							uint32_t min = 0 ; 
							uint32_t max = 0 ;
						
							err = dc1394_feature_get_boundaries( cam, features.feature[i].id , &min, &max);
						
							if ( err != DC1394_SUCCESS ) {
								cerr << "[vision_server] ERROR : Could not get boudaries for mode " << featuremode_2_string ( features.feature[i].modes.modes[j] ) << endl ;
								exit(0) ;
							}
						
							capafile << "  Value : " << min << " - " << max << std::endl ;
						}
				}

		}

	}


	// close file 

	capafile.close() ;

}



