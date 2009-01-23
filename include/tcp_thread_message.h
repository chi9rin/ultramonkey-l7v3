 

#ifndef TCP_THREAD_MESSAGE_H
#define TCP_THREAD_MESSAGE_H

#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "tcp_session.h"

namespace l7vs{

	class tcp_thread_message : private boost::noncopyable{
		public:
			boost::function<void(tcp_session::TCP_PROCESS_TYPE_TAG)> message;
			boost::asio::ip::tcp::endpoint endpoint_info;
			
			tcp_thread_message();
			~tcp_thread_message();
	};// class tcp_thread_message
}// namespace l7vs

#endif//TCP_THREAD_MESSAGE_H
