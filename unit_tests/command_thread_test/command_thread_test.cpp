#include <boost/test/included/unit_test.hpp>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "error_code.h"
#include "logger.h"
#include "parameter.h"

#include "l7vs_command.h"
//#include "command_receiver_stub.h"
#include "virtualservice_stub.h"
#include "realserver_stub.h"
#include "replication_stub.h"
#include "snmpbridge_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"
#include "l7vsd_stub.h"

#include "../../src/command_receiver.cpp"
#include "../../src/command_session.cpp"

#define	SOCKFILE	"./l7vs"

using namespace boost::unit_test_framework;

//variable
boost::asio::io_service			server_io;

//unsigned int list_err_cnt;

l7vs::l7vsd::cmd_num_map_type	error_num_map;

//util

//--tests--

void	io_thread(){
	//std::cout << "run_start" << std::endl;
	server_io.run();
	//std::cout << "run_end" << std::endl;
}

void	cl_thread( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd ){
	using boost::asio::local::stream_protocol;

	boost::array<char,65535>	buf;

	boost::asio::io_service	client_io;
	stream_protocol::socket	s( client_io );

	for(unsigned int i = 0; i < 10000; ++i){
		s.connect(stream_protocol::endpoint( SOCKFILE ));
		//std::cout << "connected" << std::endl;

		l7vs::l7vsadm_request	request;
		request.command = cmd;
		//std::cout << "request_created" << std::endl;
	
		std::stringstream	send_stream;
		boost::archive::text_oarchive	oa( send_stream );
		oa << (const l7vs::l7vsadm_request&) request;
		//std::cout << "write data is " << send_stream.str() << std::endl;
	
		//std::cout << "send" << std::endl;
		boost::asio::write( s, boost::asio::buffer( send_stream.str() ) );

		//std::cout << "recv" << std::endl;
		s.read_some( boost::asio::buffer( buf ) );
		
		std::stringstream	recv_stream;
		recv_stream << &(buf[0]);
		boost::archive::text_iarchive	ia( recv_stream );
		l7vs::l7vsd_response	response;
		ia >> response;

		// error count
		if(response.status != l7vs::l7vsd_response::RESPONSE_OK){
			++error_num_map[cmd];
		}
		// close
		s.close();
	}
}

void	thread_test(){
	BOOST_MESSAGE( "----- thread_test start -----" );

	l7vs::l7vsd	vsd;
	boost::shared_ptr< l7vs::replication >
									rep( new l7vs::replication( server_io ) );
	vsd.set_replication( rep );

	l7vs::command_receiver	receiver( server_io, SOCKFILE, vsd );

	// io thread　start
	//boost::thread	io_thd( &io_thread );
	boost::thread_group	tg;
	for(unsigned int i = 0; i < 100; ++i){
		tg.create_thread( &io_thread );
	}

	// client thread start
	boost::thread	cl_thd1( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_LIST ) );
	boost::thread	cl_thd2( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_LIST_VERBOSE ) );
	boost::thread	cl_thd3( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_LIST_KEY ) );
	boost::thread	cl_thd4( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_ADD_VS ) );
	boost::thread	cl_thd5( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_DEL_VS ) );
	boost::thread	cl_thd6( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_EDIT_VS ) );
	boost::thread	cl_thd7( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_FLUSH_VS ) );
	boost::thread	cl_thd8( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_ADD_RS ) );
	boost::thread	cl_thd9( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_DEL_RS ) );
	boost::thread	cl_thd10( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_EDIT_RS ) );
	boost::thread	cl_thd11( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_REPLICATION ) );
	boost::thread	cl_thd12( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_LOG ) );
	boost::thread	cl_thd13( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_SNMP ) );
	boost::thread	cl_thd14( boost::bind( &cl_thread, l7vs::l7vsadm_request::CMD_PARAMETER ) );

	cl_thd1.join();
	cl_thd2.join();
	cl_thd3.join();
	cl_thd4.join();
	cl_thd5.join();
	cl_thd6.join();
	cl_thd7.join();
	cl_thd8.join();
	cl_thd9.join();
	cl_thd10.join();
	cl_thd11.join();
	cl_thd12.join();
	cl_thd13.join();
	cl_thd14.join();

	// io thread stop
	//std::cout << "io_service_stop" << std::endl;
	server_io.stop();

	//std::cout << "join" << std::endl;
	//io_thd.join();
	tg.join_all();

	// check call and error num
	// unit_test[1] command_receiver_test thread case CMD_LIST callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_LIST], 20000U );
	// unit_test[2] command_receiver_test thread case CMD_LIST errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_LIST], 0U );

	// unit_test[3] command_receiver_test thread case CMD_LIST_VERBOSE callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_LIST_VERBOSE], 10000U );
	// unit_test[4] command_receiver_test thread case CMD_LIST_VERBOSE errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_LIST_VERBOSE], 0U );

	// unit_test[5] command_receiver_test thread case CMD_ADD_VS callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_ADD_VS], 10000U );
	// unit_test[6] command_receiver_test thread case CMD_ADD_VS errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_ADD_VS], 0U );

	// unit_test[7] command_receiver_test thread case CMD_DEL_VS callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_DEL_VS], 10000U );
	// unit_test[8] command_receiver_test thread case CMD_DEL_VS errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_DEL_VS], 0U );

	// unit_test[9] command_receiver_test thread case CMD_EDIT_VS callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_EDIT_VS], 10000U );
	// unit_test[10] command_receiver_test thread case CMD_EDIT_VS errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_EDIT_VS], 0U );

	// unit_test[11] command_receiver_test thread case CMD_FLUSH_VS callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_FLUSH_VS], 10000U );
	// unit_test[12] command_receiver_test thread case CMD_FLUSH_VS errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_FLUSH_VS], 0U );

	// unit_test[13] command_receiver_test thread case CMD_ADD_RS callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_ADD_RS], 10000U );
	// unit_test[14] command_receiver_test thread case CMD_ADD_RS errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_ADD_RS], 0U );

	// unit_test[15] command_receiver_test thread case CMD_DEL_RS callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_DEL_RS], 10000U );
	// unit_test[16] command_receiver_test thread case CMD_DEL_RS errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_DEL_RS], 0U );

	// unit_test[17] command_receiver_test thread case CMD_EDIT_RS callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_EDIT_RS], 10000U );
	// unit_test[18] command_receiver_test thread case CMD_EDIT_RS errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_EDIT_RS], 0U );

	// unit_test[19] command_receiver_test thread case CMD_REPLICATION callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_REPLICATION], 10000U );
	// unit_test[20] command_receiver_test thread case CMD_REPLICATION errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_REPLICATION], 0U );

	// unit_test[21] command_receiver_test thread case CMD_LOG callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_LOG], 10000U );
	// unit_test[22] command_receiver_test thread case CMD_LOG errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_LOG], 0U );

	// unit_test[23] command_receiver_test thread case CMD_SNMP callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_SNMP], 10000U );
	// unit_test[24] command_receiver_test thread case CMD_SNMP errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_SNMP], 0U );

	// unit_test[25] command_receiver_test thread case CMD_PARAMETER callnum check
	BOOST_CHECK_EQUAL( vsd.call_num_map[l7vs::l7vsadm_request::CMD_PARAMETER], 10000U );
	// unit_test[26] command_receiver_test thread case CMD_PARAMETER errnum check
	BOOST_CHECK_EQUAL( error_num_map[l7vs::l7vsadm_request::CMD_PARAMETER], 0U );


	BOOST_MESSAGE( "----- thread_test end -----" );

}



test_suite*	init_unit_test_suite( int argc, char* argv[] ){
	test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

	l7vs::Logger	logger_instance;
	l7vs::Parameter	parameter_instance;
	logger_instance.loadConf();

	ts->add( BOOST_TEST_CASE( &thread_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
