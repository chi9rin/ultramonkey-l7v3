#include "tcp_socket.h"

namespace l7vs{
	
	tcp_socket::tcp_socket(boost::asio::io_service& io):
		my_socket(io){
		
	}
	
	tcp_socket::~tcp_socket(){
	}

	boost::asio::ip::tcp::socket& tcp_socket::get_socket(){
		return my_socket;
	}

	bool tcp_socket::connect(boost::asio::ip::tcp::endpoint connect_endpoint,
		boost::system::error_code& ec){
		return true;
	}

	bool tcp_socket::close(boost::system::error_code& ec){
		return true;
	}
	bool tcp_socket::set_non_blocking_mode(boost::system::error_code& ec){
		return true;
	}
	
	std::size_t tcp_socket::write_some(boost::asio::mutable_buffers_1 buffers,
	boost::system::error_code& ec){
		/*				
		const void* pdata = boost::asio::detail::buffer_cast_helper(*buffers.begin());
		char* psz = (char*)pdata;
		std::cout << "<";
		std::cout << psz;
		std::cout << ">";
		std::cout << "write_some OK!\n";
		*/
		std::size_t size = 0;
		return size;
	}
		
	std::size_t tcp_socket::read_some(boost::asio::mutable_buffers_1 buffers,
		boost::system::error_code& ec){
			/*	
		const void* pdata = boost::asio::detail::buffer_cast_helper(*buffers.begin());
		char* psz = (char*)pdata;
		std::cout << "<";
		std::cout << psz;
		std::cout << ">";
		std::cout << "read_some OK!\n";
			*/
		std::size_t size = 0;
		return size;
	}
	
}// namespace l7vs
 
