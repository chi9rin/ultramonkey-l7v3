#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

namespace l7vs{

	class tcp_socket : private boost::noncopyable{
		protected:
			boost::asio::ip::tcp::socket my_socket;
			boost::mutex socket_mutex;
			bool open_flag;
		public:
			tcp_socket(boost::asio::io_service& io);
			~tcp_socket();
			boost::asio::ip::tcp::socket& get_socket();
			bool connect(const boost::asio::ip::tcp::endpoint connect_endpoint,boost::system::error_code& ec);
			bool close(boost::system::error_code& ec);
			bool set_non_blocking_mode(boost::system::error_code& ec);
			std::size_t write_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec);
			std::size_t read_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec);

	};// class tcp_socket
}// namespace l7vs

#endif//TCP_SOCKET_H
 
