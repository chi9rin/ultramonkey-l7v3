
#include <boost/test/included/unit_test.hpp>
#include "protocol_module_control.h"
//#include "../../src/protocol_module_control.cpp"

class	l7vs::protocol_module_control;

using namespace boost::unit_test;

//test case1.
void	protocol_module_control_test(){
	l7vs::protocol_module_control& control = l7vs::protocol_module_control::getInstance();
	control.initialize( "./" );
	control.finalize();
	control.initialize( NULL );

}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_control" );
	ts->add( BOOST_TEST_CASE( &protocol_module_control_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

