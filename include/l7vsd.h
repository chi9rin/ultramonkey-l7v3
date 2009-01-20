#ifndef	L7VSD_H
#define L7VSD_H

#include <list>
#include <vector>
#include "l7vs_command_receiver.h"
#include "l7vs_virtual_service_command_element.h"
#include "l7vs_virtual_service.h"
#include "l7vs_realserver_element.h"
#include "l7vs_realserver.h"
#include "logger/logger.h"
#include "parameter/parameter.h"

namespace l7vs{

//! l7vsd main class
class l7vsd{
public:
	typedef shared_ptr< command_receiver >	command_receiver_ptr;	//!< shared_ptr command_receiver typedef
	typedef shared_ptr< replication >		replication_ptr;		//!< shared_ptr replication typedef
	typedef shared_ptr< snmp_bridge >		snmp_bridge_ptr;		//!< shared_ptr snmp_bridge typedef

	typedef std::list< virtual_service >	vs_list_type;			//!< virtual_service list typedef
	
	struct l7vsd_operation_result{
		bool	flag;
		std::string	message;
		l7vsd_operation_result() : flag(true), message(""){}
		bool	operator==( const l7vsd_operation_result& in )
				{ return ( ( flag == in.flag ) && ( message == in.message ) ); }
		bool	operator!=( const l7vsd_operation_result& in )
				{ return ( ( flag != in.flag ) || ( message != in.message ) ); }
	};

protected:
	boost::thread_group			vs_threads;			//!< virtual_service thread group
	boost::asio::io_service		dispatcher;			//!< dispatcher

	vs_list_type				vs_list;			//!< virtual_service list

	command_receiver_ptr		cmd_receiver;		//!< command_receiver ptr
	replication_ptr				repli;				//!< replication ptr
	snmp_bridge_ptr				bridge;				//!< snmp_bridge ptr

	vs_list_type::iterator		search_vslist( virtualservice_element& );	//!< vs_list search function

public:
	l7vsd_operation_result		list_virtual_service( std::vector<virtualservice_element>&  );	//!< virtual_service list command

	l7vsd_operation_result		add_virtual_service( const virtualservice_element& );	//!< virtual_service add command
	l7vsd_operation_result		del_virtual_service( const virtualservice_element& );	//!< virtual_service del command
	l7vsd_operation_result		edit_virtual_service( const virtualservice_element& );	//!< virtual_service edit command

	l7vsd_operation_result		add_real_server( const virtualservice_element& );	//!< real_server add command
	l7vsd_operation_result		del_real_server( const virtualservice_element& );	//!< real_server del command
	l7vsd_operation_result		edit_real_server( const virtualservice_element& );	//!< real_server edit command

	l7vsd_operation_result		flush_virtual_service();	//!< all virtual_service delete command

	l7vsd_operation_result		get_replication_info( REPLICATION_MODE_TAG& );	//!< get replication info command
	l7vsd_operation_result		start_replication();		//!< start replication command
	l7vsd_operation_result		stop_replication();			//!< stop replication command
	l7vsd_operation_result		dump_replication_memory();	//!< dump replication memory command
	l7vsd_operation_result		force_replicate();			//!< force replicate command

	l7vsd_operation_result		get_log_level( const LOG_CATEGORY_TAG, LOG_LEVEL_TAG& );		//!< get loglevel command
	l7vsd_operation_result		set_log_level( const LOG_CATEGORY_TAG, const LOG_LEVEL_TAG );	//!< set loglevel command
	l7vsd_operation_result		set_log_level_all( const LOG_LEVEL_TAG );						//!< set all category loglevel command

	l7vsd_operation_result		get_snmp_connect_status( int& );									//!< get snmp connect status command
	l7vsd_operation_result		get_snmp_log_level( const LOG_CATEGORY_TAG, LOG_LEVEL_TAG& );		//!< get snmp loglevel command
	l7vsd_operation_result		set_snmp_log_level( const LOG_CATEGORY_TAG, const LOG_LEVEL_TAG );	//!< set snmp loglevel command
	l7vsd_operation_result		set_snmp_log_level_all( const LOG_LEVEL_TAG );						//!< set snmp all loglevel command

	l7vsd_operation_result		reload_parameter( const PARAMETER_COMPONENT_TAG );	//!< reload component parameter command

	void						run();		//!< l7vsd run method

protected:
	bool						is_exit_requested();		//!< check if exit requested

};

}	//namespace l7vs
#endif	//L7VSD_H
