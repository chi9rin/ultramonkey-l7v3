//
//	@file	l7vsadm
//	@brief	main module of l7vsadm
//
//	l7vsadm	: virtual server administration program for l7vsd
//	copyright(c) sdy corporation.2008
//	mail: n.nakai at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef	L7VSADM_H
#define	L7VSADM_H

#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include "l7command.h"

#ifndef	MAXBUFFER_SIZE
	#define	MAXBUFFER_SIZE (65535)
#endif

namespace l7vsd{

class	l7vsadm{
protected:

	typedef	std::map< std::string, boost::function< bool (l7vsadm_request::COMMAND_CODE_TAG, int, char*[] )> >
			command_func_map_type;
	typedef	std::map< std::string, boost::function< bool (int&, int, char*[] ) > >
			option_func_map_type;
	
	command_func_map_type	command_dic;
	
	boost::array<char, MAXBUFFER_SIZE>	send_data;
	boost::array<char, MAXBUFFER_SIZE>	recv_data;
	l7vsadm_request						request;
	l7vsadm_response					response;
	
	bool	cmd_list_parse( const l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
		option_func_map_type	option_list_dic;
		bool					option_list_numeric_parse( int&, int, char*[] );

	bool	cmd_vs_parse( const l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
		option_func_map_type	option_vs_dic;
		bool	option_vs_flag_parsr( int&, int, char*[] );
		bool	option_vs_target_parse( int&, int, char*[] );
		bool	option_vs_module_parse( int&, int, char*[] );
		bool	option_vs_scheduler_parse( int&, int, char*[] );
		bool	option_vs_upper_parse( int&, int, char*[] );
		bool	option_vs_bypass_parse( int&, int, char*[] );
		bool	option_vs_qos_service_parse( int&, int, char*[] );
		bool	option_vs_qos_client_parse( int&, int, char*[] );
		bool	option_vs_udp_parse( int&, int, char*[] );

	bool	cmd_rs_parse( const l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
		option_func_map_type	option_rs_dic;
		bool	option_rs_weight( int&, int, char*[] );

	bool	cmd_replicaton_parse( const l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
		option_func_map_type	option_replication_dic;
		bool	option_replication_switch( int&, int, char*[] );
		bool	option_replication_force( int&, int, char*[] );
		bool	option_replication_dump( int&, int, char*[] );

	bool	cmd_log_parse( const l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
		option_func_map_type	option_log_dic;
		bool	option_log_category( int&, int, char*[] );
		bool	option_log_level( int&, int, char*[] );

	bool	cmd_snmp_parse( const l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
		option_func_map_type	option_snmp_dic;
		bool	option_snmp_category( int&, int, char*[] );
		bool	option_snmp_level( int&, int, char*[] );

	bool	cmd_paramter_parse( const l7vsadm_request::COMMAND_CODE_TAG, int, char*[] );
		option_func_map_type	option_parameter_dic;
		bool	option_parameter_reload( int&, int, char*[] );

	bool	cmd_help_parse( const l7vsadm_request::COMMAND_CODE_TAG, int ,char*[] );

	bool	request_send();
	bool	response_recv();
	void	usage(FILE* fp);
	bool	pase_command( int argc, char* argv[] );
public:
	l7vsadm();
	~l7vsadm();
};

};//namespace l7vsd
	
#endif	//L7VSADM_H
