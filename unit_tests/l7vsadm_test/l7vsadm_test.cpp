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

		// unit_test[1] parse_opt_vs_upper_func normal case 1 return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}

	// parse_opt_vs_upper_func error case 1 (invalid maxconnection value(long long over))
	{
		l7vsadm_test	adm;
		int		pos		= 2;
		int		argc	= 4;
		char*	argv[]	= { "l7vsadm_test", "-A", "-u", "" };
	
		bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

		// unit_test[1] parse_opt_vs_upper_func normal case 1 return value check
		BOOST_CHECK_EQUAL( ret, false );	
	}


	BOOST_MESSAGE( "----- parse_opt_vs_upper_func_test end -----" );

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

	framework::master_test_suite().add( ts );

	return 0;
}
