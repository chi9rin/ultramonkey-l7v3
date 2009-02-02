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

#ifndef	COMMAND_BUFFER_SIZE
	#define	COMMAND_BUFFER_SIZE (65535)
#endif

#include <sstream>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "error_code.h"
#include "l7vs_command.h"
//#include "l7vsd.h"

namespace l7vs{

class	l7vsd;

//!	@class	command_session
//!	@brief	l7vsadm message session class
class	command_session : public boost::enable_shared_from_this< command_session >{
public:
	//! unix domain socket typedef
	typedef	boost::asio::local::stream_protocol::socket	unixsocket_type;
	//! command_handler typedef
	typedef std::map<l7vsadm_request::COMMAND_CODE_TAG, boost::function<void( l7vs::error_code )> >	command_handler_map_type;
	//! command code to status code map typedef
	typedef	std::map<l7vsadm_request::COMMAND_CODE_TAG, l7vsd_response::COMMAND_RESPONSE_CODE>	command_status_map_type;
	//! command_session shared_ptr typedef
	typedef	boost::shared_ptr<command_session>	command_session_ptr;

protected:
	
	//! unix domain socket
	unixsocket_type			unixsocket;
	//!	l7vsadm request data
	l7vsadm_request			request_data;
	//!	l7vsd response data
	l7vsd_response			response_data;
	//!	l7vsd refferecne
	l7vsd&					vsd;
	//!	command to handler map
	command_handler_map_type	command_handler_map;
	//! command code to status code map
	command_status_map_type		command_status_map;

	//! request recv buffer
	boost::array<char, COMMAND_BUFFER_SIZE>	request_buffer;
	//! response send stream
	std::stringstream						response_stream;

	//! @brief default constructor
	command_session();

	//!	@brief		read handler
	//!	@param[in]	error code
	//!	@param[in]	read size
	void	handle_read( const boost::system::error_code&, size_t );

	//!	@brief		write handler
	//!	@param[in]	error code
	void	handle_write( const boost::system::error_code& );

	//!	@brief		bind function command
	void	bind_function();

	//!	@brief		execute request command
	void	execute_command();

public:
	//!	@brief		constructor
	//!	@param[in]	io_service
	//!	@param[in]	l7vsd refernce
	command_session( boost::asio::io_service&, l7vsd& );

	//!	@brief		return unixsocket
	unixsocket_type&	socket(){ return unixsocket; }

	//!	@brief		session start
	void	start();
};

};//namespace l7vsd
#endif		//L7VS_COMMAND_SESSION_H
