
#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>

#include "tcp_thread_message.h"

using namespace boost::unit_test_framework;

//--test case--
void	construcor_test(){

	
	BOOST_MESSAGE( "----- construcor test start -----" );
	
	l7vs::tcp_thread_message test;

	BOOST_MESSAGE( "----- construcor test end -----" );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vs::data_buff_base class test" );

	ts->add( BOOST_TEST_CASE( &construcor_test ) );

	framework::master_test_suite().add( ts );

	return NULL;
}
