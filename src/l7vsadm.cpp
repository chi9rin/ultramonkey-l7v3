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

	//
	// command dictionary initialize
	//
	
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

	//
	// list option dictionary itinialize
	//
	option_list_dic["-n"]
		= boost::bind( &l7vsadm::option_list_numeric_parse, this, _2, _3 );
	option_list_dic["--numeric"]
		= boost::bind( &l7vsadm::option_list_numeric_parse, this, _2, _3 );

	//
	// vs option dictionary initialize
	//
	option_vs_dic["-s"]
			= boost::bind( &l7vsadm::option_vs_scheduler_parse, this, _2, _3 );
	option_vs_dic["--scheduler"]
			= boost::bind( &l7vsadm::option_vs_scheduler_parse, this, _2. _3 );
	option_vs_dic["-u"]
			= boost::bind( &l7vsadm::option_vs_upper_parse, this, _2, _3 );
	option_vs_dic["--upper"]
			= boost::bind( &l7vsadm::option_vs_upper_parse, this, _2, _3 );
	option_vs_dic["-b"]
			= boost::bind( &l7vsadm::option_vs_bypass_parse, this, _2, _3 );
	option_vs_dic["--bypass"]
			= boost::bind( &l7vsadm::option_vs_bypass_parse, this, _2, _3 );
	option_vs_dic["-f"]
			= boost::bind( &l7vsadm::option_vs_flag_parse, this, _2, _3 );
	option_vs_dic["--flag"]
			= boost::bind( &l7vsadm::option_vs_flag_parse, this, _2, _3 );
	option_vs_dic["-Q"]
			= boost::bind( &l7vsadm::option_vs_qos_service_parse, this, _2, _3 );
	option_vs_dic["--qos-up"]
			= boost::bind( &l7vsadm::option_vs_qos_service_parse, this, _2, _3 );
	option_vs_dic["-q"]
			= boost::bind( &l7vsadm::option_vs_qos_client_parse, this, _2, _3 );
	option_vs_dic["--qos-down"]
			= boost::bind( &l7vsadm::option_vs_qos_client_parse, this, _2, _3 );

	//
	// rs option dictionary initialize
	//
	option_rs_dic["-w"]
			= boost::bind( &l7vsadm::option_rs_weight, this, _2, _3 );
	option_rs_dic["--weight"]
			= boost::bind( &l7vsadm::option_rs_weight, this, _2, _3 );
	
	//
	// replication dictionary initialize
	//
	option_replication_dic["-s"]
			= boost::bind( &l7vsadm::option_switch, this, _2, _3 );
	option_replication_dic["--switch"]
			= boost::bind( &l7vsadm::option_switch, this, _2, _3 );
	option_replication_dic["-f"]
			= boost::bind( &l7vsadm::option_force, this, _2, _3 );
	option_replication_dic["--force"]
			= boost::bind( &l7vsadm::option_force, this, _2, _3 );
	option_replication_dic["-d"]
			= boost::bind( &l7vsadm::option_dump, this, _2, _3 );
	option_replication_dic["--dump"]
			= boost::bind( &l7vsadm::option_dump, this, _2, _3 );

	//
	// log dictionary initialize
	//
	option_log_dic["-c"]
			= boost::bind( &l7vsadm::option_log_category, this, _2, _3 );
	option_log_dic["--category"]
			= boost::bind( &l7vsadm::option_log_category, this, _2, _3 );
	option_log_dic["-l"]
			= boost::bind( &l7vsadm::option_log_level, this, _2, _3 );
	option_log_dic["--level"]
			= boost::bind( &l7vsadm::option_log_level, this, _2, _3 );

	//
	// snmp dictionary initialize
	//
	option_snmp_dic["-c"]
			= boost::bind( &l7vsadm::option_snmp_category, this, _2, _3 );
	option_snmp_dic["--category"]
			= boost::bind( &l7vsadm::option_snmp_category, this, _2, _3 );
	option_snmp_dic["-l"]
			= boost::bind( &l7vsadm::option_snmp_level, this, _2, _3 );
	option_snmp_dic["--level"]
			= boost::bind( &l7vsadm::option_snmp_level, this, _2, _3 );

	//
	// parameter dictionary initialize
	//
	option_parameter_dic["-r"]
			= boost::bind( &l7vsadm::option_parameter_reload, this, _2, _3 );
	option_parameter_dic["--reload"]
			= boost::bind( &l7vsadm::option_parameter_reload. this, _2, _3 );
	
}

//
// list command parse function
//
bool	l7vsadm::cmd_list_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd,
								 int argc ,
								 char* argv[] ){
	request.command = cmd;
	for( int i = 2 ; i < argc; ++i ){
		option_func_map_type::iterator itr = option_list_dic.find( argv[i] );
		if( itr == option_list_dic.end() ){
			// don't found std::cout << "don't found " << argv[1] << "option";
			return false;
		}
		if( !itr( i, argc, argv ) ) return false;
	}

	//checker is NULL
	return true;
}

//
// virtual service command parse function
//
void	l7vsadm::cmd_vs_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd,
							   int& argc,
							   char* argv[]){
	request.command = cmd;
	for( int i = 2; i < argc; ++i ){
		option_func_map_type::iterator itr = option_vs_dic.find( argv[i] );
		if( itr == option_vs_dic.end() ){
			//don't found std::cout << ""don't found " << argv[i] << " option";
			return false;
		}
		if( !itr( i, argc, argv ) ) return false;
	}

	//checker
	if( request.udpmode )
		//not recv endpoint initialize
		if( request.udp_recv_endpoint == boost::asio::ip::udp::endpoint() ) return false;
	else
		if( request.tcp_recv_endpoint == boost::asio::ip::tcp::endpoint() ) return false;
	//not protocol_module_name initialize
	if( request.protocol_module_name == "" ) return false;
	//no schedule module name initialize
	if( request.schedule_module_name == "" ) return false;

	return true;
}

//
// realserver command parse function.
//
void	l7vsadm::cmd_rs_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd,
							   int argc,
							   const char* argv[]){
	request.command = cmd;
	for( int i = 2; i < argc; ++i ){
		option_func_map_type::iterator itr = option_rs_dic.find( argv[i] );
		if( itr == option_vs_dic.end() ){
			//don't found std::cout << "don't found " << argv[i] << " option";
			return false;
		}
		if( !itr( i, argc, argv ) return false;
	}

	//checker
	if( request.udpmode )
		//not recv endpoint initialize
		if( request.udp_recv_endpoint == boost::asio::ip::udp::endpoint() ) return false;
	else
		if( request.tcp_recv_endpoint == boost::asio::ip::tcp::endpoint() ) return false;
	//not protocol_module_name initialize
	if( request.protocol_module_name == "" ) return false;
	//no schedule module name initialize
	if( request.schedule_module_name == "" ) return false;
	//realserver element check
	if( request.realserver_vector.size() == 0 ) return false;
	for( 	std::vector<realserver_element>::iterator itr = realserver_vector.begin();
			itr != itr.end();
			++itr ){
			if( request.udpmode ){
				if( itr->udp_endpoint == boost::asio::ip::udp::endpoint() ) return false;
			}
			else{
				if( itr->tcp_endpoint == boost::asio::ip::tcp::endpoint() ) return false;
			}
	}
	return true;
}

//
// replication command parse function
//
void	l7vsadm::cmd_replicaton_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd,
									   int argc,
									   char* argv[] ){
   request.command = cmd;
   for( int i = 2; i < argc; ++i ){
	   option_func_map_type::iterator itr = option_replication_dic.find( argv[i] );
	   if( itr == option_replication_dic.end() ){
		   //std::cout << "don't found replication option : " << argv[i];
		   return false;
	   }
	   if( !itr( i, argc, argv ) ) return false;
   }
   //check is NULL.
}

//
// logger command parse function
//
void	l7vsadm::cmd_log_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd,
								int argc,
								char* argv[] ){
	request.command = cmd;
	for( int i = 2; i < argc; ++ ){
		option_func_map_type::iterator itr = option_log_dic.find( argv[1] );
		if( itr == option_log_dic.end() ){
			//std::cout << "don't find logger option : " << argv[i];
			return false;
		}
		if( !itr( i, argc, argv ) ) return false;
	}
	
	//checker
	if( !request.category_all_flag ){
		if( request.log_category == LOG_CAT_NONE ) return false;
		if( request.loglevel == LOG_LV_NONE ) return false;
	}
	else{
		if( request.loglevel == LOG_LV_NONE ) return false;
	}

	return true;
}

//
// snmp command parse function
//
void	l7vsadm::cmd_snmp_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd,
								 int argc,
								 char* argv[] ){
	request.command = cmd;
	for( int i = 2; i < argc; ++i ){
		option_func_map_type::iterator itr = option_snmp_dic.find( argv[i] );
		if( itr == option_snmp_dic.end() ){
			//std::cout << "don't find logger option : " << argv[i];
			return false;
		}
		if( !itr( i, argc, argv ) ) return false;
	}

	//checker
	if( request.snmp_log_category == LOG_CAT_NONE ) return false;
	if( request.snmp_log_level == LOG_LV_NONE ) return false;

	return false;
}

//
// parameter parse function
//
void	l7vsadm::cmd_paramter_parse( const l7vsadm_request::COMMAND_CODE_TAG cmd,
									 int argc,
									 char* argv[] ){
	request.command = cmd;
	for( int i = 2; i < argc; ++i ){
		option_func_map_type::iterator itr = option_replication_dic.find( argv[i] );
		if( itr == option_replication_dic.end() ){
			//std::cout << "don't find replication option : " << argv[i]
			return false;
		}
		if( !itr( i, argc, argv ) ) return false;
	}

	//checker
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
