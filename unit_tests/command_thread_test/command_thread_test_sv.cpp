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
#include "test_data.h"

#include "../../src/command_receiver.cpp"
#include "../../src/command_session.cpp"

using namespace boost::unit_test_framework;

//variable
boost::asio::io_service			server_io;

//util

//--tests--

void	server_thread(){
	std::cout << "run_start" << std::endl;
	server_io.run();
	std::cout << "run_end" << std::endl;
}

void	thread_test(){
	BOOST_MESSAGE( "----- thread_test start -----" );

	l7vs::l7vsd	vsd;
	boost::shared_ptr< l7vs::replication >
									rep( new l7vs::replication( server_io ) );
	vsd.set_replication( rep );

	l7vs::command_receiver	receiver( server_io, "./l7vs", vsd );

	// server thread　start
	boost::thread	thd1( boost::bind( &server_thread ) );
	for(;;){
		sleep(1);
		std::cout << "for" << std::endl;
		if(vsd.exit_flag){
			std::cout << "break" << std::endl;
			break;
		}
	}
//	thd1.join();

	std::cout << "io_service_stop" << std::endl;
	server_io.stop();

	std::cout << "join" << std::endl;
	thd1.join();

	// check flag
	BOOST_CHECK_EQUAL( vsd.data.method_map[l7vs::l7vsadm_request::CMD_LIST].num_called, 100U );


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
