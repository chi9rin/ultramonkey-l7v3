#include <boost/test/included/unit_test.hpp>

#include "logger.h"
#include "parameter.h"
#include "replication_stub.h"
#include "l7vsd_stub.h"
//#include "command_receiver_stub.h"

#include "../../src/command_session.cpp"

#define	SOCKFILE	"./l7vs"

using namespace boost::unit_test_framework;

// class	command_session_test	:public	l7vs::command_session {
// public:
// 	
// };

//variable

l7vs::l7vsadm_request	request;
l7vs::l7vsd_response	response;

boost::asio::io_service	io;

//--tests--

void	server_thread(){
	//std::cout << "sv_sta" << std::endl;
	io.run();
	//std::cout << "sv_end" << std::endl;

}


void	client_thread( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd ){
	using boost::asio::local::stream_protocol;

	boost::array<char,65535>	buf;

	// accept
	boost::asio::io_service			client_io;
	stream_protocol::acceptor	acc( client_io, stream_protocol::endpoint( SOCKFILE ) );
	stream_protocol::socket	s( client_io );
	//std::cout << "accept" << std::endl;
	acc.accept( s );

// 	// wait
// 	boost::xtime xt;
// 	xtime_get(&xt, boost::TIME_UTC);
// 	xt.sec += 1;
// 	boost::thread::sleep(xt);

	// create request
	request.command = cmd;

	std::stringstream	send_stream;
	boost::archive::text_oarchive	oa( send_stream );
	oa << (const l7vs::l7vsadm_request&) request;

	// send request
	//std::cout << "write" << std::endl;
	boost::asio::write( s, boost::asio::buffer( send_stream.str() ) );

	// recv response
	//std::cout << "recv" << std::endl;
	s.read_some( boost::asio::buffer( buf ) );
	
	std::stringstream	recv_stream;
	recv_stream << &(buf[0]);
	boost::archive::text_iarchive	ia( recv_stream );
	//l7vs::l7vsd_response	response;
	ia >> response;

	s.close();

	//std::cout << "unlink" << std::endl;
	unlink( SOCKFILE );

}

void server_proc( l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd ){
	l7vs::l7vsd	vsd;
	using boost::asio::local::stream_protocol;

	// create client thread
	boost::thread	cl_thd( boost::bind( &client_thread, cmd) );

	// prepare accept wait
	boost::xtime xt;
	xtime_get(&xt, boost::TIME_UTC);
	xt.sec += 1;
	boost::thread::sleep(xt);

	// create session 
	l7vs::command_session::command_session_ptr session( new l7vs::command_session( io, vsd ) );

	// connect
	session->socket().connect(stream_protocol::endpoint( SOCKFILE ));
	session->start();

	// create server thread
	io.reset();
	boost::thread	sv_thd( &server_thread );

	// client thread join
	cl_thd.join();

	//stop server
	io.stop();
	sv_thd.join();
}

void	listvs_test(){

	// CMD_LIST normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::list_vs_called = false;
		l7vs::l7vsd::list_vs_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST;

		//execute	
		server_proc( cmd );

		// unit_test[1] command_session_test normal case CMD_LIST response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[2] command_session_test normal case CMD_LIST vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::list_vs_called, true );
	}

	// CMD_LIST error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::list_vs_called = false;
		l7vs::l7vsd::list_vs_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST;

		//execute	
		server_proc( cmd );

		// unit_test[3] command_session_test error case CMD_LIST response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_LIST_ERROR );
		// unit_test[4] command_session_test error case CMD_LIST vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::list_vs_called, true );
	}

}

void	listvs_verbose_test(){

	// CMD_LIST_VERBOSE normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::list_vs_v_called = false;
		l7vs::l7vsd::list_vs_v_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST_VERBOSE;

		//execute	
		server_proc( cmd );

		// unit_test[5] command_session_test normal case CMD_LIST_VERBOSE response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[6] command_session_test normal case CMD_LIST_VERBOSE vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::list_vs_called, true );
	}

	// CMD_LIST_VERBOSE error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::list_vs_v_called = false;
		l7vs::l7vsd::list_vs_v_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST_VERBOSE;

		//execute	
		server_proc( cmd );

		// unit_test[7] command_session_test error case CMD_LIST_VERBOSE response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_LIST_VERBOSE_ERROR );
		// unit_test[8] command_session_test error case CMD_LIST_VERBOSE vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::list_vs_v_called, true );
	}
}

void	listvs_key_test(){

	// CMD_LIST_KEY normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::list_vs_called = false;
		l7vs::l7vsd::list_vs_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST_KEY;

		//execute	
		server_proc( cmd );

		// unit_test[9] command_session_test normal case CMD_LIST_KEY response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[10] command_session_test normal case CMD_LIST_KEY vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::list_vs_called, true );
	}

	// CMD_LIST_KEY error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::list_vs_called = false;
		l7vs::l7vsd::list_vs_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LIST_KEY;

		//execute	
		server_proc( cmd );

		// unit_test[11] command_session_test error case CMD_LIST_KEY response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_LIST_KEY_ERROR );
		// unit_test[12] command_session_test error case CMD_LIST_KEY vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::list_vs_called, true );
	}
}

void	addvs_test(){

	// CMD_ADD_VS normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::add_vs_called = false;
		l7vs::l7vsd::add_vs_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_ADD_VS;

		//execute	
		server_proc( cmd );

		// unit_test[13] command_session_test normal case CMD_ADD_VS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[14] command_session_test normal case CMD_ADD_VS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::add_vs_called, true );
	}

	// CMD_ADD_VS error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::add_vs_called = false;
		l7vs::l7vsd::add_vs_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_ADD_VS;

		//execute	
		server_proc( cmd );

		// unit_test[15] command_session_test error case CMD_ADD_VS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_ADD_VS_ERROR );
		// unit_test[16] command_session_test error case CMD_ADD_VS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::add_vs_called, true );
	}
}

void	delvs_test(){

	// CMD_DEL_VS normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::del_vs_called = false;
		l7vs::l7vsd::del_vs_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_DEL_VS;

		//execute	
		server_proc( cmd );

		// unit_test[17] command_session_test normal case CMD_DEL_VS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[18] command_session_test normal case CMD_DEL_VS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::del_vs_called, true );
	}

	// CMD_DEL_VS error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::del_vs_called = false;
		l7vs::l7vsd::del_vs_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_DEL_VS;

		//execute	
		server_proc( cmd );

		// unit_test[19] command_session_test error case CMD_DEL_VS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_DEL_VS_ERROR );
		// unit_test[20] command_session_test error case CMD_DEL_VS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::del_vs_called, true );
	}
}

void	editvs_test(){

	// CMD_EDIT_VS normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::edit_vs_called = false;
		l7vs::l7vsd::edit_vs_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_EDIT_VS;

		//execute	
		server_proc( cmd );

		// unit_test[21] command_session_test normal case CMD_EDIT_VS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[22] command_session_test normal case CMD_EDIT_VS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::edit_vs_called, true );
	}

	// CMD_EDIT_VS error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::edit_vs_called = false;
		l7vs::l7vsd::edit_vs_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_EDIT_VS;

		//execute	
		server_proc( cmd );

		// unit_test[23] command_session_test error case CMD_EDIT_VS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_EDIT_VS_ERROR );
		// unit_test[24] command_session_test error case CMD_EDIT_VS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::edit_vs_called, true );
	}
}



void	addrs_test(){

	// CMD_ADD_RS normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::add_rs_called = false;
		l7vs::l7vsd::add_rs_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_ADD_RS;

		//execute	
		server_proc( cmd );

		// unit_test[25] command_session_test normal case CMD_ADD_RS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[26] command_session_test normal case CMD_ADD_RS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::add_rs_called, true );
	}

	// CMD_ADD_RS error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::add_rs_called = false;
		l7vs::l7vsd::add_rs_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_ADD_RS;

		//execute	
		server_proc( cmd );

		// unit_test[27] command_session_test error case CMD_ADD_RS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_ADD_RS_ERROR );
		// unit_test[28] command_session_test error case CMD_ADD_RS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::add_rs_called, true );
	}
}

void	delrs_test(){

	// CMD_DEL_RS normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::del_rs_called = false;
		l7vs::l7vsd::del_rs_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_DEL_RS;

		//execute	
		server_proc( cmd );

		// unit_test[29] command_session_test normal case CMD_DEL_RS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[30] command_session_test normal case CMD_DEL_RS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::del_rs_called, true );
	}

	// CMD_DEL_RS error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::del_rs_called = false;
		l7vs::l7vsd::del_rs_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_DEL_RS;

		//execute	
		server_proc( cmd );

		// unit_test[31] command_session_test error case CMD_DEL_RS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_DEL_RS_ERROR );
		// unit_test[32] command_session_test error case CMD_DEL_RS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::del_rs_called, true );
	}
}

void	editrs_test(){

	// CMD_EDIT_RS normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::edit_rs_called = false;
		l7vs::l7vsd::edit_rs_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_EDIT_RS;

		//execute	
		server_proc( cmd );

		// unit_test[33] command_session_test normal case CMD_EDIT_RS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[34] command_session_test normal case CMD_EDIT_RS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::edit_rs_called, true );
	}

	// CMD_EDIT_RS error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::edit_rs_called = false;
		l7vs::l7vsd::edit_rs_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_EDIT_RS;

		//execute	
		server_proc( cmd );

		// unit_test[35] command_session_test error case CMD_EDIT_RS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_EDIT_RS_ERROR );
		// unit_test[36] command_session_test error case CMD_EDIT_RS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::edit_rs_called, true );
	}
}

void	flushvs_test(){

	// CMD_FLUSH_VS normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::flush_vs_called = false;
		l7vs::l7vsd::flush_vs_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_FLUSH_VS;

		//execute	
		server_proc( cmd );

		// unit_test[37] command_session_test normal case CMD_FLUSH_VS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[38] command_session_test normal case CMD_FLUSH_VS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::flush_vs_called, true );
	}

	// CMD_FLUSH_VS error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::flush_vs_called = false;
		l7vs::l7vsd::flush_vs_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_FLUSH_VS;

		//execute	
		server_proc( cmd );

		// unit_test[39] command_session_test error case CMD_FLUSH_VS response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_FLUSH_VS_ERROR );
		// unit_test[40] command_session_test error case CMD_FLUSH_VS vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::flush_vs_called, true );
	}
}

void	replication_test(){

	// CMD_REPLICATION normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::rep_cmd_called = false;
		l7vs::l7vsd::rep_cmd_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_REPLICATION;

		//execute	
		server_proc( cmd );

		// unit_test[41] command_session_test normal case CMD_REPLICATION response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[42] command_session_test normal case CMD_REPLICATION vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::rep_cmd_called, true );
	}

	// CMD_REPLICATION error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::rep_cmd_called = false;
		l7vs::l7vsd::rep_cmd_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_REPLICATION;

		//execute	
		server_proc( cmd );

		// unit_test[43] command_session_test error case CMD_REPLICATION response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_REPLICATION_ERROR );
		// unit_test[44] command_session_test error case CMD_REPLICATION vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::rep_cmd_called, true );
	}
}


void	log_test(){

	// CMD_LOG normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::set_loglevel_called = false;
		l7vs::l7vsd::set_loglevel_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LOG;

		//execute	
		server_proc( cmd );

		// unit_test[45] command_session_test normal case CMD_LOG response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[46] command_session_test normal case CMD_LOG vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::set_loglevel_called, true );
	}

	// CMD_LOG error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::set_loglevel_called = false;
		l7vs::l7vsd::set_loglevel_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_LOG;

		//execute	
		server_proc( cmd );

		// unit_test[47] command_session_test error case CMD_LOG response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_LOG_ERROR );
		// unit_test[48] command_session_test error case CMD_LOG vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::set_loglevel_called, true );
	}
}

void	snmp_test(){

	// CMD_SNMP normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::snmp_set_loglevel_called = false;
		l7vs::l7vsd::snmp_set_loglevel_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_SNMP;

		//execute	
		server_proc( cmd );

		// unit_test[49] command_session_test normal case CMD_SNMP response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[50] command_session_test normal case CMD_SNMP vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::snmp_set_loglevel_called, true );
	}

	// CMD_SNMP error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::snmp_set_loglevel_called = false;
		l7vs::l7vsd::snmp_set_loglevel_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_SNMP;

		//execute	
		server_proc( cmd );

		// unit_test[51] command_session_test error case CMD_SNMP response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_SNMP_ERROR );
		// unit_test[52] command_session_test error case CMD_SNMP vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::snmp_set_loglevel_called, true );
	}
}

void	param_test(){

	// CMD_PARAMETER normal case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::param_called = false;
		l7vs::l7vsd::param_fail = false;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_PARAMETER;

		//execute	
		server_proc( cmd );

		// unit_test[53] command_session_test normal case CMD_PARAMETER response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[54] command_session_test normal case CMD_PARAMETER vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::param_called, true );
	}

	// CMD_PARAMETER error case
	{
		// init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::param_called = false;
		l7vs::l7vsd::param_fail = true;
		l7vs::l7vsadm_request::COMMAND_CODE_TAG	cmd = l7vs::l7vsadm_request::CMD_PARAMETER;

		//execute	
		server_proc( cmd );

		// unit_test[55] command_session_test error case CMD_PARAMETER response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_PARAMETER_ERROR );
		// unit_test[56] command_session_test error case CMD_PARAMETER vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::param_called, true );
	}
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){
	test_suite* ts = BOOST_TEST_SUITE( "command_session class test" );

	l7vs::Logger	logger;
	l7vs::Parameter	parameter;
	logger.loadConf();

	ts->add( BOOST_TEST_CASE( &listvs_test ) );
	ts->add( BOOST_TEST_CASE( &listvs_verbose_test ) );
	ts->add( BOOST_TEST_CASE( &listvs_key_test ) );

	ts->add( BOOST_TEST_CASE( &addvs_test ) );
	ts->add( BOOST_TEST_CASE( &delvs_test ) );
	ts->add( BOOST_TEST_CASE( &editvs_test ) );

	ts->add( BOOST_TEST_CASE( &addrs_test ) );
	ts->add( BOOST_TEST_CASE( &delrs_test ) );
	ts->add( BOOST_TEST_CASE( &editrs_test ) );

	ts->add( BOOST_TEST_CASE( &flushvs_test ) );

	ts->add( BOOST_TEST_CASE( &replication_test ) );
	ts->add( BOOST_TEST_CASE( &log_test ) );
	ts->add( BOOST_TEST_CASE( &snmp_test ) );
	ts->add( BOOST_TEST_CASE( &param_test ) );



	framework::master_test_suite().add( ts );

	return 0;
}
