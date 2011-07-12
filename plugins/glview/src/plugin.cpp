#include "plugin.h"


GLView::GLView( VisionSystem* core, string sandbox ) 
:Viewer( core, "glview", sandbox ) 
{
	active_cam = 0 ;
	next_cam = 0 ;
	callback_active = true ;
}

GLView::~GLView() {

}


bool  GLView::pre_fct() {

	string filename = get_sandbox() + string("/glview.conf") ;

	try { 
		read_config_file ( filename.c_str() ) ;  
	
	} catch ( string msg ) {
		
		cout << "[glview] Could not find config file. Using default settings" << endl ;

	}
		

	cameras = get_all_cameras() ;

	active_cam = -1 ;
	for ( int i=0; i<cameras.size(); i++ )
		if ( cameras[i]->is_active() )
			active_cam = i ;

	next_cam = active_cam ;
	
	if ( active_cam == -1 )	{
		cerr << "[glview] ERROR : No cam is active. " << endl ;
		return false ;
	}

	register_to_cam< Image<uint32_t, RGB> >( cameras[active_cam], 10 ) ;

	return true ;
}


void  GLView::preloop_fct() {
	win = new GLWindow( "glview", cameras[active_cam]->get_size().x,cameras[active_cam]->get_size().y, false ) ; 
}

void  GLView::loop_fct() {

	Image<uint32_t, RGB>	*img ;
	img = dequeue_image< Image<uint32_t, RGB> >( cameras[active_cam] ) ;

	// Draw video 
	
	win->draw(img);
	
	// Call all registered glfuncs

	glfuncs_mutex.lock() ;	
	for ( int i=0; i<glfuncs.size(); i++ )
		glfuncs[i]->glfunc( cameras[active_cam] ) ;
	glfuncs_mutex.unlock() ;
	
	// Swap buffers

	win->swap_buffers();
	
	// Process events

	XEvent event = win->processEvents();
	
	// Call all registered callbacks

	if ( callback_active )
		callback( event ) ;

	callbacks_mutex.lock() ;	
	for ( int i=0; i<callbacks.size(); i++ )
		callbacks[i]->callback( cameras[active_cam], event ) ;

	callbacks_mutex.unlock() ;	

	enqueue_image< Image<uint32_t, RGB> >( cameras[active_cam], img ) ;

	if ( active_cam != next_cam ) {
		unregister_to_cam< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
		register_to_cam< Image<uint32_t, RGB> >( cameras[next_cam], 10 ) ;
		active_cam = next_cam ;
	}

}

void GLView::callback( XEvent event ) {
	
	if (event.type == KeyPress ) {

		switch ( XLookupKeysym(&event.xkey, 0) ) {

		case XK_F1:
			if ( cameras[0]->is_active() )
				next_cam = 0 ;
			break;
	
		case XK_F2:
			if ( cameras.size() > 1  ) 
				if ( cameras[1]->is_active() )
					next_cam = 1 ;
			break;
								
		case XK_F3:
			if ( cameras.size() > 2  ) 
				if ( cameras[2]->is_active() )
					next_cam = 2 ;
			break;

		case XK_F4:
			if ( cameras.size() > 3  ) 
				if ( cameras[3]->is_active() )
					next_cam = 3 ;
			break;

		case XK_F5:
			if ( cameras.size() > 4  ) 
				if( cameras[4]->is_active() )
					next_cam = 4 ;
			break;

		case XK_F6:
			if ( cameras.size() > 5  ) 
				if ( cameras[5]->is_active() )
					next_cam = 5 ;
			break;

		case XK_F7:
			if ( cameras.size() > 6  ) 
				if ( cameras[6]->is_active() )
					next_cam = 6 ;
			break;

		case XK_F8:
			if ( cameras.size() > 7  ) 
				if ( cameras[7]->is_active() )
					next_cam = 7 ;
			break;

		case XK_F9:
			if ( cameras.size() > 8  ) 
				if ( cameras[8]->is_active() )
					next_cam = 8 ;
			break;

		case XK_F10:
			if ( cameras.size() > 9  ) 
				if ( cameras[9]->is_active() )
					next_cam = 9 ;
			break;

		case XK_F11:
			if ( cameras.size() > 10  ) 
				if ( cameras[10]->is_active() )
					next_cam = 10 ;
			break;

		case XK_F12:
			if ( cameras.size() > 11  ) 
				if ( cameras[11]->is_active() )
					next_cam = 11 ;
			break;

        case XK_Page_Up:
            for(int i = active_cam + 1; i < cameras.size(); ++i)
            {
                if ( cameras[i]->is_active() )
                {
                    next_cam = i;
                    i = cameras.size();
                }
            }
            break;

        case XK_Page_Down:
            for(int i = active_cam - 1; i >= 0; --i)
            {
                if ( cameras[i]->is_active() )
                {
                    next_cam = i;
                    i = -1;
                }
            }
            break;

		default:
			break;
		}
	}

}

bool  GLView::post_fct() {
	
	unregister_to_cam< Image<uint32_t, RGB> >( cameras[active_cam] ) ;
	return true ;
}


void GLView::parse_config_line( vector<string> &line ) {
	fill_member<bool>( line, "Function_Keys", callback_active ) ;
}


void GLView::gl_print ( ImageRef position, string text ) {

	// FIXME 

}
