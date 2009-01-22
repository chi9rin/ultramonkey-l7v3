#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "realserver.h"

class	l7vs::realserver;

using namespace boost::unit_test;

//test case1.
void	realserver_test(){
	// unit_test[1]  $B%3%s%9%H%i%/%?$N%F%9%H(B
	l7vs::realserver	server1;
	// unit_test[2]  $B%3%T!<%3%s%9%H%i%/%?$N%F%9%H(B
	l7vs::realserver	server2( server1 );

	// unit_test[3]  $BHf3S%*%Z%l!<%?$N%F%9%H(B
	BOOST_CHECK( server1 == server2 );

	// unit_test[4]  $BH]Dj%*%Z%l!<%?$N%F%9%H(B
	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.87" ), 22 ) ;

	BOOST_CHECK( server1 != server2 );

	// unit_test[5]  $B%3%T!<%*%Z%l!<%?$N%F%9%H(B
	server2 = server1 ;

	BOOST_CHECK( server1 == server2 );

	// unit_test[6]  $BBg>.Hf3S%*%Z%l!<%?$N%F%9%H(B
	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

	BOOST_CHECK( server1 < server2 );

	// unit_test[7]  $B@\B3?t<hF@%a%=%C%I$N%F%9%H(B
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[8]  $B@ZCG?t<hF@%a%=%C%I$N%F%9%H(B
	BOOST_CHECK_EQUAL( server1.get_inact(), 0 );

	// unit_test[9]  $B@\B3?t%$%s%/%j%a%s%H%a%=%C%I$N%F%9%H(B
	server1.increment_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 1 );

	// unit_test[10]  $B@\B3?t%G%/%j%a%s%H%a%=%C%I$N%F%9%H(B
	server1.decrement_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[11]  $B@ZCG?t%$%s%/%j%a%s%H%a%=%C%I$N%F%9%H(B
	server1.increment_inact();
	BOOST_CHECK_EQUAL( server1.get_inact(), 1 );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "realserver_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &realserver_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

