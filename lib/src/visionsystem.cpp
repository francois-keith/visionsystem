#include <visionsystem/visionsystem.h>
#include <vector>

using namespace std ;


namespace visionsystem {

VisionSystem::VisionSystem() {

}


VisionSystem::~VisionSystem() {
}

vector<Camera*> VisionSystem::get_all_cameras() {
    _cameras_mutex.lock() ;
    vector<Camera*> tmp ;
    for ( size_t i=0; i<_cameras.size(); i++ )
        tmp.push_back( (Camera*) _cameras[i] ) ;
    _cameras_mutex.unlock() ;
    return tmp ;
}

Camera* VisionSystem::get_camera ( string name ) {
    _cameras_mutex.lock() ;
    Camera* tmp = NULL ;
    for ( size_t i=0; i<_cameras.size(); i++ )
        if ( _cameras[i]->get_name() == name )
            tmp = (Camera*) _cameras[i] ;
    _cameras_mutex.unlock() ;
    return tmp ;
}

Camera* VisionSystem::get_default_camera() {
    _cameras_mutex.lock() ;
    for(size_t i = 0; i < _cameras.size(); ++i)
    {
        if(_cameras[i]->is_active())
        {
            Camera * res = _cameras[i];
            _cameras_mutex.unlock();
            return res;
        }
    }
    _cameras_mutex.unlock() ;
    return 0 ;
}


void VisionSystem::register_to_cam ( Plugin* plugin, Camera* cam ) {
    _subscriptions_mutex.lock() ;
    _subscriptions[cam].push_back( plugin ) ;
    _subscriptions_mutex.unlock() ;
}


void VisionSystem::unregister_to_cam ( Plugin* plugin, Camera* cam ) {
    _subscriptions_mutex.lock() ;
    for (size_t i=0; i<_subscriptions[cam].size(); i++ )
        if ( _subscriptions[cam][i] == plugin )
            _subscriptions[cam].erase( _subscriptions[cam].begin() + i ) ;
    _subscriptions_mutex.unlock() ;
}


void VisionSystem::add_camera( GenericCamera* cam ) {
    string name = cam->get_name() ;
    _cameras_mutex.lock() ;
    for ( size_t i=0; i< _cameras.size(); i++ )
        if ( _cameras[i]->get_name() == name )
            throw ( string("Cannot add camera - A camera name must be unique" ) ) ;
    _cameras.push_back( cam ) ;
    _cameras_mutex.unlock() ;
}

vector<GenericCamera*> VisionSystem::get_all_genericcameras() {
    vector<GenericCamera*> tmp ;
    _cameras_mutex.lock() ;
    tmp = _cameras ;
    _cameras_mutex.unlock() ;
    return tmp ;
}


vector<Plugin*> VisionSystem::get_all_subscriptions( GenericCamera* cam ) {
    vector<Plugin*> tmp ;
    _subscriptions_mutex.lock() ;
    tmp = _subscriptions[cam]  ;
    _subscriptions_mutex.unlock() ;
    return tmp ;
}


void VisionSystem::whiteboard_wipe()
{
    for( std::map< std::string, boost::recursive_mutex*>::iterator it = whiteboard_mutex.begin();
            it != whiteboard_mutex.end();
            ++it )
    {
        delete (*it).second;
    }
    whiteboard_data.clear();
}

}


