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

#include "../include/virtualservice.h"

namespace l7vs{

// imprementation for virtualservice_base
virtualservice_base::virtualservice_base(	const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const virtualservice_element& inelement)
												 :	vsd( invsd ),
													rep( inrep ),
													element( inelement ),
													vs_timer( dispatcher ) {};

void	virtualservice_base::handle_protomod_replication( const boost::system::error_code& ){}
void	virtualservice_base::handle_schedmod_replication( const boost::system::error_code& ){}
void	virtualservice_base::handle_throughput_update( const boost::system::error_code& ){}

void	virtualservice_base::rs_list_lock(){}
void	virtualservice_base::rs_list_unlock(){}

unsigned long long	virtualservice_base::get_qos_upstream(){ return element.qos_upstream; }
unsigned long long	virtualservice_base::get_qos_downstream(){ return element.qos_downstream; }
unsigned long long	virtualservice_base::get_throughput_upstream(){ return throughput_up; }
unsigned long long	virtualservice_base::get_throughput_downstream(){ return throughput_down; }

void	virtualservice_base::update_up_recv_size( unsigned long long	datasize ){}
void	virtualservice_base::update_up_send_size( unsigned long long	datasize ){}
void	virtualservice_base::update_down_recv_size( unsigned long long	datasize ){}
void	virtualservice_base::update_down_send_size( unsigned long long	datasize ){}
	
boost::shared_ptr<protocol_module_base>
		virtualservice_base::get_protocol_module(){ return protomod; }
boost::shared_ptr<schedule_module_base>
		virtualservice_base::get_schedule_module(){ return schedmod; }



virtualservice_tcp::virtualservice_tcp(	const l7vs::l7vsd& invsd,
										const l7vs::replication& inrep,
										const virtualservice_element& inelement )
												 :	virtualservice_base( invsd, inrep, inelement ),
													acceptor_( dispatcher ) {}
virtualservice_tcp::~virtualservice_tcp(){}

void	virtualservice_tcp::handle_replication_interrupt( const boost::system::error_code& in ){}
bool	virtualservice_tcp::read_replicationdata( vs_replication_data& out ){ return true; }

void	virtualservice_tcp::handle_accept(	const session_thread_control_ptr in_session,
											const boost::system::error_code& in_error ){}

void	virtualservice_tcp::initialize( error_code& err ){
	err.setter( true, "" );
}
void		virtualservice_tcp::finalize( error_code& err ){
	err.setter( true, "" );
}

bool	virtualservice_tcp::operator==( const virtualservice_base& in ){ return true; }
bool	virtualservice_tcp::operator!=( const virtualservice_base& in ){ return true; }

void	virtualservice_tcp::set_virtualservice( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}
void	virtualservice_tcp::edit_virtualservice( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}

void	virtualservice_tcp::add_realserver( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}
void	virtualservice_tcp::edit_realserver( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}
void	virtualservice_tcp::del_realserver( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}

void	virtualservice_tcp::run(){}
void	virtualservice_tcp::stop(){}

void	virtualservice_tcp::connection_active( const boost::asio::ip::tcp::endpoint& in ){}
void	virtualservice_tcp::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){}
void	virtualservice_tcp::release_session( const boost::thread::id thread_id ){}



virtualservice_udp::virtualservice_udp(		const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const virtualservice_element& inelement) : 
													virtualservice_base( invsd, inrep, inelement ){}
virtualservice_udp::~virtualservice_udp(){}

void	virtualservice_udp::handle_replication_interrupt( const boost::system::error_code& in ){}
bool	virtualservice_udp::read_replicationdata( vs_replication_data& out ){ return true; }

void	virtualservice_udp::initialize( error_code& err ){
	err.setter( true, "" );
}
void		virtualservice_udp::finalize( error_code& err ){
	err.setter( true, "" );
}

bool	virtualservice_udp::operator==( const virtualservice_base& in ){ return true; }
bool	virtualservice_udp::operator!=( const virtualservice_base& in ){ return true; }

void	virtualservice_udp::set_virtualservice( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}
void	virtualservice_udp::edit_virtualservice( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}

void	virtualservice_udp::add_realserver( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}
void	virtualservice_udp::edit_realserver( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}
void	virtualservice_udp::del_realserver( const virtualservice_element& in, error_code& err ){
	err.setter( true, "" );
}

void	virtualservice_udp::run(){}
void	virtualservice_udp::stop(){}

void	virtualservice_udp::connection_active( const boost::asio::ip::tcp::endpoint& in ){}
void	virtualservice_udp::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){}
void	virtualservice_udp::release_session( const boost::thread::id thread_id ){}


virtual_service::virtual_service(	const l7vs::l7vsd& invsd,
									const l7vs::replication& inrep,
									const virtualservice_element& inelement ){
	if( inelement.udpmode )
		vs = boost::shared_ptr<virtualservice_base>(
				dynamic_cast<virtualservice_base*>( new virtualservice_udp( invsd, inrep, inelement ) ) );
	else
		vs = boost::shared_ptr<virtualservice_base>(
				dynamic_cast<virtualservice_base*>( new virtualservice_tcp( invsd, inrep, inelement ) ) );
}
virtual_service::~virtual_service(){
}
	
void	virtual_service::initialize( error_code& err ){ vs->initialize( err ); }
void	virtual_service::finalize( error_code& err ){ vs->finalize( err ); }

bool	virtual_service::operator==( const virtualservice_base& in ){ return vs->operator==( in ); }
bool	virtual_service::operator!=( const virtualservice_base& in ){ return vs->operator!=( in ); }

void	virtual_service::set_virtualservice( const virtualservice_element& in, error_code& err ){
	vs->set_virtualservice( in, err );
}
void	virtual_service::edit_virtualservice( const virtualservice_element& in, error_code& err ){
	vs->edit_virtualservice( in, err );
}

void	virtual_service::add_realserver( const virtualservice_element& in, error_code& err ){
	vs->add_realserver( in, err );
}
void	virtual_service::edit_realserver( const virtualservice_element& in, error_code& err ){
	vs->edit_realserver( in, err );
}
void	virtual_service::del_realserver( const virtualservice_element& in, error_code& err ){
	vs->del_realserver( in, err );
}

virtualservice_element&	virtual_service::get_element(){ return vs->get_element(); }

void		virtual_service::run(){ vs->run(); }
void		virtual_service::stop(){ vs->stop(); }

void		virtual_service::connection_active( const boost::asio::ip::tcp::endpoint& in ){ vs->connection_active( in ); }
void		virtual_service::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){ vs->connection_inactive( in ); }
void		virtual_service::release_session( const boost::thread::id thread_id ){ vs->release_session( thread_id ); }

unsigned long long		virtual_service::get_qos_upstream(){ return vs->get_qos_upstream(); }
unsigned long long		virtual_service::get_qos_downstream(){ return vs->get_qos_downstream(); }
unsigned long long		virtual_service::get_throughput_upstream(){ return vs->get_throughput_upstream(); }
unsigned long long		virtual_service::get_throughput_downstream(){ return vs->get_throughput_downstream(); }

void		virtual_service::update_up_recv_size( unsigned long long	datasize ){
	vs->update_up_recv_size( datasize );
}
void		virtual_service::update_up_send_size( unsigned long long	datasize ){
	vs->update_up_send_size( datasize );
}
void		virtual_service::update_down_recv_size( unsigned long long	datasize ){
	vs->update_down_recv_size( datasize );
}
void		virtual_service::update_down_send_size( unsigned long long	datasize ){
	vs->update_down_send_size( datasize );
}
	
boost::shared_ptr<protocol_module_base>
			virtual_service::get_protocol_module(){ return vs->get_protocol_module(); }
boost::shared_ptr<schedule_module_base>
			virtual_service::get_schedule_module(){ return vs->get_schedule_module(); }

}
