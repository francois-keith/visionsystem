#include "imgcontroller.h"

ImgController::ImgController( VisionSystem *vs, std::string configfile )
:Controller( vs, "ImgController", configfile ) {

}

ImgController::~ImgController() {

}

bool ImgController::pre_fct( std::vector<GenericCamera*> &cams ) {
    return true ;

}

void ImgController::preloop_fct() {
}

void ImgController::loop_fct() {

}


bool ImgController::post_fct() {
    return true ;
}


