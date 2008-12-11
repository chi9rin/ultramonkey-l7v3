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
#include <cstdio.h>
#include <cstdlib.h>
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
	std::map<std::string, boost::function<void (l7vsadm_request::COMMAND_CODE_TAG, const int, const char*[] )> >	command_dic;
	std::set
	
	boost::array<char, MAXBUFFER_SIZE>	send_data;
	boost::array<char, MAXBUFFER_SIZE>	send_data;
	l7vsadm_request						request;
	l7vsadm_response						response;
	void	cmd_list_parse( const l7vsadm_request::COMMAND_CODE_TAG, const int, const char*[] );
	void	cmd_vs_parse( const l7vsadm_request::COMMAND_CODE_TAG, const int, const char*[] );
	void	cmd_rs_parse( const l7vsadm_request::COMMAND_CODE_TAG, const int, const char*[] );
	void	cmd_replicaton_parse( const l7vsadm_request::COMMAND_CODE_TAG, const int, const char*[] );
	void	cmd_log_parse( const l7vsadm_request::COMMAND_CODE_TAG, const int, const char*[] );
	void	cmd_snmp_parse( const l7vsadm_request::COMMAND_CODE_TAG, const int, const char*[] );
	void	cmd_paramter_parse( const l7vsadm_request::COMMAND_CODE_TAG, const int, const char*[] );
	void	cmd_help_parse( const l7vsadm_request::COMMAND_CODE_TAG, const int ,const char*[] );

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
