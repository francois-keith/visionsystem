#ifndef UTILS_H
#define UTILS_H

#include <dc1394/dc1394.h>
#include <string>

std::string mode_2_string ( dc1394video_mode_t mode ) ;

std::string feature_2_string ( dc1394feature_t ft ) ;

std::string featuremode_2_string ( dc1394feature_mode_t ft ) ;



#endif
