#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>

#include "protocol_module_base.h"
#include "logger_enum.h"

using namespace boost::unit_test_framework;

//--test class--

class	protocol_module_base_test : public l7vs::protocol_module_base {
public:
	protocol_module_base_test( std::string in_modulename ) : protocol_module_base( in_modulename ){};
	~protocol_module_base_test();
};


//--tests--

void	constractor_test(){

	BOOST_MESSAGE( "----- constractor test start -----" );

	protocol_module_base_test	prt_mod_base_test_1( "cinsert" );
	protocol_module_base_test	prt_mod_base_test_2( "" );
	protocol_module_base_test	prt_mod_base_test_3( NULL );

	// ## test [1]  constractor parameter set test ("cinsert")
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( prt_mod_base_test_1.name, "cinsert" );

	// ## test [2]  constractor parameter set test ("")
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( prt_mod_base_test_2.name, "" );

	// ## test [3]  constractor parameter set test (null)
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( prt_mod_base_test_3.name, NULL );

	BOOST_MESSAGE( "----- constractor test end -----" );
}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_base class test" );

	ts->add( BOOST_TEST_CASE( &constractor_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

