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
			acceptor_( io_service, boost::asio::local::stream_protocol::endpoint( file ) ),
			vsd( parent ){

	// create command_session for first acception.
	command_session::command_session_ptr session( new command_session( dispatcher, vsd ) );
	// start an asynchronous accept for first.
	acceptor_.async_accept(	session->socket(),
							boost::bind(&command_receiver::handle_accept,
								this,
								session,
								boost::asio::placeholders::error));
}

//!	@brief		destructor
command_receiver::~command_receiver(){
}

//!	@brief		accept handler
//!	@param[in]	command session
//!	@param[in]	error code
void	command_receiver::handle_accept( command_session::command_session_ptr session, const boost::system::error_code& err ){
	// check async_accept() result.
	if ( !err ) {
		// command_session start.
		session->start();
		// create command_session for next acception.
		session.reset( new command_session( dispatcher, vsd ) );
		// start an asynchronous accept for next.
		acceptor_.async_accept(	session->socket(),
								boost::bind(&command_receiver::handle_accept,
									this,
									session,
									boost::asio::placeholders::error) );

	} else {
		// start an asynchronous accept for retry.
		acceptor_.async_accept(	session->socket(),
								boost::bind(&command_receiver::handle_accept,
									this,
									session,
									boost::asio::placeholders::error) );
	}
}

}	//namespace	l7vs
