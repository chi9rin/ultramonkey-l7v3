#ifndef	L7VS_COMMAND_SESSION_H
#define	L7VS_COMMAND_SESSION_H

#ifndef	MAX_BUFFER_SIZE
	#define	MAX_BUFFER_SIZE (65535)
#endif

#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "l7command.h"

namespace l7vsd{

class	command_session : public boost::enable_shared_from_this<session>{
protected:
	//l7vsd_main_thread								main_thread;
	boost::asio:local::stream_protocol::socket	unixsocket;
	boost::array<char, MAX_BUFFER_SIZE> 		command_buffer;
	boost::array<char, MAX_BUFFER_SIZE>			response_buffer;
	l7vsadm_request								request_;
	l7vsd&										vsd;

	command_session(){}
	void	handle_read( const boost::system::error_code&, size_t );
	void	handle_write( const boost::system::error_code& );
public:
	command_session( boost::asio::io_service& io_service, l7vsd& parent ) : unixsocket( io_service ), vsd( parent );
	boost::asio::local::stream_protocol::socket&	socket(){ return unixsocket; }
	void	start();
};

};//namespace l7vsd
#endif		//L7VS_COMMAND_SESSION_H
