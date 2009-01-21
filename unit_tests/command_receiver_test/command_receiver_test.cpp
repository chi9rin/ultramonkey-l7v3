#include <boost/test/included/unit_test.hpp>

#include "l7vsd_stub.h"
#include "command_session_stub.h"

#include "../../src/command_receiver.cpp"

using namespace boost::unit_test_framework;

class	command_receiver_test	:public	l7vs::command_receiver {
public:
	static void	constractor_test();

};

//--tests--

void	command_receiver_test::constractor_test(){

}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "command_receiver class test" );
	ts->add( BOOST_TEST_CASE( &command_receiver_test::constractor_test ) );


	framework::master_test_suite().add( ts );

	return 0;
}
