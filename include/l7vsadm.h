//
//	l7vsadm.h
//	^^^^^^^^^
//	l7vsd control application.
//
// Copyright (c) Norihisa Nakai ( n dot nakai at sdy dot co dot jp )
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef	L7VSADM_H
#define	L7VSADM_H
#include <map>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <sstream>
#include "error_code.h"
#include "l7vs_command.h"

#ifndef	L7VS_MODULE_PATH
	#define	L7VS_MODULE_PATH	"./"
#endif

#ifndef	COMMAND_BUFFER_SIZE
	#define	COMMAND_BUFFER_SIZE (65535)
#endif

#ifndef	L7VS_CONFIG_SOCK_PATH
	#define L7VS_CONFIG_SOCK_PATH	"/var/run/l7vs"
#endif
#define L7VS_CONFIG_SOCKNAME		L7VS_CONFIG_SOCK_PATH "/l7vs"

namespace l7vs{

template < class T >
typename T::endpoint string_to_endpoint( std::string& str ){
	std::string::size_type pos = str.find( ":" );
	std::string	hostname = str.substr( 0, pos );
	std::string	portname = str.substr( pos+1, str.length() );
	boost::asio::io_service		io_service;
	typename T::resolver				resolver(io_service);
	typename T::resolver::query		query( hostname, portname );
	typename T::resolver::iterator	end;
	typename T::resolver::iterator	itr = resolver.resolve( query );
	if( itr == end ) return typename T::endpoint();
	return *itr;
}

class	l7vsadm{
protected:
	//
	//	command parse functions.
	//
	//! list command parse function
	bool	parse_list_func( l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
	//! virtualservice command parse function
	bool	parse_vs_func( l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
	//! realserver_command parse function
	bool	parse_rs_func( l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
	//! replication command parse function
	bool	parse_replication_func( l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
	//! log command parse function
	bool	parse_log_func( l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
	//! snmpagent command parse function
	bool	parse_snmp_func( l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
	//! parameter parse function
	bool	parse_parameter_func( l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
	//! help command parse func
	bool	parse_help_func( l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );

	//
	//	option parse functions.
	//
	//
	// list option functions.
	bool	parse_opt_list_numeric_func( int&, int, char*[] );
	//
	// virtualservice option functions.
	bool	parse_opt_vs_target_func( int&, int, char*[] );
	bool	parse_opt_vs_module_func( int&, int, char*[] );
	bool	parse_opt_vs_scheduler_func( int&, int, char*[] );
	bool	parse_opt_vs_upper_func( int&, int, char*[] );
	bool	parse_opt_vs_bypass_func( int&, int, char*[] );
	bool	parse_opt_vs_flag_func( int&, int, char*[] );
	bool	parse_opt_vs_qosup_func( int&, int, char*[] );
	bool	parse_opt_vs_qosdown_func( int&, int, char*[] );
	bool	parse_opt_vs_udp_func( int&, int, char*[] );
	// realserver option function
	bool	parse_opt_rs_weight_func( int&, int, char*[] );
	bool	parse_opt_rs_realserver_func( int&, int, char*[] );
	// replication option function
	bool	parse_opt_replication_switch_func( int&, int, char*[] );
	bool	parse_opt_replication_start_func( int&, int, char*[] );
	bool	parse_opt_replication_stop_func( int&, int, char*[] );
	bool	parse_opt_replication_force_func( int&, int, char*[] );
	bool	parse_opt_replication_dump_func( int&, int, char*[] );
	// log option function
	bool	parse_opt_log_category_func( int&, int, char*[] );
	bool	parse_opt_log_level_func( int&, int, char*[] );
	// snmp option function
	bool	parse_opt_snmp_log_category_func( int&, int, char*[] );
	bool	parse_opt_snmp_log_level_func( int&, int, char*[] );
	// parameter option function
	bool	parse_opt_parameter_reload_func( int&, int, char*[] );

	// command parse function object.type.
	typedef	boost::function< bool ( int, char*[] ) >
			parse_cmd_func_type;
	// command string - parse function object map type. 
	typedef	std::map< std::string, parse_cmd_func_type >
			parse_cmd_map_type;
	// command function map dictionary.
	parse_cmd_map_type	command_dic;

	// option parse function object type.
	typedef	boost::function< bool ( int&, int, char*[] ) >
			parse_opt_func_type;
	// option string - parse function object map type
	typedef	std::map< std::string, parse_opt_func_type >
			parse_opt_map_type;
	// list option function map dictionary.
	parse_opt_map_type	list_option_dic;
	// virtualservice option function map dictionary
	parse_opt_map_type	vs_option_dic;
	// realserver option function map dictionary
	parse_opt_map_type	rs_option_dic;
	// replication option function map dictionary
	parse_opt_map_type	replication_option_dic;
	// replication switch option function map dictionary
	parse_opt_map_type	replication_switch_option_dic;
	// log option function map dictionary
	parse_opt_map_type	log_option_dic;
	// snmp option_function map dictionary
	parse_opt_map_type	snmp_option_dic;
	// parameter option function map dictionary
	parse_opt_map_type	parameter_option_dic;

	// log category string -> log category enum convert map type.
	typedef	std::map< std::string, LOG_CATEGORY_TAG >	string_logcategory_map_type;
	// log category string to log category enum dictionary.
	string_logcategory_map_type	string_logcategory_dic;
	// snmp log category string to snmp log category enum dictionary.
	string_logcategory_map_type	string_snmp_logcategory_dic;
	// log level string -> log level enum convert map type
	typedef	std::map< std::string, LOG_LEVEL_TAG >		string_loglevel_map_type;
	// log level string to log level enum dictionary
	string_loglevel_map_type	string_loglevel_dic;
	// parameter category string -> parameter category enum convert map type
	typedef	std::map< std::string, PARAMETER_COMPONENT_TAG >	string_parameter_map_type;
	string_parameter_map_type	string_parameter_dic;

	// usage function
	std::string	usage();

	//
	// l7vsd request data
	l7vsadm_request	request;
	l7vsd_response	response;

	//
	// l7vsadm using datas.
	bool					numeric_flag;	//! numeric flag
	boost::asio::io_service	io_service;		//! io_service

	error_code	err;
	bool	help_mode;

public:
	// constractor
	l7vsadm();

	//! execute function 
	bool	execute( int, char*[] );
};

}	//namespace l7vs


#endif	//L7VSADM_H
