#define	UNIT_TEST

#include <boost/test/included/unit_test.hpp>

//#include "error_code.h"

#include "logger.h"
#include "parameter.h"

#include "protocol_module_stub.h"
#include "schedule_module_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"

#include "../../src/l7vsadm.cpp"

using namespace boost::unit_test_framework;


// test class
class	l7vsadm_test	:public	l7vs::l7vsadm {
public:

	l7vs::l7vsadm_request&		get_request()	{ return request; }
	l7vs::l7vsd_response&		get_response()	{ return response; }

	l7vs::error_code&	get_error()	{ return err; }

	bool&	get_numeric()	{ return numeric_flag; }

	bool	parse_list_func_wp( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] )
	{ return parse_list_func( cmd, argc, argv ); }

	bool	parse_opt_vs_target_func_wp( int& pos, int argc, char* argv[] )
	{ return parse_opt_vs_target_func( pos, argc, argv ); }

	bool	parse_opt_vs_module_func_wp( int& pos, int argc, char* argv[] )
	{ return parse_opt_vs_module_func( pos, argc, argv ); }

	bool	parse_opt_vs_scheduler_func_wp( int& pos, int argc, char* argv[] )
	{ return parse_opt_vs_scheduler_func( pos, argc, argv ); }

	bool	parse_opt_vs_upper_func_wp( int& pos, int argc, char* argv[] )
	{ return parse_opt_vs_upper_func( pos, argc, argv ); }

	bool	parse_opt_vs_bypass_func_wp( int& pos, int argc, char* argv[] )
	{ return parse_opt_vs_bypass_func( pos, argc, argv ); }

	bool	parse_opt_vs_flag_func_wp( int& pos, int argc, char* argv[] )
	{ return parse_opt_vs_flag_func( pos, argc, argv ); }

	bool	parse_opt_vs_qosup_func_wp( int& pos, int argc, char* argv[] )
	{ return parse_opt_vs_qosup_func( pos, argc, argv ); }

	bool	parse_opt_vs_qosdown_func_wp( int& pos, int argc, char* argv[] )
	{ return parse_opt_vs_qosdown_func( pos, argc, argv ); }

	bool	parse_opt_vs_udp_func_wp( int& pos, int argc, char* argv[] )
	{ return parse_opt_vs_udp_func( pos, argc, argv ); }

	bool	parse_vs_func_wp( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] )
	{ return parse_vs_func( cmd, argc, argv ); }


};

//util
template< typename InternetProtocol >
boost::asio::ip::basic_endpoint< InternetProtocol > string_to_endpoint( std::string str ){
	std::string::size_type pos = str.find( ":" );
	std::string	hostname = str.substr( 0, pos );
	std::string	portname = str.substr( pos+1, str.length() );
	boost::asio::io_service		io_service;
	typename InternetProtocol::resolver				resolver(io_service);
	typename InternetProtocol::resolver::query		query( hostname, portname );
	typename InternetProtocol::resolver::iterator	end;
	typename InternetProtocol::resolver::iterator	itr = resolver.resolve( query );
	if( itr == end ) return typename InternetProtocol::endpoint();
	return *itr;
}

//--tests--
void	parse_list_func_test(){
	BOOST_MESSAGE( "----- parse_list_func_test start -----" );

	// parse_list_func normal case 1 (no option)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST;
		int		argc	= 2;
		char*	argv[]	= { "l7vsadm_test", "-l" };
	
		bool ret = adm.parse_list_func_wp( cmd, argc, argv );
	
		// unit_test[1] parse_list_func normal case 1 (no option) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_list_func normal case 1 (no option) request check
		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_LIST );
	}

	// parse_list_func normal case 2 (with num option short)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-l", "-n" };
	
		bool ret = adm.parse_list_func_wp( cmd, argc, argv );

		// unit_test[1] parse_list_func normal case 2 (with num option short) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_list_func normal case 2 (with num option short) request check
		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_LIST );
		// unit_test[1] parse_list_func normal case 2 (with num option short) nummeric flag check
		BOOST_CHECK_EQUAL( adm.get_numeric(), true );
	}

	// parse_list_func normal case 3 (with num option long)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-l", "--numeric" };
	
		bool ret = adm.parse_list_func_wp( cmd, argc, argv );

		// unit_test[1] parse_list_func normal case 3 (with num option long) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_list_func normal case 3 (with num option long) request check
		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_LIST );
		// unit_test[1] parse_list_func normal case 3 (with num option long) nummeric flag check
		BOOST_CHECK_EQUAL( adm.get_numeric(), true );
	}

	// parse_list_func error case 1 (invarid option)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-l", "-z" };
	
		bool ret = adm.parse_list_func_wp( cmd, argc, argv );

		// unit_test[1] parse_list_func error case 1 (invarid option) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	BOOST_MESSAGE( "----- parse_list_func_test end -----" );

}

void	parse_opt_vs_target_func_test(){
	BOOST_MESSAGE( "----- parse_opt_vs_target_func_test start -----" );

	// parse_opt_vs_target_func normal case 1 (tcp endpoint)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-t", "10.144.169.87:22100" };
	
		bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_target_func normal case 1 (tcp endpoint) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_target_func normal case 1 (tcp endpoint) endpoint check
		boost::asio::ip::tcp::endpoint	ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, ep );
	}

	// parse_opt_vs_target_func normal case 2 (udp endpoint)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-t", "10.144.169.87:22100" };
		adm.get_request().vs_element.udpmode = true;

		bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_target_func normal case 2 (udp endpoint) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_target_func normal case 2 (udp endpoint) endpoint check
		boost::asio::ip::udp::endpoint	ep = string_to_endpoint<boost::asio::ip::udp>( "10.144.169.87:22100" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.udp_recv_endpoint, ep );
	}

	// parse_opt_vs_target_func error case 1 (invalid endpoint)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-t", "10.144.169.257:22100" };

		bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_target_func error case 1 (invalid endpoint) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_target_func error case 2 (no endpoint)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-t" };

		bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_target_func error case 2 (no endpoint) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}


	BOOST_MESSAGE( "----- parse_opt_vs_target_func_test end -----" );

}

void	parse_opt_vs_module_func_test(){
	BOOST_MESSAGE( "----- parse_opt_vs_module_func_test start -----" );

	// parse_opt_vs_module_func normal case 1 (end with module option)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 5;
		char*	argv[]	= { "l7vsadm_test", "-A", "-m", "cinsert", "option" };
		l7vs::protocol_module_control::load_module_fail = false;
		l7vs::protocol_module_stub::check_parameter_called = false;
		l7vs::protocol_module_stub::check_parameter_fail = false;
	
		bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_module_func normal case 1 (end with module option) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_module_func normal case 1 (end with module option) check_parameter call check
		BOOST_CHECK_EQUAL( l7vs::protocol_module_stub::check_parameter_called, true );
		// unit_test[1] parse_opt_vs_module_func normal case 1 (end with module option) module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
		// unit_test[1] parse_opt_vs_module_func normal case 1 (end with module option) module arg check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "option" );
	}

	// parse_opt_vs_module_func normal case 2 (end with next vs option)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 6;
		char*	argv[]	= { "l7vsadm_test", "-A", "-m", "cinsert", "option", "-t" };
		l7vs::protocol_module_control::load_module_fail = false;
		l7vs::protocol_module_stub::check_parameter_called = false;
		l7vs::protocol_module_stub::check_parameter_fail = false;
	
		bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_module_func normal case 2 (end with next vs option) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_module_func normal case 2 (end with next vs option) check_parameter call check
		BOOST_CHECK_EQUAL( l7vs::protocol_module_stub::check_parameter_called, true );
		// unit_test[1] parse_opt_vs_module_func normal case 2 (end with next vs option) module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
		// unit_test[1] parse_opt_vs_module_func normal case 2 (end with next vs option) module arg check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "option" );

	}

	// parse_opt_vs_module_func normal case 3 (no protocol module arg)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 5;
		char*	argv[]	= { "l7vsadm_test", "-A", "-m", "cinsert", "-t" };
		l7vs::protocol_module_control::load_module_fail = false;
		l7vs::protocol_module_stub::check_parameter_called = false;
		l7vs::protocol_module_stub::check_parameter_fail = false;
	
		bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_module_func normal case 3 (no protocol module arg) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_module_func normal case 3 (no protocol module arg) check_parameter call check
		BOOST_CHECK_EQUAL( l7vs::protocol_module_stub::check_parameter_called, true );
		// unit_test[1] parse_opt_vs_module_func normal case 3 (no protocol module arg) module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
		// unit_test[1] parse_opt_vs_module_func normal case 3 (no protocol module arg) module arg check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.size(), 0U );

	}

	// parse_opt_vs_module_func error case 1 (no protocol module name)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-m" };
		l7vs::protocol_module_control::load_module_fail = false;
		l7vs::protocol_module_stub::check_parameter_called = false;
		l7vs::protocol_module_stub::check_parameter_fail = false;
	
		bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_module_func error case 1 (no protocol module name) return value check
		BOOST_CHECK_EQUAL( ret, false );	

	}

	// parse_opt_vs_module_func error case 2 (protocol module load failed)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 5;
		char*	argv[]	= { "l7vsadm_test", "-A", "-m", "cinsert", "-t" };
		l7vs::protocol_module_control::load_module_fail = true;
		l7vs::protocol_module_stub::check_parameter_called = false;
		l7vs::protocol_module_stub::check_parameter_fail = false;
	
		bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_module_func error case 2 (protocol module load failed) return value check
		BOOST_CHECK_EQUAL( ret, false );	

	}

	// parse_opt_vs_module_func error case 3 (check parameter failed)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 5;
		char*	argv[]	= { "l7vsadm_test", "-A", "-m", "cinsert", "-t" };
		l7vs::protocol_module_control::load_module_fail = false;
		l7vs::protocol_module_stub::check_parameter_called = false;
		l7vs::protocol_module_stub::check_parameter_fail = true;
	
		bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_module_func error case 3 (check parameter failed) return value check
		BOOST_CHECK_EQUAL( ret, false );	
		// unit_test[1] parse_opt_vs_module_func error case 3 (check parameter failed) check_parameter call check
		BOOST_CHECK_EQUAL( l7vs::protocol_module_stub::check_parameter_called, true );

	}

	l7vs::protocol_module_control::load_module_fail = false;
	l7vs::protocol_module_stub::check_parameter_called = false;
	l7vs::protocol_module_stub::check_parameter_fail = false;

	BOOST_MESSAGE( "----- parse_opt_vs_module_func_test end -----" );

}

void	parse_opt_vs_scheduler_func_test(){
	BOOST_MESSAGE( "----- parse_opt_vs_scheduler_func_test start -----" );

	// parse_opt_vs_scheduler_func normal case 1
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-s", "rr" };
		l7vs::schedule_module_control::load_module_fail = false;
	
		bool ret = adm.parse_opt_vs_scheduler_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_scheduler_func normal case 1 return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_scheduler_func normal case 1 module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "rr" );
	}

	// parse_opt_vs_scheduler_func error case 1 (no schedule module name)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-s" };
		l7vs::schedule_module_control::load_module_fail = false;
	
		bool ret = adm.parse_opt_vs_scheduler_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_scheduler_func error case 1 (no schedule module name) return value check
		BOOST_CHECK_EQUAL( ret, false );	

	}

	// parse_opt_vs_scheduler_func error case 2 (schedule module load failed)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-s", "rr" };
		l7vs::schedule_module_control::load_module_fail = true;
	
		bool ret = adm.parse_opt_vs_scheduler_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_scheduler_func error case 2 (schedule module load failed) return value check
		BOOST_CHECK_EQUAL( ret, false );	

	}

	l7vs::schedule_module_control::load_module_fail = false;

	BOOST_MESSAGE( "----- parse_opt_vs_scheduler_func_test end -----" );

}

void	parse_opt_vs_upper_func_test(){
	BOOST_MESSAGE( "----- parse_opt_vs_upper_func_test start -----" );

	// parse_opt_vs_upper_func normal case 1
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-u", "128" };
	
		bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_upper_func normal case 1 return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_upper_func normal case 1 sorry_maxconnection check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 128 );
	}

	// parse_opt_vs_upper_func error case 1 (invalid maxconnection value (charactor))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-u", "a" };
	
		bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_upper_func error case 1 (invalid maxconnection value (charactor)) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_upper_func error case 2 (invalid maxconnection value(long long over))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		std::stringstream	ss;
		ss << (1ULL) + LLONG_MAX;
		char*	argv[]	= { "l7vsadm_test", "-A", "-u", const_cast<char*>( ss.str().c_str() ) };
	
		bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_upper_func error case 2 (invalid maxconnection value(long long over)) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_upper_func error case 3 (no maxconnection value)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-u" };
	
		bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_upper_func error case 3 (no maxconnection value) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	BOOST_MESSAGE( "----- parse_opt_vs_upper_func_test end -----" );

}

void	parse_opt_vs_bypass_func_test(){
	BOOST_MESSAGE( "----- parse_opt_vs_bypass_func_test start -----" );

	// parse_opt_vs_bypass_func normal case 1
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-b", "10.144.169.87:8080" };
	
		bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_bypass_func normal case 1 return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_bypass_func normal case 1 sorry_endpoint check
		boost::asio::ip::tcp::endpoint	ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:8080" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, ep );
	}

	// parse_opt_vs_bypass_func normal case 2 (sorryserver_clear)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-b", "0.0.0.0:0" };
	
		bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_bypass_func normal case 2 (sorryserver_clear) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_bypass_func normal case 2 (sorryserver_clear) sorry_endpoint check
		boost::asio::ip::tcp::endpoint	ep = string_to_endpoint<boost::asio::ip::tcp>( "255.255.255.255:0" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, ep );
	}

	// parse_opt_vs_bypass_func error case 1 (invalid endpoint)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-b", "10.144.169.257:8080" };

		bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_bypass_func error case 1 (invalid endpoint) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_bypass_func error case 2 (no endpoint)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-b" };

		bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_bypass_func error case 2 (no endpoint) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	BOOST_MESSAGE( "----- parse_opt_vs_bypass_func_test end -----" );

}

void	parse_opt_vs_flag_func_test(){
	BOOST_MESSAGE( "----- parse_opt_vs_flag_func_test start -----" );

	// parse_opt_vs_flag_func normal case 1 (flag on)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-f", "1" };
	
		bool ret = adm.parse_opt_vs_flag_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_flag_func normal case 1 (flag on) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_flag_func normal case 1 (flag on) sorry_flag check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_flag, true );
	}

	// parse_opt_vs_flag_func normal case 2 (flag off)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-f", "0" };
	
		bool ret = adm.parse_opt_vs_flag_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_flag_func normal case 2 (flag off) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_flag_func normal case 2 (flag off) sorry_flag check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_flag, false );
	}

	// parse_opt_vs_flag_func error case 1 (no flag)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-f" };

		bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_flag_func error case 1 (no flag) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	BOOST_MESSAGE( "----- parse_opt_vs_flag_func_test end -----" );

}

void	parse_opt_vs_qosup_func_test(){
	BOOST_MESSAGE( "----- parse_opt_vs_qosup_func_test start -----" );

	// parse_opt_vs_qosup_func normal case 1 (no unit postfix)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128" };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func normal case 1 (no unit prefix) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosup_func normal case 1 (no unit prefix) qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 128ULL );
	}

	// parse_opt_vs_qosup_func normal case 2 (unit postfix 'G')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128G" };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func normal case 2 (unit prefix 'G') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosup_func normal case 2 (unit prefix 'G') qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 137438953472ULL );
	}

	// parse_opt_vs_qosup_func normal case 3 (unit postfix 'g')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128g" };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func normal case 3 (unit prefix 'g') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosup_func normal case 3 (unit prefix 'g') qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 137438953472ULL );
	}

	// parse_opt_vs_qosup_func normal case 4 (unit postfix 'M')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128M" };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func normal case 4 (unit prefix 'M') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosup_func normal case 4 (unit prefix 'M') qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 134217728ULL );
	}

	// parse_opt_vs_qosup_func normal case 5 (unit postfix 'm')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128m" };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func normal case 5 (unit prefix 'm') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosup_func normal case 5 (unit prefix 'm') qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 134217728ULL );
	}

	// parse_opt_vs_qosup_func normal case 6 (unit postfix 'K')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128K" };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func normal case 6 (unit prefix 'K') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosup_func normal case 6 (unit prefix 'K') qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 131072ULL );
	}

	// parse_opt_vs_qosup_func normal case 7 (unit postfix 'k')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128k" };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func normal case 7 (unit prefix 'k') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosup_func normal case 7 (unit prefix 'k') qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 131072ULL );
	}

	// parse_opt_vs_qosup_func error case 1 (invalid qos_upstream value (charactor))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "a" };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func error case 1 (invalid qos_upstream value (charactor)) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosup_func error case 2 (invalid qos_upstream value(unsigned long long over))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		std::stringstream	ss;
		ss << "18446744073709551616";	// ULLONG_MAX is 18446744073709551615
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func error case 2 (invalid qos_upstream value(unsigned long long over)) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosup_func error case 3 (invalid qos_upstream value(unsigned long long over in postfix 'G'))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		std::stringstream	ss;
		ss << "17179869184G";	// ULLONG_MAX / 1024 / 1024 / 1024
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func error case 3 (invalid qos_upstream value(unsigned long long over in postfix 'G')) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosup_func error case 4 (invalid qos_upstream value(unsigned long long over in postfix 'M'))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		std::stringstream	ss;
		ss << "17592186044416M";	// ULLONG_MAX / 1024 / 1024
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func error case 4 (invalid qos_upstream value(unsigned long long over in postfix 'M')) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosup_func error case 5 (invalid qos_upstream value(unsigned long long over in postfix 'K'))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		std::stringstream	ss;
		ss << "18014398509481984K";	// ULLONG_MAX / 1024
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func error case 5 (invalid qos_upstream value(unsigned long long over in postfix 'K')) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosup_func error case 6 (no qos_upstream value)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q" };
	
		bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosup_func error case 6 (no qos_upstream value) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	BOOST_MESSAGE( "----- parse_opt_vs_qosup_func_test end -----" );

}

void	parse_opt_vs_qosdown_func_test(){
	BOOST_MESSAGE( "----- parse_opt_vs_qosdown_func_test start -----" );

	// parse_opt_vs_qosdown_func normal case 1 (no unit postfix)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128" };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func normal case 1 (no unit prefix) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosdown_func normal case 1 (no unit prefix) qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 128ULL );
	}

	// parse_opt_vs_qosdown_func normal case 2 (unit postfix 'G')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128G" };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func normal case 2 (unit prefix 'G') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosdown_func normal case 2 (unit prefix 'G') qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 137438953472ULL );
	}

	// parse_opt_vs_qosdown_func normal case 3 (unit postfix 'g')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128g" };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func normal case 3 (unit prefix 'g') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosdown_func normal case 3 (unit prefix 'g') qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 137438953472ULL );
	}

	// parse_opt_vs_qosdown_func normal case 4 (unit postfix 'M')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128M" };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func normal case 4 (unit prefix 'M') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosdown_func normal case 4 (unit prefix 'M') qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 134217728ULL );
	}

	// parse_opt_vs_qosdown_func normal case 5 (unit postfix 'm')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128m" };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func normal case 5 (unit prefix 'm') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosdown_func normal case 5 (unit prefix 'm') qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 134217728ULL );
	}

	// parse_opt_vs_qosdown_func normal case 6 (unit postfix 'K')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128K" };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func normal case 6 (unit prefix 'K') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosdown_func normal case 6 (unit prefix 'K') qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 131072ULL );
	}

	// parse_opt_vs_qosdown_func normal case 7 (unit postfix 'k')
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "128k" };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func normal case 7 (unit prefix 'k') return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_qosdown_func normal case 7 (unit prefix 'k') qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 131072ULL );
	}

	// parse_opt_vs_qosdown_func error case 1 (invalid qos_downstream value (charactor))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", "a" };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func error case 1 (invalid qos_upstream value (charactor)) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosdown_func error case 2 (invalid qos_downstream value(unsigned long long over))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		std::stringstream	ss;
		ss << "18446744073709551616";	// ULLONG_MAX is 18446744073709551615
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func error case 2 (invalid qos_downstream value(unsigned long long over)) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosdown_func error case 3 (invalid qos_downstream value(unsigned long long over in postfix 'G'))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		std::stringstream	ss;
		ss << "17179869184G";	// ULLONG_MAX / 1024 / 1024 / 1024
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func error case 3 (invalid qos_downstream value(unsigned long long over in postfix 'G')) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosdown_func error case 4 (invalid qos_downstream value(unsigned long long over in postfix 'M'))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		std::stringstream	ss;
		ss << "17592186044416M";	// ULLONG_MAX / 1024 / 1024
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func error case 4 (invalid qos_downstream value(unsigned long long over in postfix 'M')) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosdown_func error case 5 (invalid qos_downstream value(unsigned long long over in postfix 'K'))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		std::stringstream	ss;
		ss << "18014398509481984K";	// ULLONG_MAX / 1024
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func error case 5 (invalid qos_downstream value(unsigned long long over in postfix 'K')) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_qosdown_func error case 6 (no qos_upstream value)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-Q" };
	
		bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_qosdown_func error case 6 (no qos_downstream value) return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	BOOST_MESSAGE( "----- parse_opt_vs_qosdown_func_test end -----" );

}

void	parse_opt_vs_udp_func_test(){
	BOOST_MESSAGE( "----- parse_opt_vs_udp_func_test start -----" );

	// parse_opt_vs_udp_func normal case 1
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-p" };
	
		bool ret = adm.parse_opt_vs_udp_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_udp_func normal case 1 return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_udp_func normal case 1 udp_mode check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.udpmode, true );
	}

	// parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint)
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 3;
		char*	argv[]	= { "l7vsadm_test", "-A", "-p" };

		adm.get_request().vs_element.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
		adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );
		adm.get_request().vs_element.realserver_vector.front().tcp_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:80" );
	
		bool ret = adm.parse_opt_vs_udp_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) udp_mode check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.udpmode, true );

		// unit_test[1] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) tcp_accept_endpoint check
		boost::asio::ip::tcp::endpoint	zeropoint;
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, zeropoint );

		// unit_test[1] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) udp_recv_endpoint check
		boost::asio::ip::udp::endpoint	udp_recv_ep = string_to_endpoint<boost::asio::ip::udp>( "10.144.169.87:22100" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.udp_recv_endpoint, udp_recv_ep );

		// unit_test[1] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) realserver tcp_endpoint check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, zeropoint );

		// unit_test[1] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) realserver udp_endpoint check
		boost::asio::ip::udp::endpoint	rs_udp_ep = string_to_endpoint<boost::asio::ip::udp>( "10.144.169.86:80" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().udp_endpoint, rs_udp_ep );

	}

	BOOST_MESSAGE( "----- parse_opt_vs_udp_func_test end -----" );

}

void	parse_vs_func_test(){
	BOOST_MESSAGE( "----- parse_vs_func_test start -----" );

	// parse_vs_func normal case 1 (CMD_ADD_VS short_option)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_ADD_VS;
		int		argc	= 17;
		char*	argv[]	= {	"l7vsadm_test",
							"-A",
							"-t",
							"10.144.169.87:22100",
							"-m",
							"cinsert",
							"mod_arg",
							"-s",
							"lc",
							"-u",
							"50",
							"-b",
							"10.144.169.86:8080",
							"-Q",
							"100M",
							"-q",
							"200M"
							};
	
		bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

		std::cout << adm.get_error().get_message() << std::endl;

		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) request check
		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_ADD_VS );
		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) tcp_accept_endpoint check
		boost::asio::ip::tcp::endpoint	tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) protocol module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) protocol module arg check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) schedule module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "lc" );
		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) sorry_maxconnection check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 50 );
		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) sorry_endpoint check
		boost::asio::ip::tcp::endpoint	sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, sorry_ep );
		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 104857600ULL );
		// unit_test[1] parse_vs_func normal case 1 (CMD_ADD_VS short_option) qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 209715200ULL );
	}

	// parse_vs_func normal case 2 (CMD_ADD_VS long_option)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_ADD_VS;
		int		argc	= 17;
		char*	argv[]	= {	"l7vsadm_test",
							"--add-service",
							"--target",
							"10.144.169.86:11500",
							"--module",
							"curl",
							"specified_url",
							"--scheduler",
							"wrr",
							"--upper",
							"96",
							"--bypass",
							"10.144.169.87:80",
							"--qos-up",
							"12800K",
							"--qos-down",
							"25600K"
							};
	
		bool ret = adm.parse_vs_func_wp( cmd, argc, argv );
	
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) request check
		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_ADD_VS );
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) tcp_accept_endpoint check
		boost::asio::ip::tcp::endpoint	tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:11500" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) protocol module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "curl" );
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) protocol module arg check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "specified_url" );
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) schedule module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "wrr" );
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) sorry_maxconnection check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 96 );
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) sorry_endpoint check
		boost::asio::ip::tcp::endpoint	sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:80" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, sorry_ep );
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 13107200ULL );
		// unit_test[1] parse_vs_func normal case 2 (CMD_ADD_VS long_option) qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 26214400ULL );
	}

	// parse_vs_func normal case 3 (CMD_EDIT_VS short_option)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_EDIT_VS;
		int		argc	= 19;
		char*	argv[]	= {	"l7vsadm_test",
							"-E",
							"-t",
							"10.144.169.87:22100",
							"-m",
							"cinsert",
							"mod_arg",
							"-s",
							"rr",
							"-u",
							"50",
							"-b",
							"10.144.169.86:8080",
							"-f",
							"1",
							"-Q",
							"100M",
							"-q",
							"200M"
							};
	
		bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

		std::cout << adm.get_error().get_message() << std::endl;

		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) request check
		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_EDIT_VS );
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) tcp_accept_endpoint check
		boost::asio::ip::tcp::endpoint	tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) protocol module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) protocol module arg check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) schedule module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "rr" );
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) sorry_maxconnection check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 50 );
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) sorry_endpoint check
		boost::asio::ip::tcp::endpoint	sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, sorry_ep );
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) sorry_flag check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_flag, true );
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 104857600ULL );
		// unit_test[1] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 209715200ULL );
	}

	// parse_vs_func normal case 4 (CMD_EDIT_VS long_option)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_EDIT_VS;
		int		argc	= 19;
		char*	argv[]	= {	"l7vsadm_test",
							"--edit-service",
							"--target",
							"10.144.169.86:11500",
							"--module",
							"curl",
							"specified_url",
							"--scheduler",
							"lc",
							"--upper",
							"96",
							"--bypass",
							"10.144.169.87:80",
							"--flag",
							"1",
							"--qos-up",
							"12800K",
							"--qos-down",
							"25600K"
							};
	
		bool ret = adm.parse_vs_func_wp( cmd, argc, argv );
	
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) request check
		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_EDIT_VS );
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) tcp_accept_endpoint check
		boost::asio::ip::tcp::endpoint	tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:11500" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) protocol module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "curl" );
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) protocol module arg check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "specified_url" );
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) schedule module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "lc" );
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) sorry_maxconnection check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 96 );
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) sorry_endpoint check
		boost::asio::ip::tcp::endpoint	sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:80" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, sorry_ep );
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) sorry_flag check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_flag, true );
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) qos_upstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, 13107200ULL );
		// unit_test[1] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) qos_downstream check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, 26214400ULL );
	}

	// parse_vs_func normal case 5 (CMD_DEL_VS short_option)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_DEL_VS;
		int		argc	= 7;
		char*	argv[]	= {	"l7vsadm_test",
							"-D",
							"-t",
							"10.144.169.87:22100",
							"-m",
							"cinsert",
							"mod_arg"
							};
	
		bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

		std::cout << adm.get_error().get_message() << std::endl;

		// unit_test[1] parse_vs_func normal case 5 (CMD_DEL_VS short_option) return value check
		BOOST_CHECK_EQUAL( ret, true );	
		// unit_test[1] parse_vs_func normal case 5 (CMD_DEL_VS short_option) request check
		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_DEL_VS );
		// unit_test[1] parse_vs_func normal case 5 (CMD_DEL_VS short_option) tcp_accept_endpoint check
		boost::asio::ip::tcp::endpoint	tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
		// unit_test[1] parse_vs_func normal case 5 (CMD_DEL_VS short_option) protocol module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
		// unit_test[1] parse_vs_func normal case 5 (CMD_DEL_VS short_option) protocol module arg check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
	}

	// parse_vs_func normal case 6 (CMD_DEL_VS long_option)
	{
		l7vsadm_test	adm;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_DEL_VS;
		int		argc	= 7;
		char*	argv[]	= {	"l7vsadm_test",
							"--edit-service",
							"--target",
							"10.144.169.86:11500",
							"--module",
							"curl",
							"specified_url"
							};
	
		bool ret = adm.parse_vs_func_wp( cmd, argc, argv );
	
		// unit_test[1] parse_vs_func normal case 6 (CMD_DEL_VS long_option) return value check
		BOOST_CHECK_EQUAL( ret, true );
		// unit_test[1] parse_vs_func normal case 6 (CMD_DEL_VS long_option) request check
		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_DEL_VS );
		// unit_test[1] parse_vs_func normal case 6 (CMD_DEL_VS long_option) tcp_accept_endpoint check
		boost::asio::ip::tcp::endpoint	tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:11500" );
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
		// unit_test[1] parse_vs_func normal case 6 (CMD_DEL_VS long_option) protocol module name check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "curl" );
		// unit_test[1] parse_vs_func normal case 6 (CMD_DEL_VS long_option) protocol module arg check
		BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "specified_url" );
	}


// 	// parse_list_func normal case 2 (with num option short)
// 	{
// 		l7vsadm_test	adm;
// 		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST;
// 		int		argc	= 3;
// 		char*	argv[]	= { "l7vsadm_test", "-l", "-n" };
// 	
// 		bool ret = adm.parse_list_func_wp( cmd, argc, argv );
// 
// 		// unit_test[1] parse_list_func normal case 2 (with num option short) return value check
// 		BOOST_CHECK_EQUAL( ret, true );	
// 		// unit_test[1] parse_list_func normal case 2 (with num option short) request check
// 		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_LIST );
// 		// unit_test[1] parse_list_func normal case 2 (with num option short) nummeric flag check
// 		BOOST_CHECK_EQUAL( adm.get_numeric(), true );
// 	}
// 
// 	// parse_list_func normal case 3 (with num option long)
// 	{
// 		l7vsadm_test	adm;
// 		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST;
// 		int		argc	= 3;
// 		char*	argv[]	= { "l7vsadm_test", "-l", "--numeric" };
// 	
// 		bool ret = adm.parse_list_func_wp( cmd, argc, argv );
// 
// 		// unit_test[1] parse_list_func normal case 3 (with num option long) return value check
// 		BOOST_CHECK_EQUAL( ret, true );	
// 		// unit_test[1] parse_list_func normal case 3 (with num option long) request check
// 		BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_LIST );
// 		// unit_test[1] parse_list_func normal case 3 (with num option long) nummeric flag check
// 		BOOST_CHECK_EQUAL( adm.get_numeric(), true );
// 	}
// 
// 	// parse_list_func error case 1 (invarid option)
// 	{
// 		l7vsadm_test	adm;
// 		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST;
// 		int		argc	= 3;
// 		char*	argv[]	= { "l7vsadm_test", "-l", "-z" };
// 	
// 		bool ret = adm.parse_list_func_wp( cmd, argc, argv );
// 
// 		// unit_test[1] parse_list_func error case 1 (invarid option) return value check
// 		BOOST_CHECK_EQUAL( ret, false );	
// 	}

	BOOST_MESSAGE( "----- parse_vs_func_test end -----" );

}



test_suite*	init_unit_test_suite( int argc, char* argv[] ){
	test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

	l7vs::Logger	logger_instance;
	l7vs::Parameter	parameter_instance;
	logger_instance.loadConf();

	ts->add( BOOST_TEST_CASE( &parse_list_func_test ) );
	ts->add( BOOST_TEST_CASE( &parse_opt_vs_target_func_test ) );
	ts->add( BOOST_TEST_CASE( &parse_opt_vs_module_func_test ) );
	ts->add( BOOST_TEST_CASE( &parse_opt_vs_scheduler_func_test ) );
	ts->add( BOOST_TEST_CASE( &parse_opt_vs_upper_func_test ) );
	ts->add( BOOST_TEST_CASE( &parse_opt_vs_bypass_func_test ) );
	ts->add( BOOST_TEST_CASE( &parse_opt_vs_qosup_func_test ) );
	ts->add( BOOST_TEST_CASE( &parse_opt_vs_qosdown_func_test ) );
	ts->add( BOOST_TEST_CASE( &parse_opt_vs_udp_func_test ) );
	ts->add( BOOST_TEST_CASE( &parse_vs_func_test ) );



	framework::master_test_suite().add( ts );

	return 0;
}
