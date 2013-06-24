#ifndef VS_IMGCONTROLLER_H
#define VS_IMGCONTROLLER_H

#include <visionsystem/controller.h>


using namespace visionsystem ;


class ImgController : public visionsystem::Controller
{
    public:

        ImgController( VisionSystem* vs, std::string configfile ) ;
        ~ImgController() ;

        void get_cameras(std::vector<GenericCamera *> & cams) {}
    private:

        bool pre_fct( std::vector<GenericCamera*> &cam ) ;
        void preloop_fct() ;
        void loop_fct() ;
        bool post_fct() ;

} ;







CONTROLLER(ImgController)


#endif
