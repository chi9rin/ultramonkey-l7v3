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

l7vs::virtual_service::virtual_service(	const l7vs::l7vsd& invsd,
										const l7vs::replication& inrep,
										const l7vs::virtualservice_element& inelement ){
	try{
		if( inelement.udpmode )
			vs = boost::shared_ptr<l7vs::virtualservice_base>(
					dynamic_cast<l7vs::virtualservice_base*>( new l7vs::virtualservice_udp( invsd, inrep, inelement ) ) );
		else
			vs = boost::shared_ptr<l7vs::virtualservice_base>(
					dynamic_cast<l7vs::virtualservice_base*>( new l7vs::virtualservice_tcp( invsd, inrep, inelement ) ) );
	}
	catch(const std::bad_alloc&){
		throw std::bad_alloc();
	}
}

l7vs::virtual_service::~virtual_service(){
}

void	l7vs::virtual_service::initialize( l7vs::error_code& err ){
	if( NULL != vs )
		vs->initialize( err );
	else{
		err.setter( false, "Fail, create VirtualService" );
	}
}
void	l7vs::virtual_service::finalize( l7vs::error_code& err ){
	vs->finalize( err );
}

bool	l7vs::virtual_service::operator==( const l7vs::virtualservice_base& in ){
	return vs->operator==( in );
}
bool	l7vs::virtual_service::operator!=( const l7vs::virtualservice_base& in ){
	return vs->operator!=( in );
}

void	l7vs::virtual_service::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->set_virtualservice( in, err );
}
void	l7vs::virtual_service::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->edit_virtualservice( in, err );
}

void	l7vs::virtual_service::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->add_realserver( in, err );
}
void	l7vs::virtual_service::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->edit_realserver( in, err );
}
void	l7vs::virtual_service::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->del_realserver( in, err );
}

l7vs::virtualservice_element&	l7vs::virtual_service::get_element(){
	return vs->get_element();
}

void		l7vs::virtual_service::run(){
	vs->run();
}
void		l7vs::virtual_service::stop(){
	vs->stop();
}

void		l7vs::virtual_service::connection_active( const boost::asio::ip::tcp::endpoint& in ){
	vs->connection_active( in );
}
void		l7vs::virtual_service::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){
	vs->connection_inactive( in );
}
void		l7vs::virtual_service::release_session( const boost::thread::id thread_id ){
	vs->release_session( thread_id );
}

unsigned long long		l7vs::virtual_service::get_qos_upstream(){
	return vs->get_qos_upstream();
}
unsigned long long		l7vs::virtual_service::get_qos_downstream(){
	return vs->get_qos_downstream();
}
unsigned long long		l7vs::virtual_service::get_throughput_upstream(){
	return vs->get_throughput_upstream();
}
unsigned long long		l7vs::virtual_service::get_throughput_downstream(){
	return vs->get_throughput_downstream();
}

unsigned long long		l7vs::virtual_service::get_up_recv_size(){
	return vs->get_up_recv_size();
}

unsigned long long		l7vs::virtual_service::get_up_send_size(){
	return vs->get_up_send_size();
}

unsigned long long		l7vs::virtual_service::get_down_recv_size(){
	return vs->get_down_recv_size();
}

unsigned long long		l7vs::virtual_service::get_down_send_size(){
	return vs->get_down_send_size();
}

void		l7vs::virtual_service::update_up_recv_size( unsigned long long	datasize ){
	vs->update_up_recv_size( datasize );
}
void		l7vs::virtual_service::update_up_send_size( unsigned long long	datasize ){
	vs->update_up_send_size( datasize );
}
void		l7vs::virtual_service::update_down_recv_size( unsigned long long	datasize ){
	vs->update_down_recv_size( datasize );
}
void		l7vs::virtual_service::update_down_send_size( unsigned long long	datasize ){
	vs->update_down_send_size( datasize );
}
	
l7vs::protocol_module_base*
			l7vs::virtual_service::get_protocol_module(){
	return vs->get_protocol_module();
}
l7vs::schedule_module_base*
			l7vs::virtual_service::get_schedule_module(){
	return vs->get_schedule_module();
}
