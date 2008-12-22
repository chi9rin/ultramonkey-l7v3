#ifndef	L7VS_COMMAND_RECIVER_H
#define	L7VS_COMMAND_RECIVER_H

#include "l7vs_command_session.h"

namespace l7vsd{

class	command_receiver{
protected:
	command_receiver(){}
	// l7vsd_mainthread								main_thread;
	boost::asio::io_service&						dispatcher;
	boost::asio::local::stream_protocol::acceptor	acceptor_;
	l7vsd											parent;

	void	handle_accept( boost::shared_ptr<command_session>, const boost::system::error_code& );

public:
	command_receiver(	boost::asio::io_service& io_service, const std::string& file ) 
		:	dispatcher( io_service ),
			acceptor_( io_service, boost::asio::local::stream_protocol::endpoint( file ) );
	~command_receiver();
};

};	//namespace l7vsd
#endif	//L7VS_COMMAND_RECIVER_H
