//
//	@file	virtualservice.h
//	@brief	VirtualService interface class
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

#include "stub.h"

#include "realserver.h"
#include "virtualservice_element.h"
//#include "replication.h"
#include "protocol_module_base.h"
#include "schedule_module_base.h"
#include "session_thread_control.h"

#define	SESSION_POOL_NUM_DEFAULT	256

namespace l7vs{

struct	vs_operation_result{
	bool			flag;
	std::string		message;
	vs_operation_result() : flag(true), message(""){}
	bool	operator==( const vs_operation_result& in )
				{ return ( ( flag == in.flag ) && ( message == in.message ) ); }
	bool	operator!=( const vs_operation_result& in )
				{ return ( ( flag != in.flag ) || ( message != in.message ) ); }
};

struct	vs_replication_header{
	unsigned long long	udpmode;
	unsigned char		address[16];
	unsigned long long	port;
	unsigned long long	bodytype;
	unsigned long long	datasize;
};

struct	vs_replication_body{
	unsigned long long	sorry_maxconnection;
	unsigned char		sorry_address[16];
	unsigned long long	sorry_port;
	unsigned long long	sorry_flag;
	unsigned long long	qos_up;
	unsigned long long	qos_down;
};

struct	vs_replication_data{
	vs_replication_header	header;
	vs_replication_body		body;
};

class	virtualservice_base : boost::noncopyable{
public:
	typedef	boost::shared_ptr<session_thread_control>	session_thread_control_ptr;
protected:
	const	l7vs::l7vsd&		vsd;
	const	l7vs::replication&	rep;

	boost::asio::io_service		dispatcher;
	boost::asio::deadline_timer	vs_timer;

	virtualservice_element		element;

	boost::shared_ptr<protocol_module_base>	protomod;
	boost::shared_ptr<schedule_module_base>	schedmod;
	std::list<realserver>		rs_list;
	std::list<boost::mutex>		rs_mutex;
	unsigned int				rs_list_ref_count;
	boost::mutex				rs_list_ref_count_mutex;
	boost::mutex				rs_list_ref_count_inc_mutex;

	unsigned long long			recvsize_up;
	unsigned long long			current_up_recvsize;
	boost::mutex				recvsize_up_mutex;
	unsigned long long			sendsize_up;
	boost::mutex				sendsize_up_mutex;
	unsigned long long			recvsize_down;
	unsigned long long			current_down_recvsize;
	boost::mutex				recvsize_down_mutex;
	unsigned long long			sendsize_down;
	boost::mutex				sendsize_down_mutex;

	unsigned long long			last_calc_time;
	unsigned long long			throughput_up;
	unsigned long long			throughput_down;

	virtual	void				handle_replication_interrupt( const boost::system::error_code& ) = 0;
	virtual	bool				read_replicationdata( vs_replication_data& ) = 0;

	void						handle_protomod_replication( const boost::system::error_code& );
	void						handle_schedmod_replication( const boost::system::error_code& );

	void						handle_throughput_update( const boost::system::error_code& );
public:
	virtualservice_base(	const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const virtualservice_element& inelement);
	virtual	~virtualservice_base(){};

	virtual	vs_operation_result	initialize() = 0;
	virtual	vs_operation_result	finalize() = 0;

	virtual	bool				operator==( const virtualservice_base& ) = 0;
	virtual	bool				operator!=( const virtualservice_base& ) = 0;

	void						rs_list_lock();
	void						rs_list_unlock();

	virtual	vs_operation_result	set_virtualservce( const virtualservice_element& ) = 0;
	virtual	vs_operation_result	edit_virtualservce( const virtualservice_element& ) = 0;

	virtual	vs_operation_result	add_realserver( const virtualservice_element& ) = 0;
	virtual	vs_operation_result	edit_realserver( const virtualservice_element& ) = 0;
	virtual	vs_operation_result	del_realserver( const virtualservice_element& ) = 0;

	virtualservice_element&		get_element(){ return element; }

	virtual	void				run() = 0;
	virtual	void				stop() = 0;

	virtual	void				connection_active( const boost::asio::ip::tcp::endpoint& ) = 0;
	virtual	void				connection_inactive( const boost::asio::ip::tcp::endpoint& ) = 0;
	virtual	void				release_session( const boost::thread::id thread_id ) = 0;

	unsigned long long			get_qos_upstream();
	unsigned long long			get_qos_downstream();
	unsigned long long			get_throughput_upstream();
	unsigned long long			get_throughput_downstream();

	void						update_up_recv_size( unsigned long long	datasize );
	void						update_up_send_size( unsigned long long	datasize );
	void						update_down_recv_size( unsigned long long	datasize );
	void						update_down_send_size( unsigned long long	datasize );
	
	boost::shared_ptr<protocol_module_base>
								get_protocol_module();
	boost::shared_ptr<schedule_module_base>
								get_schedule_module();
};

class	virtualservice_tcp : public virtualservice_base{
public:
	typedef	std::map< boost::thread::id, boost::shared_ptr<session_thread_control> >
								session_map_type;
protected:
	boost::asio::ip::tcp::acceptor
								acceptor_;

	session_map_type			pool_sessions;
	session_map_type			active_sessions;

	void						handle_replication_interrupt( const boost::system::error_code& );
	bool						read_replicationdata( vs_replication_data& );

	void						handle_accept(	const session_thread_control_ptr,
												const boost::system::error_code& );

public:
	virtualservice_tcp(		const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const virtualservice_element& inelement);
	~virtualservice_tcp();

	vs_operation_result			initialize();
	vs_operation_result			finalize();

	bool						operator==( const virtualservice_base& );
	bool						operator!=( const virtualservice_base& );

	vs_operation_result			set_virtualservce( const virtualservice_element& );
	vs_operation_result			edit_virtualservce( const virtualservice_element& );

	vs_operation_result			add_realserver( const virtualservice_element& );
	vs_operation_result			edit_realserver( const virtualservice_element& );
	vs_operation_result			del_realserver( const virtualservice_element& );

	void						run();
	void						stop();

	void						connection_active( const boost::asio::ip::tcp::endpoint& );
	void						connection_inactive( const boost::asio::ip::tcp::endpoint& );
	void						release_session( const boost::thread::id thread_id );
};

class	virtualservice_udp : public virtualservice_base{
protected:
	udp_session					session;

	void						handle_replication_interrupt( const boost::system::error_code& );
	bool						read_replicationdata( vs_replication_data& );

public:
	virtualservice_udp(		const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const virtualservice_element& inelement);
	~virtualservice_udp();


	vs_operation_result			initialize();
	vs_operation_result			finalize();

	bool						operator==( const virtualservice_base& );
	bool						operator!=( const virtualservice_base& );

	vs_operation_result			set_virtualservce( const virtualservice_element& );
	vs_operation_result			edit_virtualservce( const virtualservice_element& );

	vs_operation_result			add_realserver( const virtualservice_element& );
	vs_operation_result			edit_realserver( const virtualservice_element& );
	vs_operation_result			del_realserver( const virtualservice_element& );

	void						run();
	void						stop();

	void						connection_active( const boost::asio::ip::tcp::endpoint& );
	void						connection_inactive( const boost::asio::ip::tcp::endpoint& );
	void						release_session( const boost::thread::id thread_id );
};

class	virtual_service{
protected:
	boost::shared_ptr<virtualservice_base>	vs;
public:
	virtual_service(	const l7vs::l7vsd& invsd,
						const l7vs::replication& inrep,
						const virtualservice_element& inelement );
	~virtual_service();
	
	vs_operation_result			initialize();
	vs_operation_result			finalize();

	bool						operator==( const virtualservice_base& in );
	bool						operator!=( const virtualservice_base& in );

	vs_operation_result			set_virtualservce( virtualservice_element& in );
	vs_operation_result			edit_virtualservce( virtualservice_element& in );

	vs_operation_result			add_realserver( virtualservice_element& in );
	vs_operation_result			edit_realserver( virtualservice_element& in );
	vs_operation_result			del_realserver( virtualservice_element& in );

	virtualservice_element&		get_element();

	void						run();
	void						stop();

	void		connection_active( const boost::asio::ip::tcp::endpoint& in );
	void		connection_inactive( const boost::asio::ip::tcp::endpoint& in );
	void		release_session( boost::thread::id thread_id );

	unsigned long long			get_qos_upstream();
	unsigned long long			get_qos_downstream();
	unsigned long long			get_throughput_upstream();
	unsigned long long			get_throughput_downstream();

	void						update_up_recv_size( unsigned long long	datasize );
	void						update_up_send_size( unsigned long long	datasize );
	void						update_down_recv_size( unsigned long long	datasize );
	void						update_down_send_size( unsigned long long	datasize );
	
	boost::shared_ptr<protocol_module_base>
								get_protocol_module();
	boost::shared_ptr<schedule_module_base>
								get_schedule_module();
};

}	//namespace l7vs

#endif//VIRTUALSERVICE_H
