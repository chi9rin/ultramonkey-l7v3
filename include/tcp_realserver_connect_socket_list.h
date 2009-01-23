#ifndef TCP_REALSERVER_CONNECT_SOCKET_LIST_H
#define TCP_REALSERVER_CONNECT_SOCKET_LIST_H

#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#include "tcp_socket.h"

namespace l7vs{

	class tcp_realserver_connect_socket_list : 
			private boost::noncopyable{
		typedef boost::shared_ptr<tcp_socket> tcp_socket_ptr;
		typedef std::pair<boost::asio::ip::tcp::endpoint,tcp_socket_ptr > list_element;
		protected:
			std::list< list_element > connect_list;
			boost::mutex list_mutex;
		public:
			tcp_realserver_connect_socket_list();
			~tcp_realserver_connect_socket_list();
			void push_back(list_element realserver_socket);
			list_element get_socket();
			bool empty();
	};// class tcp_realserver_connect_socket_list
}// namespace l7vs

#endif//TCP_REALSERVER_CONNECT_SOCKET_LIST_H
 

