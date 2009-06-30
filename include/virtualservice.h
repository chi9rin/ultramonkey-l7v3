/*!
 *	@file	virtualservice.h
 *	@brief	VirtualService classes
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#ifndef	VIRTUALSERVICE_H
#define	VIRTUALSERVICE_H

#include <sched.h>
#include <net/if.h>
#include <unistd.h>
#include <sched.h>
#include <error.h>
#include <string>
#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "error_code.h"
#include "l7vsd.h"
#include "realserver.h"
#include "virtualservice_element.h"
#include "tcp_session.h"
#include "udp_session.h"
#include "session_thread_control.h"
#include "replication.h"

#include "protocol_module_base.h"
#include "schedule_module_base.h"

#include "atomic.h"
#include "lockfree_queue.h"
#include "lockfree_hashmap.h"

#define	PARAM_RS_SIDE_NIC_NAME	"nic_realserver_side"
#define	PARAM_POOLSIZE_KEY_NAME	"session_thread_pool_size"
#define	PARAM_BPS_CALC_INTERVAL	"throughput_calc_interval"
#define	PARAM_REP_INTERVAL		"interval"

#define	PROTOMOD_NOTLOAD_ERROR_MSG	"Protocol Module not loaded"
#define	SCHEDMOD_NOTLOAD_ERROR_MSG	"Schedule Module not loaded"
#define	PROTOMOD_LOAD_ERROR_MSG		"Protocol Module load error"
#define	SCHEDMOD_LOAD_ERROR_MSG		"Schedule Module load error"

#define	REP_BLOCK_SIZE_ERR_MSG		"Replication area block size error"
#define	REP_AREA_SIZE_ERR_MSG		"Replication area size error"

#define	REP_AREA_NAME				"virtualservice"

namespace l7vs{

class	l7vsd;
class	replication;
class	session_thread_control;

//!	@class	virtualservice_base
//!	@brief	virtualservice base class is base class of tcp virtual service and udp virtual service.
class	virtualservice_base : boost::noncopyable{
public:
	//! shared_ptr session_thread_control typedef
	typedef	boost::shared_ptr<boost::asio::deadline_timer>	deadline_timer_ptr_type;
	//! tcp endpoint typedef
	typedef	boost::asio::ip::tcp::endpoint					tcp_endpoint_type;
	//! udp endpoint typedef
	typedef	boost::asio::ip::udp::endpoint					udp_endpoint_type;

	typedef	std::list<l7vs::realserver>
									rslist_type;
	typedef	boost::function< rslist_type::iterator (void)>
									rslist_iterator_begin_func_type;
	typedef	boost::function< rslist_type::iterator (void)>
									rslist_iterator_end_func_type;
	typedef	boost::function< rslist_type::iterator (rslist_type::iterator)>
									rslist_iterator_next_func_type;
	typedef	boost::function< void (		const boost::thread::id,
										rslist_iterator_begin_func_type,
										rslist_iterator_end_func_type,
										rslist_iterator_next_func_type,
										boost::asio::ip::tcp::endpoint& ) >
								tcp_schedule_func_type;

	typedef	l7vs::atomic<unsigned long long>	AUUL;

	//!	@struct	replication_header replication header structure
	struct	replication_header{
		unsigned int	data_num;
		replication_header() : data_num( 0 ) {}
	};
	//!	@struct	replication_data replication data structure
	struct	replication_data{
		bool				udpmode;
		char				tcp_endpoint[48];
		char				udp_endpoint[48];
		long long			sorry_maxconnection;
		char				sorry_endpoint[48];
		bool				sorry_flag;
		unsigned long long	qos_up;
		unsigned long long	qos_down;
	};

	const static int	SESSION_POOL_NUM_DEFAULT	= 256;		//! Default count for number of session-pool size
	const static long	BPS_INTERVAL_DEFAULT		= 500;		//! bps calcurate interval(500ms)
	const static long	REP_INTERVAL_DEFAULT		= 500;		//! replication-data create interval(500ms)
	const static int	MAX_REPLICATION_DATA_NUM	= 64;		//! Maximum count value of replication data array
	const static int	OPERATION_TIMEOUT			= 1;		//! Operation timed out value
	const static int	REFCOUNT_WAIT_INTERVAL		= 10000;	//! wait interval for rs_ref_count check
	const static int	SCHEDULER_PRIORITY		= 20;
protected:

	struct	parameter_data{
		std::string
				nic_realserver_side;
		int		session_pool_size;
		long	bps_interval;
		long	rep_interval;
		parameter_data() :	session_pool_size( SESSION_POOL_NUM_DEFAULT ),
							bps_interval( BPS_INTERVAL_DEFAULT ),
							rep_interval( REP_INTERVAL_DEFAULT ){}
	};

	boost::thread::id			this_id;

	const	l7vsd&				vsd;			//! l7vsd reference
	const	replication&		rep;			//! replication reference

	Logger						log;

	boost::asio::io_service		dispatcher;		//! dispatcer service
	deadline_timer_ptr_type		calc_bps_timer;	//! timer object
	deadline_timer_ptr_type		replication_timer;	//! timer object
	deadline_timer_ptr_type		protomod_rep_timer;	//! timer object
	deadline_timer_ptr_type		schedmod_rep_timer;	//! timer object

	parameter_data				param_data;		//! virtual service parameter data
	virtualservice_element		element;		//! virtual service element
	boost::mutex				element_mutex;	//! mutex for virtual service element

	protocol_module_base*		protomod;			//! protocol module smart pointer
	schedule_module_base*		schedmod;			//! schedule module smart pointer

	std::list<realserver>		rs_list;						//! realserver list
	AUUL						rs_list_ref_count;				//! reference count of realserver list
	wr_mutex					rs_list_ref_count_inc_mutex;	//! mutex for increase reference count

	AUUL						recvsize_up;					//! upstream total receive data size
	AUUL						current_up_recvsize;			//! current upstream receive data size for calcurate upstream throughput
	AUUL						sendsize_up;					//! upstream total send data size
	AUUL						recvsize_down;					//! downstream total receive data size
	AUUL						current_down_recvsize;			//! current downstream receive data size for calcurate upstream throughput
	AUUL						sendsize_down;					//! downstream total send data size

	AUUL						throughput_up;					//! upstream throughput value
	AUUL						throughput_down;				//! downstream throughput value

	AUUL						wait_count_up;					//! upstream recv wait count
	AUUL						wait_count_down;				//! downstream recv wait count

	void						load_parameter( l7vs::error_code& );

	virtual	void				handle_replication_interrupt( const boost::system::error_code& ) = 0;
	virtual	void				read_replicationdata() = 0;

	void						handle_protomod_replication( const boost::system::error_code& );
	void						handle_schedmod_replication( const boost::system::error_code& );

	void						handle_throughput_update( const boost::system::error_code& );

	std::list<realserver>::iterator
								rs_list_begin(){
		return rs_list.begin();
	}
	std::list<realserver>::iterator
								rs_list_end(){
		return rs_list.end();
	}
	std::list<realserver>::iterator
								rs_list_next( std::list<realserver>::iterator in_itr ){
		return ++in_itr;
	}
	void*						replication_pay_memory( const std::string& inid, unsigned int* outsize ){
		l7vs::replication&	tmp_rep = const_cast<l7vs::replication&>( rep );
		return tmp_rep.pay_memory( inid, *outsize );
	}
	void						replication_area_lock( const std::string& inid ){
		l7vs::replication&	tmp_rep = const_cast<l7vs::replication&>( rep );
		tmp_rep.lock( inid );
	}
	void						replication_area_unlock( const std::string& inid ){
		l7vs::replication&	tmp_rep = const_cast<l7vs::replication&>( rep );
		tmp_rep.unlock( inid );
	}

	void						schedule_rs(
										boost::thread::id thread_id,
										rslist_iterator_begin_func_type in_begin,
										rslist_iterator_end_func_type in_end,
										rslist_iterator_next_func_type in_next,
										boost::asio::ip::tcp::endpoint& in_ep ){
		schedmod->handle_schedule( thread_id, in_begin, in_end, in_next, in_ep );
	}

	cpu_set_t					vsnic_cpumask;
	cpu_set_t					rsnic_cpumask;
	void						get_nic_list( std::vector< std::string >& );
	cpu_set_t					get_cpu_mask( boost::asio::ip::address& );
	cpu_set_t					get_cpu_mask( std::string );

public:
	explicit virtualservice_base(	const l7vsd&,
							const replication&,
							const virtualservice_element& );
	virtual	~virtualservice_base(){};

	virtual	void				initialize( error_code& ) = 0;
	virtual	void				finalize( error_code& ) = 0;

	virtual	bool				operator==( const virtualservice_base& ) = 0;
	virtual	bool				operator!=( const virtualservice_base& ) = 0;

	void						rs_list_lock(){
									rd_scoped_lock	lock( rs_list_ref_count_inc_mutex );
									rs_list_ref_count++;
								}
	void						rs_list_unlock() { rs_list_ref_count--; };

	virtual	void				set_virtualservice( const virtualservice_element&, error_code& ) = 0;
	virtual	void				edit_virtualservice( const virtualservice_element&, error_code& ) = 0;

	virtual	void				add_realserver( const virtualservice_element&, error_code& ) = 0;
	virtual	void				edit_realserver( const virtualservice_element&, error_code& ) = 0;
	virtual	void				del_realserver( const virtualservice_element&, error_code& ) = 0;

	virtualservice_element&		get_element();

	virtual	void				run() = 0;
	virtual	void				stop() = 0;

	virtual	void				connection_active( const tcp_endpoint_type& ) = 0;
	virtual	void				connection_inactive( const tcp_endpoint_type& ) = 0;
	virtual	void				release_session( const tcp_session* session_ptr ) = 0;

	unsigned long long			get_qos_upstream(){ return element.qos_upstream; }
	unsigned long long			get_qos_downstream(){ return element.qos_downstream; }
	unsigned long long			get_throughput_upstream(){ return throughput_up.get(); }
	unsigned long long			get_throughput_downstream(){ return throughput_down.get(); }
	unsigned long long			get_up_recv_size(){ return recvsize_up.get(); }
	unsigned long long			get_up_send_size(){ return sendsize_up.get(); }
	unsigned long long			get_down_recv_size(){ return recvsize_down.get(); }
	unsigned long long			get_down_send_size(){ return sendsize_down.get(); }

	unsigned long long			get_wait_upstream(){ return wait_count_up.get(); }
	unsigned long long			get_wait_downstream(){ return wait_count_down.get(); }

	void						update_up_recv_size( unsigned long long datasize ){
		current_up_recvsize	+= datasize;
		recvsize_up		+= datasize;
	}
	void						update_up_send_size( unsigned long long datasize ){
		sendsize_up += datasize;
	}
	void						update_down_recv_size( unsigned long long datasize ){
		current_down_recvsize += datasize;
		recvsize_down += datasize;
	}
	void						update_down_send_size( unsigned long long datasize ){
		sendsize_down += datasize;
	}

	protocol_module_base*		get_protocol_module(){ return protomod; }
	schedule_module_base*		get_schedule_module(){ return schedmod; }

};

//!
//!	@brief	virtualservice class for TCP
//! @class	virtualservice_tcp is class of virtual service for TCP transfer.
class	virtualservice_tcp : public virtualservice_base{
public:
	typedef	lockfree_queue< session_thread_control >
								session_queue_type;
	typedef	lockfree_hashmap< tcp_session, session_thread_control >
								session_map_type;
protected:
	boost::asio::ip::tcp::acceptor
								acceptor_;

	session_queue_type			pool_sessions;
	session_map_type			waiting_sessions;
	session_map_type			active_sessions;
	l7vs::atomic<unsigned long long>	sorry_count;

	bool 						defer_accept_opt;				//! is set option TCP_DEFER_ACCEPT
	int 						defer_accept_val;				//! TCP_DEFER_ACCEPT option value
	l7vs::tcp_socket::tcp_socket_option_info set_sock_opt;		//! socket option for tcp_session class

	
	void						handle_replication_interrupt( const boost::system::error_code& );
	void						read_replicationdata();

	void						handle_accept( const session_thread_control*, const boost::system::error_code& );

public:
	virtualservice_tcp(		const l7vsd&,
							const replication&,
							const virtualservice_element& );
	~virtualservice_tcp();

	void						initialize( error_code& );
	void						finalize( error_code& );

	bool						operator==( const virtualservice_base& );
	bool						operator!=( const virtualservice_base& );

	void						set_virtualservice( const virtualservice_element&, error_code& );
	void						edit_virtualservice( const virtualservice_element&, error_code& );

	void						add_realserver( const virtualservice_element&, error_code& );
	void						edit_realserver( const virtualservice_element&, error_code& );
	void						del_realserver( const virtualservice_element&, error_code& );

	void						run();
	void						stop();

	void						connection_active( const tcp_endpoint_type& );
	void						connection_inactive( const tcp_endpoint_type& );
	void						release_session( const tcp_session* session_ptr );
	
	protocol_module_base::check_message_result parse_socket_option(std::vector<std::string>& args);
	
};

//!
//!	@brief	virtualservice class for UDP
//! @class	virtualservice_udp is class of virtual service for UDP transfer.
class	virtualservice_udp : public virtualservice_base{
protected:
	boost::shared_ptr<udp_session>
								session;

	void						handle_replication_interrupt( const boost::system::error_code& );
	void						read_replicationdata();

public:
	virtualservice_udp(		const l7vsd&,
							const replication&,
							const virtualservice_element& );
	~virtualservice_udp();


	void						initialize( error_code& );
	void						finalize( error_code& );

	bool						operator==( const virtualservice_base& );
	bool						operator!=( const virtualservice_base& );

	void						set_virtualservice( const virtualservice_element&, error_code& );
	void						edit_virtualservice( const virtualservice_element&, error_code& );

	void						add_realserver( const virtualservice_element&, error_code& );
	void						edit_realserver( const virtualservice_element&, error_code& );
	void						del_realserver( const virtualservice_element&, error_code& );

	void						run();
	void						stop();

	void						connection_active( const boost::asio::ip::tcp::endpoint& ){}
	void						connection_inactive( const boost::asio::ip::tcp::endpoint& ){}
	void						release_session( const tcp_session* session_ptr );
};

//!
//!	@brief	virtualservice wrapping class
//! @class	virtual_service
class	virtual_service{
protected:
	boost::shared_ptr<virtualservice_base>	vs;
public:
	virtual_service(	const l7vsd& ,
						const replication& ,
						const virtualservice_element& );
	~virtual_service();
	
	void						initialize( error_code& );
	void						finalize( error_code& );

	bool						operator==( const virtualservice_base& );
	bool						operator!=( const virtualservice_base& );

	void						set_virtualservice( const virtualservice_element& , error_code& );
	void						edit_virtualservice( const virtualservice_element& , error_code& );

	void						add_realserver( const virtualservice_element& , error_code& );
	void						edit_realserver( const virtualservice_element& , error_code& );
	void						del_realserver( const virtualservice_element& , error_code& );

	virtualservice_element&		get_element();

	void						run();
	void						stop();

	void						connection_active( const boost::asio::ip::tcp::endpoint&  );
	void						connection_inactive( const boost::asio::ip::tcp::endpoint&  );
	void						release_session( const tcp_session* session_ptr );

	unsigned long long			get_qos_upstream();
	unsigned long long			get_qos_downstream();
	unsigned long long			get_throughput_upstream();
	unsigned long long			get_throughput_downstream();
	unsigned long long			get_up_recv_size();
	unsigned long long			get_up_send_size();
	unsigned long long			get_down_recv_size();
	unsigned long long			get_down_send_size();

	void						update_up_recv_size( unsigned long long );
	void						update_up_send_size( unsigned long long );
	void						update_down_recv_size( unsigned long long );
	void						update_down_send_size( unsigned long long );

	protocol_module_base*		get_protocol_module();
	schedule_module_base*		get_schedule_module();
};

}	//namespace l7vs

#endif//VIRTUALSERVICE_H

