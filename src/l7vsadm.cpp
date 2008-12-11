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

#include "l7vsadm.h"

namespace l7vsd{


//
// l7vsadm class constractor.
//
l7vsadm::l7vsadm(){
		command_dic["-l"]
			= boost::bind( &l7vsadm::cmd_list_parse, this, l7vsadm_request::CMD_LIST, _2, _3 );
		command_dic["--list"]
			= boost::bind( &l7vsadm::cmd_list_parse, this, l7vsadm_request::CMD_LIST, _2, _3 );
		command_dic["-V"]
			= boost::bind( &l7vsadm::cmd_list_parse, this, l7vsadm_request::CMD_LIST_VERBOSE, _2, _3 );
		command_dic["--list-verbose"]
			= boost::bind( &l7vsadm::cmd_list_parse, this, l7vsadm_request::CMD_LIST_VERBOSE, _2, _3 );
		command_dic["-K"]
			= boost::bind( &l7vsadm::cmd_list_parse, this, l7vsadm_request::CMD_LIST_KEY, _2, _3 );
		command_dic["--list-ley"]
			= boost::bind( &l7vsadm::cmd_list_parse, this, l7vsadm_request::CMD_LIST_KEY, _2, _3 );
		command_dic["-A"]
			= boost::bind( &l7vsadm::cmd_vs_parse, this, l7vsadm_request::CMD_ADD_VS, _2, _3 );
		command_dic["--add-virtualservice"]
			= boost::bind( &l7vsadm::cmd_vs_parse, this, l7vsadm_request::CMD_ADD_VS, _2, _3 );
		command_dic["-D"]
			= boost::bind( &l7vsadm::cmd_vs_parse, this, l7vsadm_request::CMD_DEL_VS, _2, _3 );
		command_dic["--del-virtualservice"]
			= boost::bind( &l7vsadm::cmd_vs_parse, this, l7vsadm_request::CMD_DEL_VS, _2, _3 );
		command_dic["-E"]
			= boost::bind( &l7vsadm::cmd_vs_parse, this, l7vsadm_request::CMD_EDIT_VS, _2, _3 );
		command_dic["--edit-virutalservice"]
			= boost::bind( &l7vsadm::cmd_vs_parse, this, l7vsadm_request::CMD_EDIT_VS, _2, _3 );
		command_dic["-C"]
			= boost::bind( &l7vsadm::cmd_vs_parse, this, l7vsadm_request::CMD_FLUSH_VS, _2, _3 );
		command_dic["--flash-virtualservice"]
			= boost::bind( &l7vsadm::cmd_vs_parse, this, l7vsadm_request::CMD_FLUSH_VS, _2, _3 );
		command_dic["-a"]
			= boost::bind( &l7vsadm::cmd_rs_parse, this, l7vsadm_request::CMD_ADD_RS, _2, _3 );
		command_dic["--add-realserver"]
			= boost::bind( &l7vsadm::cmd_rs_parse, this, l7vsadm_request::CMD_ADD_RS, _2, _3 );
		command_dic["-d"]
			= boost::bind( &l7vsadm::cmd_rs_parse, this, l7vsadm_request::CMD_DEL_RS, _2, _3 );
		command_dic["--delete-realserver"]
			= boost::bind( &l7vsadm::cmd_rs_parse, this, l7vsadm_request::CMD_DEL_RS, _2, _3 );
		command_dic["-e"]
			= boost::bind( &l7vsadm::cmd_rs_parse, this, l7vsadm_request::CMD_EDIT_RS, _2, _3 );
		command_dic["--edit-realserver"]
			= boost::bind( &l7vsadm::cmd_rs_parse, this, l7vsadm_request::CMD_EDIT_RS, _2, _3 );
		command_dic["-R"]
			= boost::bind( &l7vsadm::cmd_replicaton_parse, this, l7vsadm_request::CMD_REPLICATION, _2, _3 );
		command_dic["--replication"]
			= boost::bind( &l7vsadm::cmd_replicaton_parse, this, l7vsadm_request::CMD_REPLICATION, _2, _3 );
		command_dic["-L"]
			= boost::bind( &l7vsadm::cmd_log_parse, this, l7vsadm_request::CMD_LOG, _2 , _3 );
		command_dic["--logger"]
			= boost::bind( &l7vsadm::cmd_log_parse, this, l7vsadm_request::CMD_LOG, _2 , _3 );
		command_dic["-S"]
			= boost::bind( &l7vsadm::cmd_snmp_parse, this, l7vsadm_request::CMD_SNMP, _2, _3 );
		command_dic["--snmp"]
			= boost::bind( &l7vsadm::cmd_snmp_parse, this, l7vsadm_request::CMD_SNMP, _2, _3 );
		command_dic["-P"]
			= boost::bind( &l7vsadm::cmd_paramter_parse, this, l7vsadm_request::CMD_PARAMETER, _2, _3 );
		command_dic["--parameter"]
			= boost::bind( &l7vsadm::cmd_paramter_parse, this, l7vsadm_request::CMD_PARAMETER, _2, _3 );
		command_dic["-h"]
			= boost::bind( &l7vsadm::cmd_help_parse, this, l7vsadm_request::CMD_HELP, _2, _3 );
		command_dic["--help"]
			= boost::bind( &l7vsadm::cmd_help_parse, this, l7vsadm_request::CMD_HELP, _2, _3 );
}

//
//
//
void	l7vsadm::cmd_list_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd, const int argc , const char* argv[] ){
	

}

//
//
//
void	l7vsadm::cmd_vs_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd, const int argc, const char* argv[]){
}

//
//
//
void	l7vsadm::cmd_rs_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd, const int argc, const char* argv[]){
}

//
//
//
void	l7vsadm::cmd_replicaton_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd, const int argc, const char* argv[] ){
}

//
//
//
void	l7vsadm::cmd_log_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd, const int argc, const char* argv[] ){
}

//
//
//
void	l7vsadm::cmd_snmp_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd, const int argc, const char* argv[] ){
}

//
//
//
void	l7vsadm::cmd_paramter_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd, const int argc, const char* argv[] ){
}

//
//
//
void	l7vsadm::cmd_help_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd, const int argc, const char* argv[] ){
}


void	l7vsadm::usage( FILE *p ){
	std::fprintf( p,
	"Usage: \n"
	"  l7vsadm -A|E -t service-address -m proto-module [module-args]\n"
	"          [-s scheduler] [-u connection-count] [-b sorry-server]\n"
	"          [-f sorry-flag] [-Q QoSval-up] [-q QoSval-down]\n"
	"  l7vsadm -D -t service-address -m proto-module [module-args]\n"
	"  l7vsadm -C\n"
	"  l7vsadm -a|e -t service-address -m proto-module [module-args]\n"
	"          -r server-address [-w weight]\n"
	"  l7vsadm -d -t service-address -m proto-module [module-args]\n"
	"          -r server-address\n"
	"  l7vsadm -R -s replication-switch\n"
	"  l7vsadm -R -f\n"
	"  l7vsadm -R -d\n"
	"  l7vsadm -L -c log-category -l log-level\n"
	"  l7vsadm -S -c log-category -l log-level\n"
	"  l7vsadm -P -r reload-parameter\n"
	"  l7vsadm -l [-n]\n"
	"  l7vsadm -V [-n]\n"
	"  l7vsadm -K [-n]\n"
	"  l7vsadm -h\n"
	"\n");

}

};// namespace l7vsd


#ifndef	TEST_CASE
int main( int argc, char* argv[] ){
	try{

	}
	catch( ... ){

	}
	return 0;
}
#else
int main(){ return 0;}
#endif	//TEST_CASE

}//namespace l7vsd
