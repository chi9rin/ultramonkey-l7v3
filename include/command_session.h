//
//!	@file	command_session.h
//!	@brief	l7vsadm message session class
//
//	copyright (c) sdy corporation. 2009
//	mail: a dot takamaru at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	COMMAND_SESSION_H
#define	COMMAND_SESSION_H

#ifndef	MAX_BUFFER_SIZE
	#define	MAX_BUFFER_SIZE (65535)
#endif

#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "l7vs_command.h"
#include "l7vsd.h"

namespace l7vs{

//!	@class	command_session
//!	@brief	l7vsadm message session class
class	command_session : public boost::enable_shared_from_this< command_session >{
protected:
	//l7vsd_main_thread								main_thread;
	//! unix domain socket
	boost::asio::local::stream_protocol::socket	unixsocket;
	//!	l7vsadm request data
	l7vsadm_request								request_data;
	//!	l7vsd response data
	l7vsd_response								response_data;
	//!	l7vsd refferecne
	l7vsd&										vsd;
	//!	command to handler map
	std::map<l7vsadm_request::COMMAND_CODE_TAG, boost::function<l7vsd::l7vsd_operation_result(void)> >
												command_handler_map;

	//! @brief default constructor
	command_session(){}

	//!	@brief		read handler
	//!	@param[in]	error code
	//!	@param[in]	read size
	void	handle_read( const boost::system::error_code&, size_t );

	//!	@brief		write handler
	//!	@param[in]	error code
	void	handle_write( const boost::system::error_code& );

public:
	//!	@brief		constructor
	//!	@param[in]	io_service
	//!	@param[in]	l7vsd refernce
	command_session( boost::asio::io_service&, l7vsd& );

	//!	@brief		return unixsocket
	boost::asio::local::stream_protocol::socket&	socket(){ return unixsocket; }

	//!	@brief		session start
	void	start();
};

};//namespace l7vsd
#endif		//L7VS_COMMAND_SESSION_H
