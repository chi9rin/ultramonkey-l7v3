#ifndef	L7VS_COMMAND_SESSION_H
#define	L7VS_COMMAND_SESSION_H

#ifndef	MAX_BUFFER_SIZE
	#define	MAX_BUFFER_SIZE (65535)
#endif

#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace l7vsd{

class	l7vs_command_session : public boost::enable_shared_from_this<session>{
protected:
	//l7vsd_main_thread								main_thread;
	boost::asio:local::stream_protocol::socket	unixsocket;
	boost::array<char, MAX_BUFFER_SIZE> 			command_buffer;
	boost::array<char, MAX_BUFFER_SIZE>			response_buffer;
	l7vs_command_session(){}
	void	handle_read( const boost::system::error_code&, size_t );
	void	handle_write( const boost::system::error_code& );
public:
	l7vs_command_session( boost::asio::io_service& io_service ) : unixsocket( io_service );
	boost::asio::local::stream_protocol::socket&	socket(){ return unixsocket; }
	void	start();
};

};//namespace l7vsd
#endif		//L7VS_COMMAND_SESSION_H
