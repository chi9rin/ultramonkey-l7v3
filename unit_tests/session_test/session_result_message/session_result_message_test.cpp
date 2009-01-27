
#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>

#include "session_result_message.h"

using namespace boost::unit_test_framework;

//--test case--
void	operator_equal_test(){


	BOOST_MESSAGE( "----- operator equal test start -----" );
/*	
//	session_result_message
	data_buff_base_test test_obj;

	// ## constractor test [1]  initialize member test "data"
	std::cout << "constractor test [1]  member initialize test \"data_size\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_size(), std::size_t(0) );

	// ## constractor test [2]  initialize member test "send_size"
	std::cout << "constractor test [2]  member initialize test \"send_size\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_send_size(), std::size_t(0) );

	// ## constractor test [3]  initialize member test "data"
	std::cout << "constractor test [3]  member initialize test \"data\"" << std::endl;
	boost::array< char , MAX_BUFFER_SIZE> init_data;
	for(int i = 0;i < MAX_BUFFER_SIZE; i++)
		init_data[i] = '\0';

	boost::array< char , MAX_BUFFER_SIZE>& chk_data = test_obj.get_data();
	for(int i = 0;i < MAX_BUFFER_SIZE; i++)
		BOOST_CHECK_EQUAL( init_data[i] , chk_data[i]);
*/
	BOOST_MESSAGE( "----- operator equal test end -----" );
}

void	operator_inequal_test(){
	BOOST_MESSAGE( "----- operator equal test start -----" );

	data_buff_base_test test_obj;

	size_t set_size;
	size_t get_size;
	
	// ## data size setter & getter test [1] set min(0)
	std::cout << "data size setter & getter test [1] set min(0)" << std::endl;
	set_size = 0;
	test_obj.set_size(set_size);
	get_size = test_obj.get_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// ## data size setter & getter test [2] set max(ULONG_MAX)
	std::cout << "data size setter & getter test [2] set max(ULONG_MAX)" << std::endl;
	set_size = ULONG_MAX;
	test_obj.set_size(set_size);
	get_size = test_obj.get_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// ## data size setter & getter test [3] set mid(ULONG_MAX / 2)
	std::cout << "data size setter & getter test [3] set max(ULONG_MAX / 2)" << std::endl;
	set_size = ULONG_MAX / 2;
	test_obj.set_size(set_size);
	get_size = test_obj.get_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	BOOST_MESSAGE( "----- operator equal test end -----" );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vs::data_buff_base class test" );

	ts->add( BOOST_TEST_CASE( &operator_equal_test ) );
	ts->add( BOOST_TEST_CASE( &operator_inequal_test ) );

	framework::master_test_suite().add( ts );

	return NULL;
}
