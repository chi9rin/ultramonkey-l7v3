 
#ifndef TCP_THREAD_MESSAGE_QUE_H
#define TCP_THREAD_MESSAGE_QUE_H

#include <cstdlib>
#include <queue>
#include <boost/thread/mutex.hpp>


//#include "tcp_thread_message.h"

namespace l7vs{
	class tcp_thread_message;
	
	class tcp_thread_message_que : private boost::noncopyable{
		typedef boost::shared_ptr<tcp_thread_message> tcp_thread_message_ptr;
		public:
			std::queue< tcp_thread_message_ptr > message_que;
			boost::mutex que_mutex;
			
			tcp_thread_message_que();
			~tcp_thread_message_que();
			void push(tcp_thread_message_ptr message);
			tcp_thread_message_ptr front();
			bool empty();
			void clear();
	};// class tcp_thread_message_que
}// namespace l7vs

#endif//TCP_THREAD_MESSAGE_H
