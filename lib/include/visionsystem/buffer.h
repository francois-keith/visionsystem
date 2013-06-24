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

            // Access front of frame buffer ( Producer side with frameskipping enabled )
            // Will keep the buffer lock closed until update_front is called
            Data* lock_front();

            // Unlock the buffer once front has been updated
            void update_front(Data * in);

			// Misc.

			void clear() ;
			int size() ;
			bool is_full() ;

	} ;

#include <visionsystem/buffer.hpp>

}

#endif

