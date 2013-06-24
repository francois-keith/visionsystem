#include "camera1394.h"

#include <iostream>

bool Camera1394::apply_settings() {

	dc1394error_t err ;

	err = dc1394_video_set_iso_speed ( cam, speed ) ;
	if ( err != DC1394_SUCCESS ) {
		cerr << "[camdc1394] could not set iso speed" << endl ;
		return false ;
	}

	err = dc1394_video_set_mode ( cam, mode ) ;
	if ( err != DC1394_SUCCESS ) {
		cerr << "[camdc1394] could not set mode" << endl ;
		return false ;
	}

	err = dc1394_video_set_framerate ( cam, framerate ) ;
	if ( err != DC1394_SUCCESS ) {
		cerr << "[camdc1394] could not set framerate" << endl ;
		return false ;
	}


	err = dc1394_capture_setup ( cam, 4, flags ) ;
	if ( err != DC1394_SUCCESS ) {
		cerr << "[camdc1394] could not set capture flags" << endl ;
		return false ;
	}


	err = dc1394_video_set_transmission ( cam, DC1394_ON ) ;
	if ( err != DC1394_SUCCESS ) {
		cerr << "[camdc1394] could not set transmission to ON" << endl ;
		return false ;
	}

	for (size_t i=0; i<features.size(); i++ ) {

		bool ok = false ;

		dc1394bool_t test ;
		dc1394error_t err ;
		dc1394feature_t feature ;
		string s_feature = features[i][0] ;
		string s_mode = features[i][1] ;
		unsigned int value, min, max ;
		unsigned int value2 ;
		unsigned int value3 ;

		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_BRIGHTNESS
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_BRIGHTNESS" ) {

			feature = DC1394_FEATURE_BRIGHTNESS ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_EXPOSURE
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_EXPOSURE" ) {

			feature = DC1394_FEATURE_EXPOSURE ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_SHARPNESS
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_SHARPNESS" ) {

			feature = DC1394_FEATURE_SHARPNESS ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}

		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_HUE
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_HUE" ) {

			feature = DC1394_FEATURE_HUE ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}

		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_SATURATION
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_SATURATION" ) {

			feature = DC1394_FEATURE_SATURATION ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_GAMMA
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_GAMMA" ) {

			feature = DC1394_FEATURE_GAMMA ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_SHUTTER
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_SHUTTER" ) {

			feature = DC1394_FEATURE_SHUTTER ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_GAIN
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_GAIN" ) {

			feature = DC1394_FEATURE_GAIN ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_IRIS
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_IRIS" ) {

			feature = DC1394_FEATURE_IRIS ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_FOCUS
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_FOCUS" ) {

			feature = DC1394_FEATURE_SHUTTER ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_set_value ( cam, feature, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** UNIMPLEMENTED FEATURES
		// ***
		// ***********************************************

		if ( ( s_feature == "DC1394_FEATURE_TRIGGER" ) ||
		     ( s_feature == "DC1394_FEATURE_TRIGGER_DELAY" ) ||
		     ( s_feature == "DC1394_FEATURE_FRAMERATE" ) ||
		     ( s_feature == "DC1394_FEATURE_ZOOM" ) ||
		     ( s_feature == "DC1394_FEATURE_PAN" ) ||
		     ( s_feature == "DC1394_FEATURE_TILT" ) ||
		     ( s_feature == "DC1394_FEATURE_OPTICAL_FILTER" ) ||
		     ( s_feature == "DC1394_FEATURE_CAPTURE_SIZE" ) ||
		     ( s_feature == "DC1394_FEATURE_CAPTURE_QUALITY" ) )  {

			cerr << "[camdc1394] Camera " << _gid << " : Feature " << s_feature << " is not implemented yet. Aborting." << endl ;
			return false ;

		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_WHITE_BALANCE
		// ***           ( two args : Ub and Ur )
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_WHITE_BALANCE" ) {

			feature = DC1394_FEATURE_WHITE_BALANCE ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 4 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : " << s_feature << "needs two arguments. "<< endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				value2 = atoi( features[i][3].c_str() ) ;
				err = dc1394_feature_whitebalance_set_value ( cam, value, value2 ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set values" << value <<" " << value2 << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_WHITE_SHADING
		// ***           ( three args : R G and B )
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_WHITE_SHADING" ) {

			feature = DC1394_FEATURE_WHITE_SHADING ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 5 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : " << s_feature << "needs three arguments. "<< endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				value2 = atoi( features[i][3].c_str() ) ;
				value3 = atoi( features[i][3].c_str() ) ;
				err = dc1394_feature_whiteshading_set_value ( cam, value, value2, value3 ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value "
						<< value << " " << value2 << " " << value3 << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// ***********************************************
		// ***
		// *** FEATURE : DC1394_FEATURE_TEMPERATURE
		// ***		  has its own set_value function in dc1394 ... why ???
		// ***
		// ***********************************************

		if ( s_feature == "DC1394_FEATURE_TEMPERATURE" ) {

			feature = DC1394_FEATURE_TEMPERATURE ;

			dc1394_feature_is_present( cam, feature, &test ) ;
			if (!test) {
				cerr << "[camdc1394] Camera1394 : feature " << s_feature << "is not supported by cam " << _gid << endl ;
				return false ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_AUTO" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_AUTO ) ;
				if (err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
			if ( s_mode == "DC1394_FEATURE_MODE_MANUAL" ) {
				err = dc1394_feature_set_mode( cam, feature, DC1394_FEATURE_MODE_MANUAL ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set mode " << s_mode << " for feature " << s_feature << endl ;
					return false ;
				}
				if ( features[i].size() != 3 ) {
					cerr << "[camdc1394] Camera "<< _gid << " : Wrong number of arguments for feature " << s_feature << endl ;
					return false ;
				}
				value = atoi( features[i][2].c_str() ) ;
				err=dc1394_feature_get_boundaries( cam, feature, &min, &max);
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not retrieve boundaries for feature " << s_feature << endl ;
					return false ;
				}
				if (( value < min ) || ( value > max ) ) {
					cerr << "[camdc1394] Camera "<< _gid << " : value out of boundaries for feature " << s_feature << endl ;
					return false ;
				}
				err = dc1394_feature_temperature_set_value ( cam, value ) ;
				if ( err != DC1394_SUCCESS ) {
					cerr << "[camdc1394] Camera "<< _gid << " : could not set value " << value << " for feature " << s_feature << endl ;
					return false ;
				}
				ok = 1 ;
			}
		}


		// *******************************************
		// ***
		// *** Unknown line
		// ***
		// *******************************************

		if (ok != 1 ) {

			cerr << "[camdc1394] Camera " << _gid << " : Problem when parsing the file " << endl ;
			cerr << "aborting." << endl ;
			return false ;
		}

	}

	// *******************************************
	// ***
	// *** Filling Buffer
	// ***
	// *******************************************


	_buffer.clear() ;
	for ( int i=0; i< _buffersize; i++ ){
		_buffer.enqueue ( new Frame( get_coding(), get_size() ) ) ;
	}

	return true ;
}





