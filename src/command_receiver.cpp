//
//!	@file	command_receiver.cpp
//!	@brief	l7vsadm message receiver class
//
//	copyright (c) sdy corporation. 2008
//	mail: a dot takamaru at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include	"command_receiver.h"

namespace	l7vs{

//!	@brief		constructor
//!	@param[in]	io_service
//!	@param[in]	socket file
//!	@param[in]	l7vsd refernce
command_receiver::command_receiver( boost::asio::io_service& io_service, const std::string& file, l7vsd& parent )
		:	dispatcher( io_service ),
			acceptor( io_service, boost::asio::local::stream_protocol::endpoint( file ) ),
			vsd( parent ){

}

//!	@brief		destructor
~command_receiver(){
}

//!	@brief		accept handler
//!	@param[in]	command session
//!	@param[in]	error code
void	command_receiver::handle_accept( boost::shared_ptr< command_session > session, const boost::system::error_code& ec ){

}

}	//namespace	l7vs