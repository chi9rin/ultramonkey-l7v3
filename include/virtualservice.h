//
//!	@file	virtualservice.h
//!	@brief	VirtualService interface class
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
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

#define	PARAM_RS_SIDE_NIC_NAME	"nic_realserver_side"
#define	PARAM_POOLSIZE_KEY_NAME	"session_thread_pool_size"
#define	PARAM_BPS_CALC_INTERVAL	"throughput_calc_interval"
#define	PARAM_SCHED_ALGORITHM	"task_scheduler_algorithm"
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

	typedef	boost::shared_ptr<boost::mutex>					mutex_ptr;

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
protected:

	struct	parameter_data{
		std::string
				nic_realserver_side;
		int		session_pool_size;
		long	bps_interval;
		int		schedule_algorithm;
		long	rep_interval;
		parameter_data() :	session_pool_size( SESSION_POOL_NUM_DEFAULT ),
							bps_interval( BPS_INTERVAL_DEFAULT ),
							schedule_algorithm( SCHED_OTHER ),
							rep_interval( REP_INTERVAL_DEFAULT ) {}
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
	std::map< tcp_endpoint_type,mutex_ptr >
								rs_mutex_list;					//! list of realserver mutex
	unsigned long long 			rs_list_ref_count;				//! reference count of realserver list
	boost::mutex				rs_list_ref_count_mutex;		//! mutex for update reference count
	boost::mutex				rs_list_ref_count_inc_mutex;	//! mutex for increase reference count

	unsigned long long			recvsize_up;					//! upstream total receive data size
	unsigned long long			current_up_recvsize;			//! current upstream receive data size for calcurate upstream throughput
//	boost::mutex				recvsize_up_mutex;				//! mutex for update upstream receive data size
	wr_mutex					recvsize_up_mutex;				//! mutex for update upstream receive data size
	unsigned long long			sendsize_up;					//! upstream total send data size
//	boost::mutex				sendsize_up_mutex;				//! mutex for update upstream send data size
	wr_mutex					sendsize_up_mutex;				//! mutex for update upstream send data size
	unsigned long long			recvsize_down;					//! downstream total receive data size
	unsigned long long			current_down_recvsize;			//! current downstream receive data size for calcurate upstream throughput
//	boost::mutex				recvsize_down_mutex;			//! mutex for update downstream receive data size
	wr_mutex					recvsize_down_mutex;			//! mutex for update downstream receive data size
	unsigned long long			sendsize_down;					//! downstream total send data size
//	boost::mutex				sendsize_down_mutex;			//! mutex for update downstream send data size
	wr_mutex					sendsize_down_mutex;			//! mutex for update downstream send data size

	unsigned long long			throughput_up;					//! upstream throughput value
//	boost::mutex				throughput_up_mutex;			//! mutex for update upstream throughput value
	wr_mutex					throughput_up_mutex;			//! mutex for update upstream throughput value
	unsigned long long			throughput_down;				//! downstream throughput value
//	boost::mutex				throughput_down_mutex;			//! mutex for update downstream throughput value
	wr_mutex					throughput_down_mutex;			//! mutex for update downstream throughput value

	unsigned long long			wait_count_up;					//! upstream recv wait count
//	boost::mutex				wait_count_up_mutex;			//! mutex for upstream recv wait count
	wr_mutex					wait_count_up_mutex;			//! mutex for upstream recv wait count
	unsigned long long			wait_count_down;				//! downstream recv wait count
//	boost::mutex				wait_count_down_mutex;			//! mutex for downstream recv wait count
	wr_mutex					wait_count_down_mutex;			//! mutex for downstream recv wait count

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
	virtualservice_base(	const l7vsd&,
							const replication&,
							const virtualservice_element& );
	virtual	~virtualservice_base(){};

	virtual	void				initialize( error_code& ) = 0;
	virtual	void				finalize( error_code& ) = 0;

	virtual	bool				operator==( const virtualservice_base& ) = 0;
	virtual	bool				operator!=( const virtualservice_base& ) = 0;

	void						rs_list_lock();
	void						rs_list_unlock();

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
	virtual	void				release_session( const boost::thread::id ) = 0;

	unsigned long long			get_qos_upstream();
	unsigned long long			get_qos_downstream();
	unsigned long long			get_throughput_upstream();
	unsigned long long			get_throughput_downstream();
	unsigned long long			get_up_recv_size();
	unsigned long long			get_up_send_size();
	unsigned long long			get_down_recv_size();
	unsigned long long			get_down_send_size();
	unsigned long long			get_wait_upstream();
	unsigned long long			get_wait_downstream();

	void						update_up_recv_size( unsigned long long );
	void						update_up_send_size( unsigned long long );
	void						update_down_recv_size( unsigned long long );
	void						update_down_send_size( unsigned long long );

	protocol_module_base*		get_protocol_module();
	schedule_module_base*		get_schedule_module();
};

//!
//!	@brief	virtualservice class for TCP
//! @class	virtualservice_tcp is class of virtual service for TCP transfer.
class	virtualservice_tcp : public virtualservice_base{
public:
	typedef	std::vector<session_thread_control*>			session_queue_type;
	typedef	std::map< boost::thread::id, session_thread_control* >
								session_map_type;
	typedef	std::pair< boost::thread::id, session_thread_control* >
								session_map_pair_type;
protected:
	boost::asio::ip::tcp::acceptor
								acceptor_;

	session_queue_type			pool_sessions;
// 	boost::mutex				pool_sessions_mutex;
	wr_mutex					pool_sessions_mutex;
	session_map_type			waiting_sessions;
// 	boost::mutex				waiting_sessions_mutex;
	wr_mutex					waiting_sessions_mutex;
	session_map_type			active_sessions;
// 	boost::mutex				active_sessions_mutex;
	wr_mutex					active_sessions_mutex;
	session_map_type			sorry_sessions;
// 	boost::mutex				sorry_sessions_mutex;
	wr_mutex					sorry_sessions_mutex;

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
	void						release_session( const boost::thread::id );
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
	void						release_session( const boost::thread::id );
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
	void						release_session( const boost::thread::id );

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
