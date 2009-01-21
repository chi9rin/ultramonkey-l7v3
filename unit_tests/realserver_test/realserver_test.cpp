
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "realserver.h"

using namespace boost::unit_test;

//test case1.
void	realserver_test(){
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "realserver_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &realserver_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

