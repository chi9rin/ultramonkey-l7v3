
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "virtualservice.h"

using namespace boost::unit_test;

//test case1.
void	virtualservice_tcp_test(){
}

//test case2
void	virtualservice_udp_test(){
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "virtualservice_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_udp_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

