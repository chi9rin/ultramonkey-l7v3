#define	UNIT_TEST

#include <boost/test/included/unit_test.hpp>

//#include "error_code.h"

#include "logger.h"
#include "parameter.h"

#include "../../src/l7vsadm.cpp"

using namespace boost::unit_test_framework;


// test class


//--tests--
void	list_virtual_service_test(){
	BOOST_MESSAGE( "----- list_virtual_service_test start -----" );


	BOOST_MESSAGE( "----- list_virtual_service_test end -----" );

}




test_suite*	init_unit_test_suite( int argc, char* argv[] ){
	test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

	l7vs::Logger	logger_instance;
	l7vs::Parameter	parameter_instance;
	logger_instance.loadConf();

	ts->add( BOOST_TEST_CASE( &list_virtual_service_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
