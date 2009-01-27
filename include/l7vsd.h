#ifndef	L7VSD_H
#define L7VSD_H

#include <string>
#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include "error_code.h"
#include "command_receiver.h"
#include "virtualservice_element.h"
#include "virtualservice.h"
#include "realserver_element.h"
#include "realserver.h"
#include "replication.h"
#include "snmpbridge.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"
#include "logger.h"
#include "parameter.h"

#ifndef	L7VS_MODULE_PATH
	#define L7VS_MODULE_PATH		"."
#endif	//L7VS_MODULE_PATH

#ifndef	L7VS_CONFIG_SOCK_PATH
	#define L7VS_CONFIG_SOCK_PATH	"/var/run/l7vs"
#endif
#define L7VS_CONFIG_SOCKNAME		L7VS_CONFIG_SOCK_PATH "/l7vs"

namespace l7vs{
class virtual_service;

//! l7vsd main class
class l7vsd : private boost::noncopyable{
public:
	typedef boost::shared_ptr< command_receiver >	command_receiver_ptr;	//!< shared_ptr command_receiver typedef
	typedef boost::shared_ptr< replication >		replication_ptr;		//!< shared_ptr replication typedef
	typedef boost::shared_ptr< snmpbridge >			snmpbridge_ptr;			//!< shared_ptr snmp_bridge typedef

	typedef std::list< boost::shared_ptr< virtual_service > >	vslist_type;	//!< virtual service list typedef
	//typedef std::vector< virtualservice_element >				vsvec_type;		//!< virtual service element vector typedef
	typedef std::list< virtualservice_element >					vselist_type;	//!< virtual service element list typedef

	l7vsd();				//!< constructor
	virtual	~l7vsd();		//!< destructor

protected:
	boost::thread_group			vs_threads;			//!< virtual_service thread group
	boost::asio::io_service		dispatcher;			//!< dispatcher

	mutable	vslist_type			vslist;				//!< virtual_service list

	command_receiver_ptr		receiver;			//!< command_receiver ptr
	replication_ptr				rep;				//!< replication ptr
	snmpbridge_ptr				bridge;				//!< snmp_bridge ptr

	boost::mutex				command_mutex;		//!< command execute mutex
	boost::mutex				vslist_mutex;		//!< virtual service list mutex

	virtual	vslist_type::iterator
								search_vslist( const virtualservice_element& )	const;	//!< vs_list search function

	bool						help;				//!< help mode
	bool						debug;				//!< debug mode

	//! option parse function object type.
	typedef	boost::function< bool ( int&, int, char*[] ) >
			parse_opt_func_type;
	//! option string - parse function object map type
	typedef	std::map< std::string, parse_opt_func_type >
			parse_opt_map_type;
	//! list option function map dictionary.
	parse_opt_map_type	option_dic;

public:
	void	list_virtual_service( vselist_type&, error_code&  );	//!< virtual_service list command

	void	add_virtual_service( const virtualservice_element&, error_code& );	//!< virtual_service add command
	void	del_virtual_service( const virtualservice_element&, error_code& );	//!< virtual_service del command
	void	edit_virtual_service( const virtualservice_element&, error_code& );	//!< virtual_service edit command

	void	add_real_server( const virtualservice_element&, error_code& );	//!< real_server add command
	void	del_real_server( const virtualservice_element&, error_code& );	//!< real_server del command
	void	edit_real_server( const virtualservice_element&, error_code& );	//!< real_server edit command

	void	flush_virtual_service( error_code& );	//!< all virtual_service delete command

	void	get_replication_info( replication::REPLICATION_MODE_TAG&, error_code& );	//!< get replication info command
	void	start_replication( error_code& );				//!< start replication command
	void	stop_replication( error_code& );				//!< stop replication command
	void	dump_replication_memory( error_code& );			//!< dump replication memory command
	void	force_replicate( error_code& );					//!< force replicate command

	void	get_log_level( const LOG_CATEGORY_TAG, LOG_LEVEL_TAG&, error_code& );		//!< get loglevel command
	void	set_log_level( const LOG_CATEGORY_TAG, const LOG_LEVEL_TAG, error_code& );	//!< set loglevel command
	void	set_log_level_all( const LOG_LEVEL_TAG, error_code& );						//!< set all category loglevel command

	void	get_snmp_connect_status( int&, error_code& );									//!< get snmp connect status command
	void	get_snmp_log_level( const LOG_CATEGORY_TAG, LOG_LEVEL_TAG&, error_code& );		//!< get snmp loglevel command
	void	set_snmp_log_level( const LOG_CATEGORY_TAG, const LOG_LEVEL_TAG, error_code& );	//!< set snmp loglevel command
	void	set_snmp_log_level_all( const LOG_LEVEL_TAG, error_code& );						//!< set snmp all loglevel command

	void	reload_parameter( const PARAMETER_COMPONENT_TAG, error_code& );	//!< reload component parameter command

	int		run( int, char*[] );		//!< l7vsd run method

	void	release_virtual_service( const virtualservice_element& )	const;		//!< virtualservice release from vslist

protected:
	bool	is_exit_requested();		//!< check if exit requested

	bool	parse_help( int&, int, char*[] );		//!< parse help func
	bool	parse_debug( int&, int, char*[] );		//!< parse debug func

	bool	check_options( int, char*[] );		//!< check option func
	std::string	usage();				//!< make usage message

};

}	//namespace l7vs
#endif	//L7VSD_H
