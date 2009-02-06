//
//
//
//
// l7vsadm.cpp
// ^^^^^^^^^^^
//	l7vsadm command is l7vsd control application
//
//	Copyrigh (c) Norihisa Nakai ( n dot nakai at sdy dot co dot jp )
//
//	Distributed under the Boost Software License. Version 1.0
//	See http://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <sstream>
#include <exception>
#include <boost/bind.hpp>
#include "l7vsadm.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"

#ifndef	L7VS_MODULE_PATH
#define	L7VS_MODULE_PATH	"./"
#endif

//
// command functions.
//
//! list command parsing.
bool	l7vs::l7vsadm::parse_list_func(	l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;	// set response
	if( argc < 4 ) return true;	// optin is none. this pattern is true
	for( int pos = 3; pos < argc; ++pos ){ 	//sarch option function from argv strings
		parse_opt_map_type::iterator itr = list_option_dic.find( argv[pos] );
		if( itr != list_option_dic.end() ){	// option strign function find.
			if( ! itr->second( pos, argc, argv ) ) return false;	// option string function error.
		}
		else{	//option string function don't find.
			// print option not found message.
			return false;
		}
	}
	return true;
}
//
// option list functions.
//
//! list numeric flag check.
bool	l7vs::l7vsadm::parse_opt_list_numeric_func( int& pos, int argc, char* argv[] ){
	numeric_flag = true;	//numeric flag on.
	return true;
}
//! virtualservice command parsing.
bool	l7vs::l7vsadm::parse_vs_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;	// set command
	for( int pos = 3; pos < argc; ++pos ){	// check options.
		parse_opt_map_type::iterator itr = vs_option_dic.find( argv[pos] );
		if( itr != vs_option_dic.end() ){	// find option
			if( ! itr->second( pos, argc, argv ) ) return false;	// option function execute.
		}
		else{	// don't find option function.
			return false;
		}
	}
	// check vertualservice on response
	// 
	if( request.vs_element.schedule_module_name.length() == 0 ){
		//scheduker name error
		return false;
	}
	if( request.vs_element.protocol_module_name.length() == 0 ){
		//module name error
		return false;
	}
	if( request.vs_element.udpmode ){
		if( request.vs_element.udp_recv_endpoint == boost::asio::ip::udp::endpoint() ){
			// udp mode,but not acceptor endpoint
			return false;
		}
	}
	else{
		if( request.vs_element.tcp_accept_endpoint == boost::asio::ip::tcp::endpoint() ){
			// tcp mode, but not acceptor endpoint
			return false;
		}
	}
	return true;
}
//
// option virtualservice functions.
//
//! target option check
bool	l7vs::l7vsadm::parse_opt_vs_target_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		//don't rarget recvaddress:port
		return false;
	}
	// get host endpoint from string
	std::string	src_str = argv[pos];
	try{
		if( request.vs_element.udpmode ){
			request.vs_element.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( src_str );
		}
		else{
			request.vs_element.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( src_str );
		}
	}
	catch( boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> > & err ){
		return false;
	}
	return true;
}
//! module option check
bool	l7vs::l7vsadm::parse_opt_vs_module_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		//don't rarget shcedule name.
		return false;
	}
	std::string	module_name = argv[pos];
	protocol_module_control&	ctrl = protocol_module_control::getInstance();
	ctrl.initialize( L7VS_MODULE_PATH );
	protocol_module_base* module;
	try{
		module = ctrl.load_module( module_name );
	}
	catch( ... ){
		return false;
	}
	if( !module ){
		//don't find protocol module.
		return false;
	}
	// create module args.
	std::vector< std::string > module_args;
	while( true ){
		if( ++pos == argc ) break; //module option end.
		parse_opt_map_type::iterator itr = vs_option_dic.find( argv[pos] );
		if( itr != vs_option_dic.end() ) break;	// module option end.

		module_args.push_back( argv[pos] );
	}
	protocol_module_base::check_message_result module_message = module->check_parameter( module_args );
	if( !module_message.flag ){
		// args is not suppoted.
	}
	request.vs_element.protocol_module_name = module_name;
	BOOST_FOREACH( std::string str,	module_args ){
		request.vs_element.protocol_args.push_back( str );
	}
	ctrl.unload_module( module );


	return true;
}

//! scheduler option check.
bool	l7vs::l7vsadm::parse_opt_vs_scheduler_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't target scheduler name.
		return false;
	}
	//schedule module check.
	std::string	scheduler_name = argv[pos];
	schedule_module_control&	ctrl = schedule_module_control::getInstance();
	ctrl.initialize( L7VS_MODULE_PATH );
	schedule_module_base* module;
	try{
		module = ctrl.load_module( scheduler_name );
	}
	catch( ... ){
		return false;
	}
	if( !module ){
		// don't find schedule module 
		return false;
	}
	ctrl.unload_module( module );
	request.vs_element.schedule_module_name = scheduler_name;
	return true;
}
//! upper flag check
bool	l7vs::l7vsadm::parse_opt_vs_upper_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't target maxconnection_num
		return false;
	}
	try{
		request.vs_element.sorry_maxconnection = boost::lexical_cast< long long >( argv[pos] );
	}
	catch( boost::bad_lexical_cast& e ){
		// don't convert argv[pos] is 
	}
	//check connection limit and zero
	return true;
}
//! bypass(SorryServer) option check
bool	l7vs::l7vsadm::parse_opt_vs_bypass_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		//don't rarget sorryserver:port
		return false;
	}
	try{
		std::string sorry_endpoint = argv[pos];
		request.vs_element.sorry_endpoint = string_to_endpoint< boost::asio::ip::tcp > ( sorry_endpoint );
	}
	catch( boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> > & err ){
		//don't resolve sorryserver endpoint
		return false;
	}
	return true;	//
}
//! virtualservice option flag function
bool	l7vs::l7vsadm::parse_opt_vs_flag_func( int& pos, int argc, char* argv[] ){
	request.vs_element.sorry_flag = true;
	return true;
}
//! virtualservice option qosupstream function
bool	l7vs::l7vsadm::parse_opt_vs_qosup_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		//don't rarget QoS upstream value.
		return false;
	}
	try{
		virtualservice_element& elem = request.vs_element;	// request virtualservice element refalence get.
		elem.qos_upstream = boost::lexical_cast< unsigned long long > ( argv[pos] );	// set qos_upstream
	}
	catch( boost::bad_lexical_cast& ex ){	// don't convert string to qos_upsatream
		// don't conv qos upstream
		return false;
	}
	return true;		
}
//! virtualservice option qosdownstream functipn
bool	l7vs::l7vsadm::parse_opt_vs_qosdown_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't target QoS downstream value
		return false;
	}
	try{
		virtualservice_element& elem = request.vs_element;	// request virtualservice element refarence get.
		elem.qos_downstream = boost::lexical_cast< unsigned long long > ( argv[pos] );
	}
	catch( boost::bad_lexical_cast& ex ){
		// don' conv qos downstream
		return false;
	}
	return true;
}
//! virtualservice option udp func.
bool	l7vs::l7vsadm::parse_opt_vs_udp_func( int& pos, int argc, char* argv[] ){
	virtualservice_element& elem = request.vs_element;	// request virtualservie element reference get.
	elem.udpmode = true;	// udpmode on.
	boost::asio::ip::tcp::endpoint	zeropoint;
	if( zeropoint != elem.tcp_accept_endpoint ){ // adddress tcp_acceptor endpoint
		std::stringstream	sstream;
		sstream	<< elem.tcp_accept_endpoint;
		try{
			std::string	endpoint	= sstream.str();
			elem.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( endpoint );
			elem.tcp_accept_endpoint = zeropoint;
		}
		catch( boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> >& e ){
			return false;
		}
	}
	if( elem.realserver_vector.size() != 0 && elem.realserver_vector.front().tcp_endpoint != zeropoint ){
		std::stringstream 	sstream;
		sstream << elem.realserver_vector.front().tcp_endpoint;
		try{
			std::string	endpoint = sstream.str();
			elem.realserver_vector.front().udp_endpoint = string_to_endpoint< boost::asio::ip::udp > ( endpoint );
		}
		catch( boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> >& e ){
			return false;
		}
	}
	return true;
}

//! realserver command parsing.
bool	l7vs::l7vsadm::parse_rs_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	request.vs_element.realserver_vector.push_back( realserver_element() );
	for( int pos = 3; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = rs_option_dic.find( argv[pos] );
		if( itr != rs_option_dic.end() ){
			if( ! itr->second( pos, argc, argv ) ) return false;
		}
		else{
			return false;
		}
	}
	return true;
}
//
// realserver option functions.
//
//! realserver target set

//! weight is set 
bool	l7vs::l7vsadm::parse_opt_rs_weight_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		//don't target weight value
		return false;
	}
	try{
		request.vs_element.realserver_vector.front().weight = boost::lexical_cast<int>( argv[pos] );
	}
	catch( boost::bad_lexical_cast& ex ){
		// lexical cast error
		return false;
	}
	return true;
}
//! realserver target set
bool	l7vs::l7vsadm::parse_opt_rs_realserver_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't rarget realserver address
		return false;
	}
	try{
		if( request.vs_element.udpmode ){
			std::string	endpoint = argv[pos];
			request.vs_element.realserver_vector.front().udp_endpoint = string_to_endpoint< boost::asio::ip::udp >( endpoint );
		}
		else{
			std::string	endpoint = argv[pos];
			request.vs_element.realserver_vector.front().tcp_endpoint = string_to_endpoint< boost::asio::ip::tcp >( endpoint );
		}
	}
	catch( ... ){
		// address string error.
		return false;
	}
	return true;
}
	
//! replication command parsing.
bool	l7vs::l7vsadm::parse_replication_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	for( int pos = 3; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = replication_option_dic.find( argv[pos] );
		if( ! itr->second( pos, argc, argv ) ) return false;
	}
	return true;
}

//
//	replication option functions.
//
//! start function
bool	l7vs::l7vsadm::parse_opt_replication_start_func( int& pos, int argc, char* argv[] ){
	if( request.replication_command != l7vsadm_request::REP_NONE ){ 
		// double command target.
		return false;
	}
	request.replication_command = l7vsadm_request::REP_START;
	return true;
}
//! stop function
bool	l7vs::l7vsadm::parse_opt_replication_stop_func( int& pos, int argc, char* argv[] ){
	if( request.replication_command != l7vsadm_request::REP_NONE ){ 
		// double command target.
		return false;
	}
	request.replication_command = l7vsadm_request::REP_STOP;
	return true;
}
//! force function
bool	l7vs::l7vsadm::parse_opt_replication_force_func( int& pos, int argc, char* argv[] ){
	if( request.replication_command != l7vsadm_request::REP_NONE ){ 
		// double command target.
		return false;
	}
	request.replication_command = l7vsadm_request::REP_FORCE;
	return true;
}
//! dump function
bool	l7vs::l7vsadm::parse_opt_replication_dump_func( int& pos, int argc, char* argv[] ){
	if( request.replication_command != l7vsadm_request::REP_NONE ){ 
		// double command target.
		return false;
	}
	request.replication_command = l7vsadm_request::REP_DUMP;
	return true;
}

//! log command parsing.
bool	l7vs::l7vsadm::parse_log_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	for( int pos = 3; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = log_option_dic.find( argv[pos] );
		if( ! itr->second( pos, argc, argv ) ) return false;
	}
	return true;
}
//
//	log option function
//
//! log category set function
bool	l7vs::l7vsadm::parse_opt_log_category_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't target logcategory
		return false;
	}
	if( request.log_category != LOG_CAT_NONE ){
		// double target commands.
		return false;
	}
	string_logcategory_map_type::iterator itr = string_logcategory_dic.find( argv[pos] );
	if( itr != string_logcategory_dic.end() ){
		request.log_category = itr->second;
		return true;
	}
	return false;
}
//! log level set function
bool	l7vs::l7vsadm::parse_opt_log_level_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't rarget logcategory
		return false;
	}
	if( request.log_level != LOG_LV_NONE ){
		// double target commands.
		return false;
	}
	string_loglevel_map_type::iterator itr = string_loglevel_dic.find( argv[pos] );
	if( itr != string_loglevel_dic.end() ){
		request.log_level = itr->second;
		return false;
	}
	return true;
}

//! snmp command parsing
bool	l7vs::l7vsadm::parse_snmp_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	for( int pos = 3; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = log_option_dic.find( argv[pos] );
		if( !itr->second( pos, argc, argv ) ) return false;
	}
	return true;
}
//! snmp log category set function
bool	l7vs::l7vsadm::parse_opt_snmp_log_category_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't target logcategory
		return false;
	}
	if( request.snmp_log_category != LOG_CAT_NONE ){
		// double target commands.
		return false;
	}
	string_logcategory_map_type::iterator itr = string_snmp_logcategory_dic.find( argv[pos] );
	if( itr != string_snmp_logcategory_dic.end() ){
		request.snmp_log_category = itr->second;
		return true;
	}
	return false;
}
//! log level set function
bool	l7vs::l7vsadm::parse_opt_snmp_log_level_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't rarget logcategory
		return false;
	}
	if( request.snmp_log_level != LOG_LV_NONE ){
		// double target commands.
		return false;
	}
	string_loglevel_map_type::iterator itr = string_loglevel_dic.find( argv[pos] );
	if( itr != string_loglevel_dic.end() ){
		request.snmp_log_level = itr->second;
		return false;
	}
	return true;
}

//! parameter command parsing
bool	l7vs::l7vsadm::parse_parameter_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	for( int pos = 3; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = parameter_option_dic.find( argv[pos] );
		if( !itr->second( pos, argc, argv ) ) return false;
	}
	return true;
}
//
//	parameter command 
//
bool	l7vs::l7vsadm::parse_opt_parameter_reload_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't rarget logcategory
		return false;
	}
	string_parameter_map_type::iterator itr = string_parameter_dic.find( argv[pos] );
	if( itr != string_parameter_dic.end() ){
		request.reload_param = itr->second;
	}
	return false;	
}

//! help command parsing
bool	l7vs::l7vsadm::parse_help_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){

	std::cout << usage() << std::endl;

	std::cout <<
	"Commands:\n"
	"  --add-service     -A        add virtual service with options\n"
	"  --edit-service    -E        edit virtual service with options\n"
	"  --delete-service  -D        delete virtual service with options\n"
	"  --flush           -C        flush virtual service\n"
	"  --add-server      -a        add real server with options\n"
	"  --edit-server     -e        edit real server with options\n"
	"  --delete-server   -d        delete real server with options\n"
	"  --replication     -R        control replication-function\n"
	"  --log             -L        control logger-function\n"
	"  --snmp            -S        control SNMP Agent-function\n"
	"  --parameter       -P        control parameter-function\n"
	"  --list            -l        list the table\n"
	"  --verbose         -V        list the table in verbose format\n"
	"  --key             -K        list the table in key setting format\n"
	"  --help            -h        show usage\n"
	<< std::endl;

	std::cout <<
	"Options:\n"
	"  --tcp-service   -t service-address     service-address is host:port\n"
	"  --proto-module  -m proto-module        protocol module name and module argment\n"
	"                     [module-args]\n"
	"  --scheduler     -s scheduler           one of rr,lc,wrr\n"
	"  --upper         -u connection-count    maximum number of connections\n"
	"  --bypass        -b sorry-server        sorry server address is host:port\n"
	"  --flag          -f sorry-flag          sorry status set to virtual service\n"
	"  --qos-up        -Q QoSval-up           QoS Threshold(bps) set to real server direction\n"
	"  --qos-down      -q QoSval-down         QoS Threshold(bps) set to client direction\n"
	"  --udp           -p                     VirtualService UDP mode on\n"
	"  --real-server   -r server-address      server-address is host:port\n"
	"  --weight        -w weight              scheduling weight set to real server\n"
	"  --switch        -s replication-switch  start or stop replication\n"
	"  --force         -f                     force replication start\n"
	"  --dump          -d                     dump replication memory\n"
	"  --category      -c log-category        set log category for l7vsd or SNMP Agent\n"
	"  --level         -l log-level           set log level for l7vsd or SNMP Agent\n"
	"  --reload        -r reload-parameter    reload specified config parameter\n"
	"  --numeric       -n                     list the table in numeric\n"
	<< std::endl;

	return true;
}

//! usage function.
std::string	l7vs::l7vsadm::usage(){
	std::stringstream	stream;
	stream <<
	"Usage: \n"
	"  l7vsadm -A|E -t service-address -m proto-module [module-args]\n"
	"          [-s scheduler] [-u connection-count] [-b sorry-server]\n"
	"          [-f sorry-flag] [-Q QoSval-up] [-q QoSval-down]\n"
    "          [-p ]\n"
	"  l7vsadm -D -t service-address -m proto-module [module-args]\n"
	"  l7vsadm -C\n"
	"  l7vsadm -a|e -t service-address -m proto-module [module-args] [-u]\n"
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
	<< std::endl;
	return stream.str();
}

//
// l7vsadm constractor.
// create including all dictionary.
l7vs::l7vsadm::l7vsadm() : numeric_flag(false){

	// create command dictionary.
	command_dic["-l"]				= boost::bind( &l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST, _1, _2 );
	command_dic["--list"]			= boost::bind( &l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST, _1, _2 );
	command_dic["-V"]				= boost::bind( &l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST_VERBOSE, _1, _2 );
	command_dic["--verbose"]		= boost::bind( &l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST_VERBOSE, _1, _2 );
	command_dic["-K"]				= boost::bind( &l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST_KEY, _1, _2 );
	command_dic["--key"]			= boost::bind( &l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST_KEY, _1, _2 );
	command_dic["-A"] 				= boost::bind( &l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_ADD_VS, _1, _2 );
	command_dic["--add-service"]	= boost::bind( &l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_ADD_VS, _1, _2 );
	command_dic["-D"]				= boost::bind( &l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_DEL_VS, _1, _2 );
	command_dic["--delete-service"]	= boost::bind( &l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_DEL_VS, _1, _2 );
	command_dic["-E"]				= boost::bind( &l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_EDIT_VS, _1, _2 );
	command_dic["--edit-service"]	= boost::bind( &l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_EDIT_VS, _1, _2 );
	command_dic["-C"]				= boost::bind( &l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_FLUSH_VS, _1, _2 );
	command_dic["--flush"]			= boost::bind( &l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_FLUSH_VS, _1, _2 );
	command_dic["-a"]				= boost::bind( &l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_ADD_RS, _1, _2 );
	command_dic["--add-server"]		= boost::bind( &l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_ADD_RS, _1, _2 );
	command_dic["-d"]				= boost::bind( &l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_DEL_RS, _1, _2 );
	command_dic["--delete-server"]	= boost::bind( &l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_DEL_RS, _1, _2 );
	command_dic["-e"]				= boost::bind( &l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_EDIT_RS, _1, _2 );
	command_dic["--edit-server"]	= boost::bind( &l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_EDIT_RS, _1, _2 );
	command_dic["-R"]				= boost::bind( &l7vsadm::parse_replication_func, this, l7vsadm_request::CMD_REPLICATION, _1, _2 );
	command_dic["--replication"]	= boost::bind( &l7vsadm::parse_replication_func, this, l7vsadm_request::CMD_REPLICATION, _1, _2 );
	command_dic["-L"]				= boost::bind( &l7vsadm::parse_log_func, this, l7vsadm_request::CMD_LOG, _1, _2 );
	command_dic["--log"]			= boost::bind( &l7vsadm::parse_log_func, this, l7vsadm_request::CMD_LOG, _1, _2 );
	command_dic["-S"]				= boost::bind( &l7vsadm::parse_snmp_func, this, l7vsadm_request::CMD_SNMP, _1, _2 );
	command_dic["--snmp"]			= boost::bind( &l7vsadm::parse_snmp_func, this, l7vsadm_request::CMD_SNMP, _1, _2 );
	command_dic["-P"]				= boost::bind( &l7vsadm::parse_parameter_func, this, l7vsadm_request::CMD_PARAMETER, _1, _2 );
	command_dic["--parameter"]		= boost::bind( &l7vsadm::parse_parameter_func, this, l7vsadm_request::CMD_PARAMETER, _1, _2 );
	command_dic["-h"]				= boost::bind( &l7vsadm::parse_help_func, this, l7vsadm_request::CMD_HELP, _1, _2 );
	command_dic["--help"]			= boost::bind( &l7vsadm::parse_help_func, this, l7vsadm_request::CMD_HELP, _1, _2 );

	// create list option dictionary.
	list_option_dic["-n"]			= boost::bind( &l7vsadm::parse_opt_list_numeric_func, this, _1, _2, _3 );
	list_option_dic["--numeric"]	= boost::bind( &l7vsadm::parse_opt_list_numeric_func, this, _1, _2, _3 );
	// create virtualservice option dictionary
	vs_option_dic["-t"]				= boost::bind( &l7vsadm::parse_opt_vs_target_func, this, _1, _2, _3 );
	vs_option_dic["--target"]		= boost::bind( &l7vsadm::parse_opt_vs_target_func, this, _1, _2, _3 );
	vs_option_dic["-m"]				= boost::bind( &l7vsadm::parse_opt_vs_module_func, this, _1, _2, _3 );
	vs_option_dic["--module"]		= boost::bind( &l7vsadm::parse_opt_vs_module_func, this, _1, _2, _3 );
	vs_option_dic["-s"]				= boost::bind( &l7vsadm::parse_opt_vs_scheduler_func, this, _1, _2, _3 );
	vs_option_dic["--scheduler"]	= boost::bind( &l7vsadm::parse_opt_vs_scheduler_func, this, _1, _2, _3 );
	vs_option_dic["-u"]				= boost::bind( &l7vsadm::parse_opt_vs_upper_func, this, _1, _2, _3 );
	vs_option_dic["--upper"]		= boost::bind( &l7vsadm::parse_opt_vs_upper_func, this, _1, _2, _3 );
	vs_option_dic["-b"]				= boost::bind( &l7vsadm::parse_opt_vs_bypass_func, this, _1, _2, _3 );
	vs_option_dic["--bypass"]		= boost::bind( &l7vsadm::parse_opt_vs_bypass_func, this, _1, _2, _3 );
	vs_option_dic["-f"]				= boost::bind( &l7vsadm::parse_opt_vs_flag_func, this, _1, _2, _3 );
	vs_option_dic["--flag"]			= boost::bind( &l7vsadm::parse_opt_vs_flag_func, this, _1, _2, _3 );
	vs_option_dic["-Q"]				= boost::bind( &l7vsadm::parse_opt_vs_qosup_func, this, _1, _2, _3 );
	vs_option_dic["--qos-up"]		= boost::bind( &l7vsadm::parse_opt_vs_qosup_func, this, _1, _2, _3 );
	vs_option_dic["-q"]				= boost::bind( &l7vsadm::parse_opt_vs_qosdown_func, this, _1, _2, _3 );
	vs_option_dic["--qos-down"]		= boost::bind( &l7vsadm::parse_opt_vs_qosdown_func, this, _1, _2, _3 );
	vs_option_dic["-p"]				= boost::bind( &l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3 );
	vs_option_dic["--udp"]			= boost::bind( &l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3 );
	// create realserver option dictionary
	rs_option_dic["-t"]				= boost::bind( &l7vsadm::parse_opt_vs_target_func, this, _1, _2, _3 );
	rs_option_dic["--target"]		= boost::bind( &l7vsadm::parse_opt_vs_target_func, this, _1, _2, _3 );
	rs_option_dic["-w"]				= boost::bind( &l7vsadm::parse_opt_rs_weight_func, this, _1, _2, _3 );
	rs_option_dic["--weight"]		= boost::bind( &l7vsadm::parse_opt_rs_weight_func, this, _1, _2, _3 );
	rs_option_dic["-m"]				= boost::bind( &l7vsadm::parse_opt_vs_module_func, this, _1, _2, _3 );
	rs_option_dic["--module"]		= boost::bind( &l7vsadm::parse_opt_vs_module_func, this, _1, _2, _3 );
	vs_option_dic["-p"]				= boost::bind( &l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3 );
	vs_option_dic["--udp"]			= boost::bind( &l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3 );
	rs_option_dic["-r"]				= boost::bind( &l7vsadm::parse_opt_rs_realserver_func, this, _1, _2, _3 );
	rs_option_dic["--real-server"]	= boost::bind( &l7vsadm::parse_opt_rs_realserver_func, this, _1, _2, _3 );
	// create replication option dictionary
	//replication_option_dic["-s"]	= boost::bind( &l7vsadm::parse_opt_replication_switch_func, this, _1, _2, _3 );
	//replication_option_dic["--switch"]
	//								= boost::bind( &l7vsadm::parse_opt_replication_switch_func, this, _1, _2, _3 );
	replication_option_dic["f"]		= boost::bind( &l7vsadm::parse_opt_replication_force_func, this, _1, _2, _3 );
	replication_option_dic["--force"]
									= boost::bind( &l7vsadm::parse_opt_replication_force_func, this, _1, _2, _3 );
	replication_option_dic["-d"]	= boost::bind( &l7vsadm::parse_opt_replication_dump_func, this, _1, _2, _3 );
	replication_option_dic["--dump"]
									= boost::bind( &l7vsadm::parse_opt_replication_dump_func, this, _1, _2, _3 );
	// create log option function dictionary create
	log_option_dic["-c"]			= boost::bind( &l7vsadm::parse_opt_log_category_func, this, _1, _2, _3 );
	log_option_dic["--category"]	= boost::bind( &l7vsadm::parse_opt_log_category_func, this, _1, _2, _3 );
	log_option_dic["-l"]			= boost::bind( &l7vsadm::parse_opt_log_level_func, this, _1, _2, _3 );
	log_option_dic["--level"]		= boost::bind( &l7vsadm::parse_opt_log_level_func, this, _1, _2, _3 );
	// snmp agent option function dictionary create
	snmp_option_dic["-c"]			= boost::bind( &l7vsadm::parse_opt_snmp_log_category_func, this, _1, _2, _3 );
	snmp_option_dic["--category"]	= boost::bind( &l7vsadm::parse_opt_snmp_log_category_func, this, _1, _2, _3 );
	snmp_option_dic["-l"]			= boost::bind( &l7vsadm::parse_opt_snmp_log_level_func, this, _1, _2, _3 );
	snmp_option_dic["--level"]		= boost::bind( &l7vsadm::parse_opt_snmp_log_level_func, this, _1, _2, _3 );
	// parameter option function dictionary create
	parameter_option_dic["-r"]		= boost::bind( &l7vsadm::parse_opt_parameter_reload_func, this, _1, _2, _3 );
	parameter_option_dic["--reload"]
									= boost::bind( &l7vsadm::parse_opt_parameter_reload_func, this, _1, _2, _3 );
	// string logcategory dictionary create
	string_logcategory_dic["l7vsd_network"]	= LOG_CAT_L7VSD_NETWORK;
	string_logcategory_dic["nw"]			= LOG_CAT_L7VSD_NETWORK;
		// also, another log category is not commit.

	// string snmp logcategory dictionary create 
	string_snmp_logcategory_dic["snmpagent_start_stop"]			= LOG_CAT_SNMPAGENT_START_STOP;
	string_snmp_logcategory_dic["snmp_stastp"]					= LOG_CAT_SNMPAGENT_START_STOP;
	string_snmp_logcategory_dic["snmpagent_manager_receive"]	= LOG_CAT_SNMPAGENT_MANAGER_RECEIVE;
	string_snmp_logcategory_dic["snmp_mngrcv"]					= LOG_CAT_SNMPAGENT_MANAGER_RECEIVE;
	string_snmp_logcategory_dic["snmpagent_manager_send"]		= LOG_CAT_SNMPAGENT_MANAGER_SEND;
	string_snmp_logcategory_dic["snmp_mngsnd"]					= LOG_CAT_SNMPAGENT_MANAGER_SEND;
	string_snmp_logcategory_dic["snmpagent_l7vsd_receive"]		= LOG_CAT_SNMPAGENT_L7VSD_RECEIVE;
	string_snmp_logcategory_dic["snmp_vsdrcv"]					= LOG_CAT_SNMPAGENT_L7VSD_RECEIVE;
	string_snmp_logcategory_dic["snmpagent_l7vsd_send"]			= LOG_CAT_SNMPAGENT_L7VSD_SEND;
	string_snmp_logcategory_dic["snmpagent_logger"]				= LOG_CAT_SNMPAGENT_LOGGER;
	string_snmp_logcategory_dic["snmp_logger"]					= LOG_CAT_SNMPAGENT_LOGGER;
	string_snmp_logcategory_dic["snmpagent_parameter"]			= LOG_CAT_SNMPAGENT_PARAMETER;
	string_snmp_logcategory_dic["snmp_para"]					= LOG_CAT_SNMPAGENT_PARAMETER;

	// string log level dictionary create.
	string_loglevel_dic["debug"]	= LOG_LV_DEBUG;
	string_loglevel_dic["info"]		= LOG_LV_INFO;
	string_loglevel_dic["warn"]		= LOG_LV_WARN;
	string_loglevel_dic["error"]	= LOG_LV_ERROR;
	string_loglevel_dic["fatal"]	= LOG_LV_FATAL;

	// parameter category dictionary create
	string_parameter_dic["ALL"]		= PARAM_COMP_ALL;
	string_parameter_dic["all"]		= PARAM_COMP_ALL;
	// other parameter is not yet!
}


bool	l7vs::l7vsadm::execute( int argc, char* argv[] ){
	// no argument, display usage
	if( 1 == argc ){
		std::cout << usage() << std::endl;
		return true;
	}

	// analyze command line
	int pos = 1;
	parse_cmd_map_type::iterator itr = command_dic.find( argv[pos] );
	if( itr != command_dic.end() ){
		itr->second( argc, argv );
	}
	else{
		err.setter( true, "command not found." );
	}

	// display analyze result
	if( err ){

		return false;
	}

	if( help_mode ){
		// display help

		return true;
	}
	else{
		// communicate to l7vsd
	
		// display result
	}
	return true;
}

#ifndef	UNIT_TEST
int main( int argc, char* argv[] ){
	Logger		logger;
	Parameter	param;
	logger.reloadConf();

	l7vsadm	adm;
	if( !adm.execute( argc, argv ) ){
		return -1;
	}
	return 0;
}
#endif	//UNIT_TEST

