#include <boost/test/included/unit_test.hpp>

#include "l7vsd_stub.h"
#include "command_receiver_stub.h"
#include "../../src/command_session.cpp"

using namespace boost::unit_test_framework;

class	command_session_test	:public	l7vs::command_session {
public:
	static void	constractor_test();

};

//--tests--

void	command_session_test::constractor_test(){

}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "command_session class test" );
	ts->add( BOOST_TEST_CASE( &command_session_test::constractor_test ) );


	framework::master_test_suite().add( ts );

	return 0;
}
