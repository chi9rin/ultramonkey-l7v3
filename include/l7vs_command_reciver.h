#ifndef	L7VS_COMMAND_RECIVER_H
#define	L7VS_COMMAND_RECIVER_H

#include "l7vs_command_session.h"

namespace l7vsd{

class	command_reciver{
protected:
	command_reciver(){}
	// l7vsd_mainthread										main_thread;
	boost::asio::io_service&								dispatcher;
	boost::asio::local::stream_protocol::acceptor		acceptor_;
	void	handle_accept( boost::shared_ptr<l7vs_command_session>, const boost::system::error_code& );
public:
	command_reciver(	boost::asio::io_service& io_service, const std::string& file ) 
		:	dispatcher( io_service ),
			acceprot_( io_service, boost::asio::local::stream_protocol::endpoint( file ) );
	~command_reciver();
};

};	//namespace l7vsd
#endif	//L7VS_COMMAND_RECIVER_H
