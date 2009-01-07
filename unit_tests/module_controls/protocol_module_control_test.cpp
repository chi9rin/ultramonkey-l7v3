
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "protocol_module_control.h"
//#include "../../src/protocol_module_control.cpp"

#define	PM1	"test1"
#define	PM2	"test2"

class	l7vs::protocol_module_control;

using namespace boost::unit_test;

//test case1.
void	protocol_module_control_test(){
	l7vs::protocol_module_base*		protomod = NULL;
	l7vs::protocol_module_control& control = l7vs::protocol_module_control::getInstance();
	//call load_module before initialize
	try{
		protomod = control.load_module( PM1 );
	}
//	catch( l7vs::module_control_error& err ){
//		std::cout << err.message << std::endl;
//	}
	catch(...){
	}

	//test initialize and finalize
	control.initialize( "./" );
	control.finalize();

}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_control" );
	ts->add( BOOST_TEST_CASE( &protocol_module_control_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

