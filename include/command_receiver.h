//
//!	@file	command_receiver.h
//!	@brief	l7vsadm message receiver class
//
//	copyright (c) sdy corporation. 2008
//	mail: a dot takamaru at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	COMMAND_RECEIVER_H
#define	COMMAND_RECEIVER_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include "command_session.h"
#include "l7vsd.h"

namespace l7vs{
class	l7vsd;

//!	@class	command_receiver
//!	@brief	l7vsadm message receiver class
class	command_receiver : private boost::noncopyable{
protected:
	//! io dispatcher
	boost::asio::io_service&						dispatcher;
	//! acceptor
	boost::asio::local::stream_protocol::acceptor	acceptor_;
	//! l7vsd refference
	l7vsd&											vsd;

	//!	@brief		accept handler
	//!	@param[in]	command session
	//!	@param[in]	error code
	void	handle_accept( boost::shared_ptr<command_session>, const boost::system::error_code& );

public:
	//!	@brief		constructor
	//!	@param[in]	io_service
	//!	@param[in]	socket file
	//!	@param[in]	l7vsd refernce
	command_receiver(	boost::asio::io_service&, const std::string&, l7vsd& );

	//!	@brief		destructor
	~command_receiver();
};

};	//namespace l7vsd
#endif	//COMMAND_RECEIVER_H
