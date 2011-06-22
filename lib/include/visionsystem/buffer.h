#ifndef VS_BUFFER_H
#define VS_BUFFER_H

#include <boost/thread/mutex.hpp>
#include <vector>

namespace visionsystem {

	template < typename Data >
	class Buffer {

		private:

			std::vector< Data* > _frames ;
			std::vector< Data* > _trash  ;
			boost::mutex	    _mutex ; 

		public:

			Buffer()  ;
			~Buffer() ;

			// Primitives to use data inside the buffer ( Consummer side )

			Data* bl_dequeue() ;	
			Data* nbl_dequeue() ;
			void enqueue ( Data* ) ;				    

			// Primitives to feed data to the buffer ( Producer side )

			Data* pull() ;
			void push( Data* ) ;

			// Empty the buffer :
			
			void clear() ;

	} ;

#include <visionsystem/buffer.hpp>

}

#endif

