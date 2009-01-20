#define	TEST_CASE

#include <boost/test/unit_test.hpp>

#include "logger_enum.h"
#include "logger_stub.h"
#include "parameter_enum.h"
#include "parameter_stub.h"
#include "command_receiver_stub.h"
#include "virtualservice_stub.h"
#include "virtualservice_element_stub.h"
#include "realserver_stub.h"
#include "realserver_element_stub.h"
#include "replication_stub.h"
#include "snmpbridge_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"

#include "../../src/l7vsd.cpp"

using namespace boost::unit_test_framework;

class	l7vsd_test	:public	l7vs::l7vsd {
public:
	static void	constractor_test();

};

void	l7vsd_test::constractor_test(){

}

test_suite*
init_unit_test_suite( int argc, char* argv[] ){
	test_suite* test = BOOST_TEST_SUITE( "l7vsd class test" );
	test->add( BOOST_TEST_CASE( l7vsd_test::constractor_test ) );

	return test;
}
