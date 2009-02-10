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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/format.hpp>
#include "l7vsadm.h"
#include "logger.h"
#include "parameter.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"

//
// command functions.
//
//! list command parsing.
bool	l7vs::l7vsadm::parse_list_func(	l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;	// set command
	if( argc < 3 ) return true;	// option is none. this pattern is true
	for( int pos = 2; pos < argc; ++pos ){ 	//search option function from argv strings
		parse_opt_map_type::iterator itr = list_option_dic.find( argv[pos] );
		if( itr != list_option_dic.end() ){	// option string function find.
			if( ! itr->second( pos, argc, argv ) ) return false;	// option string function error.
		}
		else{	//option string function don't find.
			// print option not found message.
			std::stringstream buf;
			buf << "list option not found:" << argv[pos];
			err.setter( true, buf.str() );
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
	for( int pos = 2; pos < argc; ++pos ){	// check options.
		parse_opt_map_type::iterator itr = vs_option_dic.find( argv[pos] );
		if( itr != vs_option_dic.end() ){	// find option
			if( ! itr->second( pos, argc, argv ) ) return false;	// option function execute.
		}
		else{	// don't find option function.
			std::stringstream buf;
			buf << "virtualservice option not found:" << argv[pos];
			err.setter( true, buf.str() );
			return false;
		}
	}
	// check virtualservice on response
	//
	if( l7vsadm_request::CMD_FLUSH_VS == cmd ){
		// flushvs required no option
		return true;
	}
	if( ( l7vsadm_request::CMD_ADD_VS == cmd ) && ( request.vs_element.schedule_module_name.length() == 0 ) ){
		//scheduler module not specified
		//scheduler module check.
		std::string	scheduler_name = "rr";		//default scheduler
		schedule_module_control&	ctrl = schedule_module_control::getInstance();
		ctrl.initialize( L7VS_MODULE_PATH );
		schedule_module_base* module;
		try{
			module = ctrl.load_module( scheduler_name );
		}
		catch( ... ){
			std::stringstream buf;
			buf << "scheduler module load error:" << scheduler_name;
			err.setter( true, buf.str() );
			return false;
		}
		if( !module ){
			// don't find schedule module
			std::stringstream buf;
			buf << "scheduler module not found:" << scheduler_name;
			err.setter( true, buf.str() );
			return false;
		}
		ctrl.unload_module( module );
		request.vs_element.schedule_module_name = scheduler_name;
	}
	if( request.vs_element.protocol_module_name.length() == 0 ){
		//protocol module name error
		err.setter( true, "protocol module not specified." );
		return false;
	}
	if( request.vs_element.udpmode ){
		if( request.vs_element.udp_recv_endpoint == boost::asio::ip::udp::endpoint() ){
			// udp mode,but not acceptor endpoint
			err.setter( true, "udp recv endpoint not specified." );
			return false;
		}
	}
	else{
		if( request.vs_element.tcp_accept_endpoint == boost::asio::ip::tcp::endpoint() ){
			// tcp mode, but not acceptor endpoint
			err.setter( true, "tcp accpeptor endpoint not specified." );
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
		//don't target recvaddress:port
		err.setter( true, "target endpoint is not specified." );
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
	catch( boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> > & sys_err ){
		std::stringstream buf;
		buf << "target endpoint parse error:" << src_str;
		err.setter( true, buf.str() );
		return false;
	}
	return true;
}
//! module option check
bool	l7vs::l7vsadm::parse_opt_vs_module_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		//don't target protomod name.
		err.setter( true, "protomod name is not specified." );
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
		std::stringstream buf;
		buf << "protocol module load error:" << module_name;
		err.setter( true, buf.str() );
		return false;
	}
	if( !module ){
		//don't find protocol module.
		std::stringstream buf;
		buf << "protocol module not found:" << module_name;
		err.setter( true, buf.str() );
		return false;
	}
	// create module args.
	std::vector< std::string > module_args;
	while( true ){
		if( ++pos == argc ) break; //module option end.
		parse_opt_map_type::iterator vsitr = vs_option_dic.find( argv[pos] );
		if( vsitr != vs_option_dic.end() ){
			--pos;	// back for next option
			break;	// module option end.
		}
		parse_opt_map_type::iterator rsitr = rs_option_dic.find( argv[pos] );
		if( rsitr != rs_option_dic.end() ){
			--pos;	// back for next option
			break;	// module option end.
		}
		module_args.push_back( argv[pos] );
	}
	protocol_module_base::check_message_result module_message = module->check_parameter( module_args );
	if( !module_message.flag ){
		// args is not supported.
		err.setter( true, "protocol module argument error." );
		return false;
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
		err.setter( true, "scheduler name is not specified." );
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
		std::stringstream buf;
		buf << "scheduler module load error:" << scheduler_name;
		err.setter( true, buf.str() );
		return false;
	}
	if( !module ){
		// don't find schedule module
		std::stringstream buf;
		buf << "scheduler module not found:" << scheduler_name;
		err.setter( true, buf.str() );
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
		err.setter( true, "maxconnection value is not specified." );
		return false;
	}
	try{
		request.vs_element.sorry_maxconnection = boost::lexical_cast< long long >( argv[pos] );
		if( 0LL == request.vs_element.sorry_maxconnection )
			request.vs_element.sorry_maxconnection = LLONG_MAX;		// clear value
	}
	catch( boost::bad_lexical_cast& e ){
		// don't convert argv[pos] is 
		err.setter( true, "invalid sorry_maxconnection value." );
		return false;
	}
	//check connection limit and zero
	return true;
}
//! bypass(SorryServer) option check
bool	l7vs::l7vsadm::parse_opt_vs_bypass_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		//don't target sorryserver:port
		err.setter( true, "sorryserver endpoint is not specified." );
		return false;
	}
	std::string sorry_endpoint = argv[pos];
	try{
		request.vs_element.sorry_endpoint = string_to_endpoint< boost::asio::ip::tcp > ( sorry_endpoint );
		// clear endpoint check
		if( request.vs_element.sorry_endpoint == boost::asio::ip::tcp::endpoint() ){
			std::string	clear_endpoint = "255.255.255.255:0";		// clear value
			request.vs_element.sorry_endpoint = string_to_endpoint< boost::asio::ip::tcp > ( clear_endpoint );
		}
	}
	catch( boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> > & sys_err ){
		//don't resolve sorryserver endpoint
		std::stringstream buf;
		buf << "sorryserver endpoint parse error:" << sorry_endpoint;
		err.setter( true, buf.str() );
		return false;
	}
	return true;	//
}
//! virtualservice option flag function
bool	l7vs::l7vsadm::parse_opt_vs_flag_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		//don't target sorry flag
		err.setter( true, "sorryflag value is not specified." );
		return false;
	}
	try{
		int	tmp = boost::lexical_cast< int >( argv[pos] );
		if( 0 == tmp )
			request.vs_element.sorry_flag = INT_MAX;	// clear value
		else
			request.vs_element.sorry_flag = 1;
	}
	catch( boost::bad_lexical_cast& e ){
		// don't convert argv[pos] is 
		err.setter( true, "invalid sorryflag value." );
		return false;
	}
	return true;	//
}
//! virtualservice option qosupstream function
bool	l7vs::l7vsadm::parse_opt_vs_qosup_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		//don't rarget QoS upstream value.
		err.setter( true, "qos_upstream value is not specified." );
		return false;
	}
	try{
		virtualservice_element& elem = request.vs_element;	// request virtualservice element refalence get.
		std::string	tmp	= argv[pos];
		std::string::reverse_iterator ritr = tmp.rbegin();
		if( *ritr == 'G' || *ritr == 'g' ){
			std::string	strval = tmp.substr(0, tmp.length() - 1);
			unsigned long long	ullval	= boost::lexical_cast< unsigned long long > ( strval );
			if( ( ULLONG_MAX / 1024 / 1024 / 1024 ) < ullval ){
				err.setter( true, "qos_upstream value is too big." );
				return false;
			}
			elem.qos_upstream = ullval * 1024 * 1024 * 1024;		// set qos_upstream
		}
		else if( *ritr == 'M' || *ritr == 'm' ){
			std::string	strval = tmp.substr(0, tmp.length() - 1);
			unsigned long long	ullval	= boost::lexical_cast< unsigned long long > ( strval );
			if( ( ULLONG_MAX / 1024 / 1024 ) < ullval ){
				err.setter( true, "qos_upstream value is too big." );
				return false;
			}
			elem.qos_upstream = ullval * 1024 * 1024;		// set qos_upstream
		}
		else if( *ritr == 'K' || *ritr == 'k' ){
			std::string	strval = tmp.substr(0, tmp.length() - 1);
			unsigned long long	ullval	= boost::lexical_cast< unsigned long long > ( strval );
			if( ( ULLONG_MAX / 1024 ) < ullval ){
				err.setter( true, "qos_upstream value is too big." );
				return false;
			}
			elem.qos_upstream = ullval * 1024;		// set qos_upstream
		}
		else{
			elem.qos_upstream = boost::lexical_cast< unsigned long long > ( argv[pos] );	// set qos_upstream
		}
		if( 0ULL == elem.qos_upstream )
			elem.qos_upstream = ULLONG_MAX;		// clear value
	}
	catch( boost::bad_lexical_cast& ex ){	// don't convert string to qos_upsatream
		// don't conv qos upstream
		err.setter( true, "invalid qos_upstream value." );
		return false;
	}
	return true;		
}
//! virtualservice option qosdownstream functipn
bool	l7vs::l7vsadm::parse_opt_vs_qosdown_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't target QoS downstream value
		err.setter( true, "qos_downstream value is not specified." );
		return false;
	}
	try{
		virtualservice_element& elem = request.vs_element;	// request virtualservice element refalence get.
		std::string	tmp	= argv[pos];
		std::string::reverse_iterator ritr = tmp.rbegin();
		if( *ritr == 'G' || *ritr == 'g' ){
			std::string	strval = tmp.substr(0, tmp.length() - 1);
			unsigned long long	ullval	= boost::lexical_cast< unsigned long long > ( strval );
			if( ( ULLONG_MAX / 1024 / 1024 / 1024 ) < ullval ){
				err.setter( true, "qos_downstream value is too big." );
				return false;
			}
			elem.qos_downstream = ullval * 1024 * 1024 * 1024;		// set qos_upstream
		}
		else if( *ritr == 'M' || *ritr == 'm' ){
			std::string	strval = tmp.substr(0, tmp.length() - 1);
			unsigned long long	ullval	= boost::lexical_cast< unsigned long long > ( strval );
			if( ( ULLONG_MAX / 1024 / 1024 ) < ullval ){
				err.setter( true, "qos_downstream value is too big." );
				return false;
			}
			elem.qos_downstream = ullval * 1024 * 1024;		// set qos_upstream
		}
		else if( *ritr == 'K' || *ritr == 'k' ){
			std::string	strval = tmp.substr(0, tmp.length() - 1);
			unsigned long long	ullval	= boost::lexical_cast< unsigned long long > ( strval );
			if( ( ULLONG_MAX / 1024 ) < ullval ){
				err.setter( true, "qos_downstream value is too big." );
				return false;
			}
			elem.qos_downstream = ullval * 1024;		// set qos_upstream
		}
		else{
			elem.qos_downstream = boost::lexical_cast< unsigned long long > ( argv[pos] );	// set qos_downstream
		}
		if( 0ULL == elem.qos_downstream )
			elem.qos_downstream = ULLONG_MAX;		// clear value
	}
	catch( boost::bad_lexical_cast& ex ){
		// don' conv qos downstream
		err.setter( true, "invalid qos_downstream value." );
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
		std::string	endpoint	= sstream.str();
		try{
			elem.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( endpoint );
			elem.tcp_accept_endpoint = zeropoint;
		}
		catch( boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> >& e ){
			std::stringstream buf;
			buf << "target endpoint parse error:" << endpoint;
			err.setter( true, buf.str() );
			return false;
		}
	}
	if( elem.realserver_vector.size() != 0 && elem.realserver_vector.front().tcp_endpoint != zeropoint ){
		std::stringstream 	sstream;
		sstream << elem.realserver_vector.front().tcp_endpoint;
		std::string	endpoint = sstream.str();
		try{
			elem.realserver_vector.front().udp_endpoint = string_to_endpoint< boost::asio::ip::udp > ( endpoint );
			elem.realserver_vector.front().tcp_endpoint = zeropoint;
		}
		catch( boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> >& e ){
			std::stringstream buf;
			buf << "realserver endpoint parse error:" << endpoint;
			err.setter( true, buf.str() );
			return false;
		}
	}
	return true;
}

//! realserver command parsing.
bool	l7vs::l7vsadm::parse_rs_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	request.vs_element.realserver_vector.push_back( realserver_element() );
	for( int pos = 2; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = rs_option_dic.find( argv[pos] );
		if( itr != rs_option_dic.end() ){
			if( ! itr->second( pos, argc, argv ) ) return false;
		}
		else{
			std::stringstream buf;
			buf << "realserver option not found:" << argv[pos];
			err.setter( true, buf.str() );
			return false;
		}
	}

	if( request.vs_element.protocol_module_name.length() == 0 ){
		//protocol module name error
		err.setter( true, "protocol module not specified." );
		return false;
	}
	if( request.vs_element.udpmode ){
		if( request.vs_element.udp_recv_endpoint == boost::asio::ip::udp::endpoint() ){
			// udp mode,but not acceptor endpoint
			err.setter( true, "udp recv endpoint not specified." );
			return false;
		}
		if( request.vs_element.realserver_vector.front().udp_endpoint == boost::asio::ip::udp::endpoint() ){
			// udp mode,but not realserver endpoint
			err.setter( true, "realserver udp endpoint not specified." );
			return false;
		}
	}
	else{
		if( request.vs_element.tcp_accept_endpoint == boost::asio::ip::tcp::endpoint() ){
			// tcp mode, but not acceptor endpoint
			err.setter( true, "tcp accpeptor endpoint not specified." );
			return false;
		}
		if( request.vs_element.realserver_vector.front().tcp_endpoint == boost::asio::ip::tcp::endpoint() ){
			// tcp mode,but not realserver endpoint
			err.setter( true, "realserver tcp endpoint not specified." );
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
		err.setter( true, "weight value is not specified." );
		return false;
	}
	try{
		request.vs_element.realserver_vector.front().weight = boost::lexical_cast<int>( argv[pos] );
		if( 0 == request.vs_element.realserver_vector.front().weight )
			request.vs_element.realserver_vector.front().weight = INT_MAX;		// clear value
	}
	catch( boost::bad_lexical_cast& ex ){
		// lexical cast error
		err.setter( true, "invalid weight value." );
		return false;
	}
	return true;
}
//! realserver target set
bool	l7vs::l7vsadm::parse_opt_rs_realserver_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't target realserver address
		err.setter( true, "realserver address is not specified." );
		return false;
	}
	std::string	src_str = argv[pos];
	try{
		if( request.vs_element.udpmode ){
			request.vs_element.realserver_vector.front().udp_endpoint = string_to_endpoint< boost::asio::ip::udp >( src_str );
		}
		else{
			request.vs_element.realserver_vector.front().tcp_endpoint = string_to_endpoint< boost::asio::ip::tcp >( src_str );
		}
	}
	catch( ... ){
		// address string error.
		std::stringstream buf;
		buf << "realserver endpoint parse error:" << src_str;
		err.setter( true, buf.str() );
		return false;
	}
	return true;
}
	
//! replication command parsing.
bool	l7vs::l7vsadm::parse_replication_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	for( int pos = 2; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = replication_option_dic.find( argv[pos] );
		if( itr != replication_option_dic.end() ){
			if( ! itr->second( pos, argc, argv ) ) return false;
		}
		else{
			std::stringstream buf;
			buf << "replication option not found:" << argv[pos];
			err.setter( true, buf.str() );
			return false;
		}
	}
	if( l7vsadm_request::REP_NONE == request.replication_command ){
		// not specified replication command
		err.setter( true, "replication command not specified." );
		return false;
	}
	return true;
}

//
//	replication option functions.
//
//! switch function
bool	l7vs::l7vsadm::parse_opt_replication_switch_func( int& pos, int argc, char* argv[] ){
	if( request.replication_command != l7vsadm_request::REP_NONE ){ 
		// double command target.
		err.setter( true, "replication option is double specified." );
		return false;
	}
	if( ++pos >= argc ){
		// don't target replication switch value
		err.setter( true, "replication switch option is not specified." );
		return false;
	}
	parse_opt_map_type::iterator itr = replication_switch_option_dic.find( argv[pos] );
	if( itr != replication_switch_option_dic.end() ){	// option string function find.
		if( ! itr->second( pos, argc, argv ) ) return false;	// option string function error.
	}
	else{	//option string function don't find.
		// print option not found message.
		std::stringstream buf;
		buf << "replication switch option not found:" << argv[pos];
		err.setter( true, buf.str() );
		return false;
	}
	return true;
}
//! start function
bool	l7vs::l7vsadm::parse_opt_replication_start_func( int& pos, int argc, char* argv[] ){
	request.replication_command = l7vsadm_request::REP_START;
	return true;
}
//! stop function
bool	l7vs::l7vsadm::parse_opt_replication_stop_func( int& pos, int argc, char* argv[] ){
	request.replication_command = l7vsadm_request::REP_STOP;
	return true;
}
//! force function
bool	l7vs::l7vsadm::parse_opt_replication_force_func( int& pos, int argc, char* argv[] ){
	if( request.replication_command != l7vsadm_request::REP_NONE ){ 
		// double command target.
		err.setter( true, "replication option is double specified." );
		return false;
	}
	request.replication_command = l7vsadm_request::REP_FORCE;
	return true;
}
//! dump function
bool	l7vs::l7vsadm::parse_opt_replication_dump_func( int& pos, int argc, char* argv[] ){
	if( request.replication_command != l7vsadm_request::REP_NONE ){ 
		// double command target.
		err.setter( true, "replication option is double specified." );
		return false;
	}
	request.replication_command = l7vsadm_request::REP_DUMP;
	return true;
}

//! log command parsing.
bool	l7vs::l7vsadm::parse_log_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	for( int pos = 2; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = log_option_dic.find( argv[pos] );
		if( itr != log_option_dic.end() ){	// option string function find.
			if( ! itr->second( pos, argc, argv ) ) return false;	// option string function error.
		}
		else{	//option string function don't find.
			// print option not found message.
			std::stringstream buf;
			buf << "log option not found:" << argv[pos];
			err.setter( true, buf.str() );
			return false;
		}
	}
	if( LOG_CAT_NONE == request.log_category ){
		// not specified logcategory
		err.setter( true, "logcategory not specified." );
		return false;
	}
	if( LOG_LV_NONE == request.log_level ){
		// not specified loglevel
		err.setter( true, "loglevel not specified." );
		return false;
	}
	return true;
}
//
//	log option function
//
//! log category set function
bool	l7vs::l7vsadm::parse_opt_log_category_func( int& pos, int argc, char* argv[] ){
	if( request.log_category != LOG_CAT_NONE ){
		// double target commands.
		err.setter( true, "logcategory is double specified." );
		return false;
	}
	if( ++pos >= argc ){
		// don't target logcategory
		err.setter( true, "logcategory is not specified." );
		return false;
	}
	string_logcategory_map_type::iterator itr = string_logcategory_dic.find( argv[pos] );
	if( itr != string_logcategory_dic.end() ){
		request.log_category = itr->second;
		return true;
	}
	std::stringstream buf;
	buf << "logcategory not found:" << argv[pos];
	err.setter( true, buf.str() );
	return false;
}
//! log level set function
bool	l7vs::l7vsadm::parse_opt_log_level_func( int& pos, int argc, char* argv[] ){
	if( request.log_level != LOG_LV_NONE ){
		// double target commands.
		err.setter( true, "loglevel is double specified." );
		return false;
	}
	if( ++pos >= argc ){
		// don't target loglevel
		err.setter( true, "loglevel is not specified." );
		return false;
	}
	string_loglevel_map_type::iterator itr = string_loglevel_dic.find( argv[pos] );
	if( itr != string_loglevel_dic.end() ){
		request.log_level = itr->second;
		return true;
	}
	std::stringstream buf;
	buf << "loglevel not found:" << argv[pos];
	err.setter( true, buf.str() );
	return false;
}

//! snmp command parsing
bool	l7vs::l7vsadm::parse_snmp_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	for( int pos = 2; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = snmp_option_dic.find( argv[pos] );
		if( itr != snmp_option_dic.end() ){	// option string function find.
			if( ! itr->second( pos, argc, argv ) ) return false;	// option string function error.
		}
		else{	//option string function don't find.
			// print option not found message.
			std::stringstream buf;
			buf << "snmp log option not found:" << argv[pos];
			err.setter( true, buf.str() );
			return false;
		}
	}
	if( LOG_CAT_NONE == request.snmp_log_category ){
		// not specified logcategory
		err.setter( true, "snmp logcategory not specified." );
		return false;
	}
	if( LOG_LV_NONE == request.snmp_log_level ){
		// not specified loglevel
		err.setter( true, "snmp loglevel not specified." );
		return false;
	}
	return true;
}
//! snmp log category set function
bool	l7vs::l7vsadm::parse_opt_snmp_log_category_func( int& pos, int argc, char* argv[] ){
	if( request.snmp_log_category != LOG_CAT_NONE ){
		// double target commands.
		err.setter( true, "snmp logcategory is double specified." );
		return false;
	}
	if( ++pos >= argc ){
		// don't target logcategory
		err.setter( true, "snmp logcategory is not specified." );
		return false;
	}
	string_logcategory_map_type::iterator itr = string_snmp_logcategory_dic.find( argv[pos] );
	if( itr != string_snmp_logcategory_dic.end() ){
		request.snmp_log_category = itr->second;
		return true;
	}
	std::stringstream buf;
	buf << "snmp logcategory not found:" << argv[pos];
	err.setter( true, buf.str() );
	return false;
}
//! log level set function
bool	l7vs::l7vsadm::parse_opt_snmp_log_level_func( int& pos, int argc, char* argv[] ){
	if( request.snmp_log_level != LOG_LV_NONE ){
		// double target commands.
		err.setter( true, "snmp loglevel is double specified." );
		return false;
	}
	if( ++pos >= argc ){
		// don't rarget logcategory
		err.setter( true, "snmp loglevel is not specified." );
		return false;
	}
	string_loglevel_map_type::iterator itr = string_loglevel_dic.find( argv[pos] );
	if( itr != string_loglevel_dic.end() ){
		request.snmp_log_level = itr->second;
		return true;
	}
	std::stringstream buf;
	buf << "snmp loglevel not found:" << argv[pos];
	err.setter( true, buf.str() );
	return false;
}

//! parameter command parsing
bool	l7vs::l7vsadm::parse_parameter_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	request.command = cmd;
	for( int pos = 2; pos < argc; ++pos ){
		parse_opt_map_type::iterator itr = parameter_option_dic.find( argv[pos] );
		if( itr != parameter_option_dic.end() ){	// option string function find.
			if( ! itr->second( pos, argc, argv ) ) return false;	// option string function error.
		}
		else{	//option string function don't find.
			// print option not found message.
			std::stringstream buf;
			buf << "parameter option not found:" << argv[pos];
			err.setter( true, buf.str() );
			return false;
		}
	}
	if( PARAM_COMP_NOCAT == request.reload_param ){
		// not specified reload_param
		err.setter( true, "reload component not specified." );
		return false;
	}
	return true;
}
//
//	parameter command 
//
bool	l7vs::l7vsadm::parse_opt_parameter_reload_func( int& pos, int argc, char* argv[] ){
	if( ++pos >= argc ){
		// don't target reload component
		err.setter( true, "reload component is not specified." );
		return false;
	}
	string_parameter_map_type::iterator itr = string_parameter_dic.find( argv[pos] );
	if( itr != string_parameter_dic.end() ){
		request.reload_param = itr->second;
		return true;
	}
	std::stringstream buf;
	buf << "reload component not found:" << argv[pos];
	err.setter( true, buf.str() );
	return false;
}

//! help command parsing
bool	l7vs::l7vsadm::parse_help_func( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] ){
	help_mode = true;

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

//!	disp_list function
void	l7vs::l7vsadm::disp_list(){
	std::stringstream buf;
	buf << boost::format( "Layer-7 Virtual Server version %s\n" ) % L7VS_VERSION_STRING;
	buf << "Prot LocalAddress:Port ProtoMod Scheduler\n";
	buf << "  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn\n";
	BOOST_FOREACH( virtualservice_element vse, response.virtualservice_status_list ){
		buf << boost::format( "TCP %s %s %s\n" )
			% vse.tcp_accept_endpoint
			% vse.protocol_module_name
			% vse.schedule_module_name;
		BOOST_FOREACH( realserver_element rse, vse.realserver_vector ){
			buf << boost::format( "  -> %-28s %-7s %-6d %-10d %-10d\n" )
				% rse.tcp_endpoint
				% "Masq"
				% rse.weight
				% rse.get_active()
				% rse.get_inact();
		}
	}
	std::cout << buf.str() << std::endl;
}

//!	disp_list_key function
void	l7vs::l7vsadm::disp_list_key(){

}

//!	disp_list_verbose function
void	l7vs::l7vsadm::disp_list_verbose(){

}
//
// l7vsadm constractor.
// create including all dictionary.
l7vs::l7vsadm::l7vsadm()
				:	numeric_flag(false),
					help_mode(false){

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
	rs_option_dic["-p"]				= boost::bind( &l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3 );
	rs_option_dic["--udp"]			= boost::bind( &l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3 );
	rs_option_dic["-r"]				= boost::bind( &l7vsadm::parse_opt_rs_realserver_func, this, _1, _2, _3 );
	rs_option_dic["--real-server"]	= boost::bind( &l7vsadm::parse_opt_rs_realserver_func, this, _1, _2, _3 );
	// create replication option dictionary
	replication_option_dic["-s"]	= boost::bind( &l7vsadm::parse_opt_replication_switch_func, this, _1, _2, _3 );
	replication_option_dic["--switch"]
									= boost::bind( &l7vsadm::parse_opt_replication_switch_func, this, _1, _2, _3 );
	replication_switch_option_dic["start"]
									= boost::bind( &l7vsadm::parse_opt_replication_start_func, this, _1, _2, _3 );
	replication_switch_option_dic["stop"]
									= boost::bind( &l7vsadm::parse_opt_replication_stop_func, this, _1, _2, _3 );
	replication_option_dic["-f"]	= boost::bind( &l7vsadm::parse_opt_replication_force_func, this, _1, _2, _3 );
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
	string_logcategory_dic["l7vsd_network"]					= LOG_CAT_L7VSD_NETWORK;
	string_logcategory_dic["nw"]							= LOG_CAT_L7VSD_NETWORK;
	string_logcategory_dic["l7vsd_network.qos"]				= LOG_CAT_L7VSD_NETWORK_QOS;
	string_logcategory_dic["nw.qos"]						= LOG_CAT_L7VSD_NETWORK_QOS;
	string_logcategory_dic["l7vsd_network.bandwidth"]		= LOG_CAT_L7VSD_NETWORK_BANDWIDTH;
	string_logcategory_dic["nw.bw"]							= LOG_CAT_L7VSD_NETWORK_BANDWIDTH;
	string_logcategory_dic["l7vsd_network.num_connection"]	= LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION;
	string_logcategory_dic["nw.conn"]						= LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION;
	string_logcategory_dic["l7vsd_network.access"]			= LOG_CAT_L7VSD_NETWORK_ACCESS;
	string_logcategory_dic["nw.acc"]						= LOG_CAT_L7VSD_NETWORK_ACCESS;
	string_logcategory_dic["l7vsd_mainthread"]				= LOG_CAT_L7VSD_MAINTHREAD;
	string_logcategory_dic["mth"]							= LOG_CAT_L7VSD_MAINTHREAD;
	string_logcategory_dic["l7vsd_virtualservice"]			= LOG_CAT_L7VSD_VIRTUALSERVICE;
	string_logcategory_dic["vs"]							= LOG_CAT_L7VSD_VIRTUALSERVICE;
	string_logcategory_dic["l7vsd_virtualservice.thread"]	= LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD;
	string_logcategory_dic["vs.th"]							= LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD;
	string_logcategory_dic["l7vsd_session"]					= LOG_CAT_L7VSD_SESSION;
	string_logcategory_dic["ss"]							= LOG_CAT_L7VSD_SESSION;
	string_logcategory_dic["l7vsd_session.thread"]			= LOG_CAT_L7VSD_SESSION_THREAD;
	string_logcategory_dic["ss.th"]							= LOG_CAT_L7VSD_SESSION_THREAD;
	string_logcategory_dic["l7vsd_realserver"]				= LOG_CAT_L7VSD_REALSERVER;
	string_logcategory_dic["rs"]							= LOG_CAT_L7VSD_REALSERVER;
	string_logcategory_dic["l7vsd_sorryserver"]				= LOG_CAT_L7VSD_SORRYSERVER;
	string_logcategory_dic["sorry"]							= LOG_CAT_L7VSD_SORRYSERVER;
	string_logcategory_dic["l7vsd_module"]					= LOG_CAT_L7VSD_MODULE;
	string_logcategory_dic["mod"]							= LOG_CAT_L7VSD_MODULE;
	string_logcategory_dic["l7vsd_replication"]				= LOG_CAT_L7VSD_REPLICATION;
	string_logcategory_dic["rep"]							= LOG_CAT_L7VSD_REPLICATION;
	string_logcategory_dic["l7vsd_replication.sendthread"]	= LOG_CAT_L7VSD_REPLICATION_SENDTHREAD;
	string_logcategory_dic["rep.sth"]						= LOG_CAT_L7VSD_REPLICATION_SENDTHREAD;
	string_logcategory_dic["l7vsd_parameter"]				= LOG_CAT_L7VSD_PARAMETER;
	string_logcategory_dic["para"]							= LOG_CAT_L7VSD_PARAMETER;
	string_logcategory_dic["l7vsd_logger"]					= LOG_CAT_L7VSD_LOGGER;
	string_logcategory_dic["logger"]						= LOG_CAT_L7VSD_LOGGER;
	string_logcategory_dic["l7vsd_command"]					= LOG_CAT_L7VSD_COMMAND;
	string_logcategory_dic["cmd"]							= LOG_CAT_L7VSD_COMMAND;
	string_logcategory_dic["l7vsd_start_stop"]				= LOG_CAT_L7VSD_START_STOP;
	string_logcategory_dic["stastp"]						= LOG_CAT_L7VSD_START_STOP;
	string_logcategory_dic["l7vsd_system"]					= LOG_CAT_L7VSD_SYSTEM;
	string_logcategory_dic["sys"]							= LOG_CAT_L7VSD_SYSTEM;
	string_logcategory_dic["l7vsd_system.memory"]			= LOG_CAT_L7VSD_SYSTEM_MEMORY;
	string_logcategory_dic["sys.mem"]						= LOG_CAT_L7VSD_SYSTEM_MEMORY;
	string_logcategory_dic["l7vsd_system.endpoint"]			= LOG_CAT_L7VSD_SYSTEM_ENDPOINT;
	string_logcategory_dic["sys.ep"]						= LOG_CAT_L7VSD_SYSTEM_ENDPOINT;
	string_logcategory_dic["l7vsd_system.signal"]			= LOG_CAT_L7VSD_SYSTEM_SIGNAL;
	string_logcategory_dic["sys.sig"]						= LOG_CAT_L7VSD_SYSTEM_SIGNAL;
	string_logcategory_dic["l7vsd_system_environment"]		= LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT;
	string_logcategory_dic["sys.env"]						= LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT;
	string_logcategory_dic["protocol"]						= LOG_CAT_PROTOCOL;
	string_logcategory_dic["prot"]							= LOG_CAT_PROTOCOL;
	string_logcategory_dic["schedule"]						= LOG_CAT_SCHEDULE;
	string_logcategory_dic["sched"]							= LOG_CAT_SCHEDULE;
	string_logcategory_dic["all"]							= LOG_CAT_END;

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
	string_snmp_logcategory_dic["snmp_vsdsnd"]					= LOG_CAT_SNMPAGENT_L7VSD_SEND;
	string_snmp_logcategory_dic["snmpagent_logger"]				= LOG_CAT_SNMPAGENT_LOGGER;
	string_snmp_logcategory_dic["snmp_logger"]					= LOG_CAT_SNMPAGENT_LOGGER;
	string_snmp_logcategory_dic["snmpagent_parameter"]			= LOG_CAT_SNMPAGENT_PARAMETER;
	string_snmp_logcategory_dic["snmp_para"]					= LOG_CAT_SNMPAGENT_PARAMETER;
	string_snmp_logcategory_dic["snmpagent_system"]				= LOG_CAT_SNMPAGENT_SYSTEM;
	string_snmp_logcategory_dic["snmp_sys"]						= LOG_CAT_SNMPAGENT_SYSTEM;
	string_snmp_logcategory_dic["snmpagent_system.memory"]		= LOG_CAT_SNMPAGENT_SYSTEM_MEMORY;
	string_snmp_logcategory_dic["snmp_sys.mem"]					= LOG_CAT_SNMPAGENT_SYSTEM_MEMORY;
	string_snmp_logcategory_dic["snmpagent_system.endpoint"]	= LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT;
	string_snmp_logcategory_dic["snmp_sys.ep"]					= LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT;
	string_snmp_logcategory_dic["snmpagent_system.signal"]		= LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL;
	string_snmp_logcategory_dic["snmp_sys.sig"]					= LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL;
	string_snmp_logcategory_dic["snmpagent_system.environment"]	= LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT;
	string_snmp_logcategory_dic["snmp_sys.env"]					= LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT;
	string_snmp_logcategory_dic["all"]							= LOG_CAT_END;

	// string log level dictionary create.
	string_loglevel_dic["debug"]	= LOG_LV_DEBUG;
	string_loglevel_dic["info"]		= LOG_LV_INFO;
	string_loglevel_dic["warn"]		= LOG_LV_WARN;
	string_loglevel_dic["error"]	= LOG_LV_ERROR;
	string_loglevel_dic["fatal"]	= LOG_LV_FATAL;

	// parameter category dictionary create
	string_parameter_dic["all"]				= PARAM_COMP_ALL;
	string_parameter_dic["l7vsd"]			= PARAM_COMP_L7VSD;
	string_parameter_dic["command"]			= PARAM_COMP_COMMAND;
	string_parameter_dic["session"]			= PARAM_COMP_SESSION;
	string_parameter_dic["virtualservice"]	= PARAM_COMP_VIRTUALSERVICE;
	string_parameter_dic["module"]			= PARAM_COMP_MODULE;
	string_parameter_dic["replication"]		= PARAM_COMP_REPLICATION;
	string_parameter_dic["logger"]			= PARAM_COMP_LOGGER;
	string_parameter_dic["l7vsadm"]			= PARAM_COMP_L7VSADM;
	string_parameter_dic["snmpagent"]		= PARAM_COMP_SNMPAGENT;
	string_parameter_dic["sslproxy"]		= PARAM_COMP_SSLPROXY;

	// create disp_result dictionary.
	disp_result_dic[l7vsadm_request::CMD_LIST]			= boost::bind( &l7vsadm::disp_list, this );
	disp_result_dic[l7vsadm_request::CMD_LIST_KEY]		= boost::bind( &l7vsadm::disp_list_key, this );
	disp_result_dic[l7vsadm_request::CMD_LIST_VERBOSE]	= boost::bind( &l7vsadm::disp_list_verbose, this );

	// response_message_dic create
	response_error_message_dic[l7vsd_response::RESPONSE_ERROR]				= "command error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_LIST_ERROR]			= "list command error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_LIST_VERBOSE_ERROR]	= "list verbose error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_LIST_KEY_ERROR]		= "list key error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_ADD_VS_ERROR]		= "add vs error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_DEL_VS_ERROR]		= "del vs error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_EDIT_VS_ERROR]		= "edit vs error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_FLUSH_VS_ERROR]		= "flush vs error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_ADD_RS_ERROR]		= "add rs error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_DEL_RS_ERROR]		= "del rs error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_EDIT_RS_ERROR]		= "edit rs error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_REPLICATION_ERROR]	= "replication command error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_LOG_ERROR]			= "log command error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_SNMP_ERROR]			= "snmp command error : ";
	response_error_message_dic[l7vsd_response::RESPONSE_PARAMETER_ERROR]	= "parameter error : ";
}

bool	l7vs::l7vsadm::execute( int argc, char* argv[] ){
	// no argument, assume list command
	if( 1 == argc ){
		request.command = l7vsadm_request::CMD_LIST;
	}
	else {
		// parse command line
		int pos = 1;
		parse_cmd_map_type::iterator itr = command_dic.find( argv[pos] );
		if( itr != command_dic.end() ){
			itr->second( argc, argv );
		}
		else{
			err.setter( true, "command not found." );
		}
	}

	// display command parse result
	if( err ){
		std::cerr << "PARSE ERROR : " << err.get_message() << std::endl;
		std::cerr << usage() << std::endl;
		return false;
	}

	if( !help_mode ){
		// communicate to l7vsd
		using boost::asio::local::stream_protocol;
		boost::array< char, COMMAND_BUFFER_SIZE >	buf;

		// connect
		boost::asio::io_service	io;
		stream_protocol::socket	s( io );
		s.connect(stream_protocol::endpoint( L7VS_CONFIG_SOCK_PATH ));

		// write sockfile
		std::stringstream	send_stream;
		boost::archive::text_oarchive	oa( send_stream );
		oa << (const l7vs::l7vsadm_request&) request;
		boost::asio::write( s, boost::asio::buffer( send_stream.str() ) );

		// read sockfile
		s.read_some( boost::asio::buffer( buf ) );
		
		std::stringstream	recv_stream;
		recv_stream << &(buf[0]);
		boost::archive::text_iarchive	ia( recv_stream );
		ia >> response;

		// close socket
		s.close();
	
		// display result
		if( l7vsd_response::RESPONSE_OK == response.status ){
			disp_result_map_type::iterator	itr = disp_result_dic.find( request.command );
			if( itr != disp_result_dic.end() )
				itr->second();
		}
		else{
			response_error_message_map_type::iterator	itr = response_error_message_dic.find( response.status );
			if( itr != response_error_message_dic.end() )
				std::cerr << itr->second << response.message << std::endl;
			else
				std::cerr << "command error : " << response.message << std::endl;
			return false;
		}
	}
	return true;
}

#ifndef	UNIT_TEST
int main( int argc, char* argv[] ){
	l7vs::Logger		logger;
	l7vs::Parameter		param;
	logger.loadConf();

	l7vs::l7vsadm	adm;
	if( !adm.execute( argc, argv ) ){
		return -1;
	}
	return 0;
}
#endif	//UNIT_TEST

