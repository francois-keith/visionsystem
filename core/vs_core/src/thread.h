#ifndef VS_THREAD_H
#define VS_THREAD_H

#include <string>

#include <boost/thread.hpp>

#include <visionsystem/visionsystem.h>

#ifndef WIN32
	#include <dlfcn.h>
#endif



template< typename data >
class Thread {

	public:
		Thread( visionsystem::VisionSystem* core, std::string plugin, std::string sandbox ) ;
		~Thread() ;


		void start_thread() ;
		void request_stop() ;
		void join() ;

		data*  pointer  ;

	private:

		boost::thread	_thread ;
	
		#ifndef WIN32
			void*	handle	;
		#else
			HINSTANCE handle;
		#endif

		typedef data* create_t( visionsystem::VisionSystem*, std::string sandbox );
		typedef void destroy_t( data* );

		create_t*	_create_fct ;
		destroy_t*	_destroy_fct ;


	private:

		void main() ;
		bool _done  ;

} ;	



template<typename data>
Thread<data>::Thread( visionsystem::VisionSystem* core, std::string plugin, std::string sandbox ) {

    std::string filename ;

    #ifndef WIN32
        filename = std::string("lib") + plugin + std::string(".so" ) ;
    	handle = dlopen( filename.c_str() , RTLD_NOW | RTLD_GLOBAL );
    #else
	filename = plugin + std::string(".dll") ;
    	handle = LoadLibrary(filename.c_str());
    #endif

    if (!handle) {
    		#ifndef WIN32
  	    		std::cerr << "[vs_core] ERROR: " << dlerror() << '\n';
    		#else
    			LPVOID lpMsgBuf;
                        LPVOID lpDisplayBuf;
    			GetLastError() returns the error code
                        DWORD dw = GetLastError();
    
    			// We get the error message associated with the error code
     		        						         			    	    						                                                            		    FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw,
                                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                        (LPTSTR) &lpMsgBuf, 0, NULL );
 			                                                                                                                                                                                                                    // Display buffer for the error message

			lpDisplayBuf = (LPVOID)LocalAlloc( LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + 40) * sizeof(TCHAR) );
     		        
			StringCchPrintf( (LPTSTR)lpDisplayBuf,
					  LocalSize(lpDisplayBuf) / sizeof(TCHAR),
					  TEXT("Failed with error %d: %s"),
				          dw, lpMsgBuf );
			
			// We print the error

			std::cerr << "[vs_core] ERROR: " << (LPCTSTR)lpDisplayBuf << std::endl;

			LocalFree(lpMsgBuf);
			LocalFree(lpDisplayBuf);
		#endif
		
		throw ( std::string("[vs_core] ERROR: Could not load dll: ") + filename ) ;
     }

	// load the symbols

	#ifndef WIN32
		_create_fct  = (create_t*)  dlsym( handle, "create" );
		_destroy_fct = (destroy_t*) dlsym( handle, "destroy");
	#else
		_create_fct  = (create_t*)  GetProcAddress(handle, "create");
		_destroy_fct = (destroy_t*) GetProcAddress(handle, "destroy");
	#endif

	if ( !_create_fct || !_destroy_fct ) {
	
		#ifndef WIN32
			std::cerr << "[vs_core] ERROR: " << dlerror() << '\n';
		#else
			std::cerr << "[vs_core] ERROR: " << filename << std::endl;
		#endif
	
		throw( std::string("[vs_core] ERROR: Could not import symbols") );
	}

	// create an instance of the class

	pointer = _create_fct( core, sandbox );

	// Get ready to run th thread
	
	_done = false ;

}


template<typename Data>
Thread<Data>::~Thread() {
	_destroy_fct( pointer ) ;
    #ifndef WIN32
    dlclose(handle);
    #else
    #endif
}


template<typename Data>
void Thread<Data>::start_thread() {
	_thread = boost::thread( &Thread<Data>::main, this );  

}


template<typename Data>
void Thread<Data>::request_stop() {
	_done = true ;
	_thread.interrupt() ;	
}

template<typename Data>
void Thread<Data>::join() {
	_thread.join() ;   
}

template< typename Data >
void Thread<Data>::main() {
	
	try {
		pointer->preloop_fct() ;

		while ( !_done ) 
			pointer->loop_fct() ;
	
	} catch ( std::string msg ) {
		
		if ( !_done )
			std::cerr << msg << std::endl ;
		return ;
	
	}

}

#endif

