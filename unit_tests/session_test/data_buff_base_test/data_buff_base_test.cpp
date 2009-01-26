
#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>

#include "data_buff_base.h"

using namespace boost::unit_test_framework;

//--test class--
class	data_buff_base_test : public l7vs::data_buff_base {
	public:
		data_buff_base_test(){
		}
		~data_buff_base_test(){
		}
};


//--test case--
void	constractor_test(){

	BOOST_MESSAGE( "----- constractor test start -----" );
	
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

	BOOST_MESSAGE( "----- constractor test end -----" );
}

void	data_size_function_test(){
	BOOST_MESSAGE( "----- data size setter & getter test start -----" );

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

	BOOST_MESSAGE( "----- data size setter & getter test end -----" );
}

void	send_size_function_test(){
	BOOST_MESSAGE( "----- send size setter & getter test start -----" );

	data_buff_base_test test_obj;
	
	size_t set_size;
	size_t get_size;
	
	// ## send size setter & getter test [1] set min(0)
	std::cout << "send size setter & getter test [1] set min(0)" << std::endl;
	set_size = 0;
	test_obj.set_send_size(set_size);
	get_size = test_obj.get_send_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// ## send size setter & getter test [2] set max(ULONG_MAX)
	std::cout << "send size setter & getter test [2] set max(ULONG_MAX)" << std::endl;
	set_size = ULONG_MAX;
	test_obj.set_send_size(set_size);
	get_size = test_obj.get_send_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// ## send size setter & getter test [3] set mid(ULONG_MAX / 2)
	std::cout << "send size setter & getter test [3] set max(ULONG_MAX / 2)" << std::endl;
	set_size = ULONG_MAX / 2;
	test_obj.set_send_size(set_size);
	get_size = test_obj.get_send_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	BOOST_MESSAGE( "----- send size setter & getter test end -----" );
}

void	data_function_test(){
	BOOST_MESSAGE( "----- data setter & getter test start -----" );

	data_buff_base_test test_obj;

	// ## data setter & getter test [1]
	boost::array< char , MAX_BUFFER_SIZE> ref_data;
			
	for(int set_char = CHAR_MIN;set_char <= CHAR_MAX; set_char++){
		std::cout << "data setter & getter test [1] char code[";
		std::cout << set_char;
		std::cout << "] test" << std::endl;
		
		for(int i = 0;i < MAX_BUFFER_SIZE; i++)
			ref_data[i] = set_char;

		test_obj.set_data(ref_data);
		boost::array< char , MAX_BUFFER_SIZE>& chk_data = test_obj.get_data();
		
		for(int i = 0;i < MAX_BUFFER_SIZE; i++)
			BOOST_CHECK_EQUAL(ref_data[i],chk_data[i]);
	}

	BOOST_MESSAGE( "----- data setter & getter test end -----" );
}

void	initialize_test(){

	BOOST_MESSAGE( "----- initialize test start -----" );

	data_buff_base_test test_obj;
	size_t ref_size = ULONG_MAX;
	boost::array< char , MAX_BUFFER_SIZE> ref_data;
	for(int i = 0;i < MAX_BUFFER_SIZE; i++){
		ref_data[i] = CHAR_MAX;
	}
	
	test_obj.set_size(ref_size);
	test_obj.set_send_size(ref_size);
	test_obj.set_data(ref_data);

	test_obj.initialize();

	// ## initialize test [1]  initialize member test "data_size"
	std::cout << "initialize test [1]  initialize member test \"data_size\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_size(), std::size_t(0) );

	// ## initialize test [2]  initialize member test "send_size"
	std::cout << "initialize test [2]  initialize member test \"send_size\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_send_size(), std::size_t(0) );

	// ## initialize test [3]  initialize member test "data"
	std::cout << "initialize test [3]  initialize member test \"data\"" << std::endl;
	boost::array< char , MAX_BUFFER_SIZE> init_data;
	for(int i = 0;i < MAX_BUFFER_SIZE; i++)
		init_data[i] = '\0';
	boost::array< char , MAX_BUFFER_SIZE>& chk_data = test_obj.get_data();
	for(int i = 0;i < MAX_BUFFER_SIZE; i++)
		BOOST_CHECK_EQUAL( init_data[i] , chk_data[i]);

	BOOST_MESSAGE( "----- initialize test end -----" );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vs::data_buff_base class test" );

	ts->add( BOOST_TEST_CASE( &constractor_test ) );
	ts->add( BOOST_TEST_CASE( &data_size_function_test ) );
	ts->add( BOOST_TEST_CASE( &send_size_function_test ) );
	ts->add( BOOST_TEST_CASE( &data_function_test ) )
			
			;
	ts->add( BOOST_TEST_CASE( &initialize_test ) );

	framework::master_test_suite().add( ts );

	return NULL;
}
