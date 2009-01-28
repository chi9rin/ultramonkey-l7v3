
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "../../include/virtualservice.h"

using namespace boost::unit_test;

//test case1.
void	virtualservice_base_test1(){
	// unit_test[1]  object create
	BOOST_MESSAGE( "-------1" );
	
	// unit_test[2]  object destroy
	BOOST_MESSAGE( "-------2" );
}

//test case2
void	virtualservice_base_test2(){
}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "virtualservice_base_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &virtualservice_base_test1 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_base_test2 ) );

	framework::master_test_suite().add( ts );

	return 0;
}

