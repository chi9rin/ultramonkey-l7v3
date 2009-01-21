//
//!	@file	command_session.cpp
//!	@brief	l7vsadm message session class
//
//	copyright (c) sdy corporation. 2009
//	mail: a dot takamaru at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include	"command_session.h"

namespace	l7vs{

//!	@brief		constructor
//!	@param[in]	io_service
//!	@param[in]	l7vsd refernce
command_session::command_session( boost::asio::io_service& io_service, l7vsd& parent )
			:	unixsocket( io_service ),
				vsd( parent ){
}

//!	@brief		read handler
//!	@param[in]	error code
//!	@param[in]	read size
void	command_session::handle_read( const boost::system::error_code& ec, size_t size){

}

//!	@brief		write handler
//!	@param[in]	error code
void	command_session::handle_write( const boost::system::error_code& ec ){

}

//!	@brief		session start
void	command_session::start(){
}

}	//namespace	l7vs
