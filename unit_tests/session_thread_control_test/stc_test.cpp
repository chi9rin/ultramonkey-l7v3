
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include "session_thread_control.h"

using namespace boost::unit_test;

//test case1.
void	stc_test(){
	boost::shared_ptr<l7vs::tcp_session>	session( new l7vs::tcp_session );
	boost::shared_ptr<l7vs::session_thread_control>	stc( new l7vs::session_thread_control( session ) );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "stc_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &stc_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

