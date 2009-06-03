//
//!	@file	virtualservice.cpp
//!	@brief	VirtualService class implementations
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#include <new>
#include <vector>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "virtualservice.h"
#include "logger_enum.h"
#include "logger.h"
#include "parameter.h"

#define	BPS_DEFAULT_INTERVAL_USEC	500000ULL

// imprementation for virtualservice_tcp
l7vs::virtualservice_tcp::virtualservice_tcp(	const l7vsd& invsd,
												const replication& inrep,
												const virtualservice_element& inelement )
												 :	virtualservice_base( invsd, inrep, inelement ),
													acceptor_( dispatcher ) {}

l7vs::virtualservice_tcp::~virtualservice_tcp(){
}

void	l7vs::virtualservice_tcp::handle_replication_interrupt( const boost::system::error_code& in ){
}

void	l7vs::virtualservice_tcp::read_replicationdata(){
}

void	l7vs::virtualservice_tcp::handle_accept( const l7vs::session_thread_control* stc_ptr, const boost::system::error_code& err ){
}

void	l7vs::virtualservice_tcp::initialize( l7vs::error_code& err ){
	err.setter( false, "" );
}

void		l7vs::virtualservice_tcp::finalize( l7vs::error_code& err ){
	err.setter( false, "" );
}

bool	l7vs::virtualservice_tcp::operator==( const l7vs::virtualservice_base& in ){
	return true;
}

bool	l7vs::virtualservice_tcp::operator!=( const l7vs::virtualservice_base& in ){
	return true;
}

void	l7vs::virtualservice_tcp::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( false, "" );
}

void	l7vs::virtualservice_tcp::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( false, "" );
}

void	l7vs::virtualservice_tcp::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( false, "" );
}

void	l7vs::virtualservice_tcp::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( false, "" );
}

void	l7vs::virtualservice_tcp::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	err.setter( false, "" );
}

void	l7vs::virtualservice_tcp::run(){
}

void	l7vs::virtualservice_tcp::stop(){
}

void	l7vs::virtualservice_tcp::connection_active( const boost::asio::ip::tcp::endpoint& in ){
}

void	l7vs::virtualservice_tcp::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){
}

void	l7vs::virtualservice_tcp::release_session( const boost::thread::id thread_id ){
}

