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

#include <string>
#include <vector>
#include <map>
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

#define	SESSION_POOL_NUM_DEFAULT	(256)

#define	PARAM_POOLSIZE_KEY_NAME	"SESSION_POOL_SIZE"

#define	PROTOMOD_NOTLOAD_ERROR_MSG	"Protocol Module not loaded"
#define	SCHEDMOD_NOTLOAD_ERROR_MSG	"Schedule Module not loaded"
#define	PROTOMOD_LOAD_ERROR_MSG	"Protocol Module load error"
#define	SCHEDMOD_LOAD_ERROR_MSG "Schedule Module load error"

namespace l7vs{

class	l7vsd;
class	replication;
class	session_thread_control;

//!	@class	virtualservice_base
//!	@brief	virtualservice base class is base class of tcp virtual service and udp virtual service.
class	virtualservice_base : boost::noncopyable{
public:
	//! shared_ptr session_thread_control typedef
	typedef	boost::shared_ptr<session_thread_control>		session_thread_control_ptr;
	typedef	boost::shared_ptr<boost::asio::deadline_timer>	deadline_timer_ptr_type;
protected:
	//!	@class	vs_replication_header replication data structure for header data
	class	replication_data{
	public:
		replication_data() : udpflag( false ) {}
		replication_data( const replication_data& in ){
			udpflag			= in.udpflag;
			tcp_endpoint	= in.tcp_endpoint;
			udp_endpoint	= in.udp_endpoint;
			sorry_maxconnection = in.sorry_maxconnection;
			sorry_endpoint	= in.sorry_endpoint;
			sorry_flag		= in.sorry_flag;
			qos_up			= in.qos_up;
			qos_down		= in.qos_down;
		}
		replication_data&	operator=( const replication_data& in ){
			udpflag			= in.udpflag;
			tcp_endpoint	= in.tcp_endpoint;
			udp_endpoint	= in.udp_endpoint;
			sorry_maxconnection = in.sorry_maxconnection;
			sorry_endpoint	= in.sorry_endpoint;
			sorry_flag		= in.sorry_flag;
			qos_up			= in.qos_up;
			qos_down		= in.qos_down;
			return *this;
		}
		bool				udpflag;
		boost::asio::ip::tcp::endpoint
							tcp_endpoint;
		boost::asio::ip::udp::endpoint
							udp_endpoint;
		long long			sorry_maxconnection;
		boost::asio::ip::tcp::endpoint
							sorry_endpoint;
		bool				sorry_flag;
		unsigned long long	qos_up;
		unsigned long long	qos_down;
	private:
		friend class	boost::serialization::access;		//! friend boost serializable class
		//! serializable
		//! @brief using boost serialiable. class serializable function.
		//! @param[in]	archive
		//! @param[in]	version
		template <class Archive > void serialize( Archive& ar, const unsigned int version ){
			ar & udpflag;
			ar & tcp_endpoint;
			ar & udp_endpoint;
			ar & sorry_maxconnection;
			ar & sorry_endpoint;
			ar & sorry_flag;
			ar & qos_up;
			ar & qos_down;
		}
	};

	struct	parameter_data{
		int	session_pool_size;
		parameter_data() : session_pool_size( SESSION_POOL_NUM_DEFAULT ){}
	};

	const	l7vsd&				vsd;			//! l7vsd reference
	const	replication&		rep;			//! replication reference

	boost::asio::io_service		dispatcher;		//! dispatcer service
	deadline_timer_ptr_type		calc_bps_timer;	//! timer object
	deadline_timer_ptr_type		replication_timer;	//! timer object

	parameter_data				param_data;		//! virtual service parameter data
	virtualservice_element		element;		//! virtual service element

	boost::shared_ptr<protocol_module_base>	protomod;			//! protocol module smart pointer
	boost::shared_ptr<schedule_module_base>	schedmod;			//! schedule module smart pointer

	std::list<realserver>		rs_list;						//! realserver list
	std::list<boost::mutex>		rs_mutex_list;					//! list of realserver mutex
	unsigned long long 			rs_list_ref_count;				//! reference count of realserver list
	boost::mutex				rs_list_ref_count_mutex;		//! mutex for update reference count
	boost::mutex				rs_list_ref_count_inc_mutex;	//! mutex for increase reference count

	unsigned long long			recvsize_up;					//! upstream total receive data size
	unsigned long long			current_up_recvsize;			//! current upstream receive data size for calcurate upstream throughput
	boost::mutex				recvsize_up_mutex;				//! mutex for update upstream receive data size
	unsigned long long			sendsize_up;					//! upstream total send data size
	boost::mutex				sendsize_up_mutex;				//! mutex for update upstream send data size
	unsigned long long			recvsize_down;					//! downstream total receive data size
	unsigned long long			current_down_recvsize;			//! current downstream receive data size for calcurate upstream throughput
	boost::mutex				recvsize_down_mutex;			//! mutex for update downstream receive data size
	unsigned long long			sendsize_down;					//! downstream total send data size
	boost::mutex				sendsize_down_mutex;			//! mutex for update downstream send data size

	boost::xtime				last_calc_time;					//! time at calcurate throughput
	unsigned long long			throughput_up;					//! upstream throughput value
	boost::mutex				throughput_up_mutex;			//! mutex for update upstream throughput value
	unsigned long long			throughput_down;				//! downstream throughput value
	boost::mutex				throughput_down_mutex;			//! mutex for update downstream throughput value

	void						load_parameter();

	virtual	void				handle_replication_interrupt( const boost::system::error_code& ) = 0;
	virtual	bool				read_replicationdata( replication_data& ) = 0;

	void						handle_protomod_replication( const boost::system::error_code& );
	void						handle_schedmod_replication( const boost::system::error_code& );

	void						handle_throughput_update( const boost::system::error_code& );
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

	virtualservice_element&		get_element(){ return element; }

	virtual	void				run() = 0;
	virtual	void				stop() = 0;

	virtual	void				connection_active( const boost::asio::ip::tcp::endpoint& ) = 0;
	virtual	void				connection_inactive( const boost::asio::ip::tcp::endpoint& ) = 0;
	virtual	void				release_session( const boost::thread::id ) = 0;

	unsigned long long			get_qos_upstream();
	unsigned long long			get_qos_downstream();
	unsigned long long			get_throughput_upstream();
	unsigned long long			get_throughput_downstream();

	void						update_up_recv_size( unsigned long long );
	void						update_up_send_size( unsigned long long );
	void						update_down_recv_size( unsigned long long );
	void						update_down_send_size( unsigned long long );

	boost::shared_ptr<protocol_module_base>
								get_protocol_module();
	boost::shared_ptr<schedule_module_base>
								get_schedule_module();
};

//!
//!	@brief	virtualservice class for TCP
//! @class	virtualservice_tcp is class of virtual service for TCP transfer.
class	virtualservice_tcp : public virtualservice_base{
public:
	typedef	std::map< boost::thread::id, boost::shared_ptr<session_thread_control> >
								session_map_type;
	typedef	std::pair< boost::thread::id, boost::shared_ptr<session_thread_control> >
								session_map_pair_type;
protected:
	boost::asio::ip::tcp::acceptor
								acceptor_;

	session_map_type			pool_sessions;
	session_map_type			active_sessions;

	void						handle_replication_interrupt( const boost::system::error_code& );
	bool						read_replicationdata( replication_data& );

	void						handle_accept(	const virtualservice_base::session_thread_control_ptr,
												const boost::system::error_code& );

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

	void						connection_active( const boost::asio::ip::tcp::endpoint& );
	void						connection_inactive( const boost::asio::ip::tcp::endpoint& );
	void						release_session( const boost::thread::id );
};

//!
//!	@brief	virtualservice class for UDP
//! @class	virtualservice_udp is class of virtual service for UDP transfer.
class	virtualservice_udp : public virtualservice_base{
protected:
	udp_session					session;

	void						handle_replication_interrupt( const boost::system::error_code& );
	bool						read_replicationdata( virtualservice_base::replication_data& );

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

	void						connection_active( const boost::asio::ip::tcp::endpoint& );
	void						connection_inactive( const boost::asio::ip::tcp::endpoint& );
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

	void						update_up_recv_size( unsigned long long );
	void						update_up_send_size( unsigned long long );
	void						update_down_recv_size( unsigned long long );
	void						update_down_send_size( unsigned long long );
	
	boost::shared_ptr<protocol_module_base>
								get_protocol_module();
	boost::shared_ptr<schedule_module_base>
								get_schedule_module();
};

}	//namespace l7vs

#endif//VIRTUALSERVICE_H
