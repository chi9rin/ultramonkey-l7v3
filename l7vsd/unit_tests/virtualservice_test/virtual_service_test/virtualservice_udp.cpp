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
#include "stub.h"

#define	BPS_DEFAULT_INTERVAL_USEC	500000ULL

// imprementation for virtualservice_udp
/*!
 * virtualservice_udp class constructor.
 */
l7vs::virtualservice_udp::virtualservice_udp(	const l7vs::l7vsd& invsd,
												const l7vs::replication& inrep,
												const l7vs::virtualservice_element& inelement) : 
													l7vs::virtualservice_base( invsd, inrep, inelement ){}
l7vs::virtualservice_udp::~virtualservice_udp(){}

void	l7vs::virtualservice_udp::handle_replication_interrupt( const boost::system::error_code& in ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
}
void	l7vs::virtualservice_udp::read_replicationdata(){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
}

void	l7vs::virtualservice_udp::initialize( l7vs::error_code& err ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
	err.setter( true, "" );

}
void	l7vs::virtualservice_udp::finalize( l7vs::error_code& err ){
	err.setter( true, "" );
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
}

bool	l7vs::virtualservice_udp::operator==( const l7vs::virtualservice_base& in ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
	return true;
}
bool	l7vs::virtualservice_udp::operator!=( const l7vs::virtualservice_base& in ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
	return true;
}

void	l7vs::virtualservice_udp::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
	err.setter( true, "" );
}
void	l7vs::virtualservice_udp::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
	err.setter( true, "" );
}

void	l7vs::virtualservice_udp::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
	err.setter( true, "" );
}
void	l7vs::virtualservice_udp::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
	err.setter( true, "" );
}
void	l7vs::virtualservice_udp::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
	err.setter( true, "" );
}

void	l7vs::virtualservice_udp::run(){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
}
void	l7vs::virtualservice_udp::stop(){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
}

void	l7vs::virtualservice_udp::release_session( const boost::thread::id thread_id ){
	virtual_service_test_struct::getInstance().udpfunctioncallcount++;
}
