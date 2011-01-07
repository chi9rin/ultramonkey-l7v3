
#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>

#include "../logger_stb/logger.h"
#include "../../../src/tcp_data.cpp"

using namespace boost::unit_test_framework;

//--test class--
class	tcp_data_test : public l7vs::tcp_data {
	public:
		tcp_data_test(){
		}
		~tcp_data_test(){
		}
};

//--test case--
void	constractor_test(){

	BOOST_MESSAGE( "----- constractor test start -----" );
	
	tcp_data_test test_obj;

	// unit_test [1] constractor test  initialize member test "endpoint_info"
	std::cout << "[1] constractor test  initialize member test \"endpoint_info\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_endpoint(), boost::asio::ip::tcp::endpoint());

	// unit_test [2] constractor test initialize member test "data"
	std::cout << "[2] constractor test initialize member test \"data_size\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_size(), std::size_t(0) );

	// unit_test [3] constractor test initialize member test "send_size"
	std::cout << "[3] constractor test initialize member test \"send_size\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_send_size(), std::size_t(0) );

	// unit_test [4] constractor test initialize member test "data"
	std::cout << "[4] constractor test initialize member test \"data\"" << std::endl;
	boost::array< char , MAX_BUFFER_SIZE> init_data;
	for(int i = 0;i < MAX_BUFFER_SIZE; i++)
		init_data[i] = '\0';

	boost::array< char , MAX_BUFFER_SIZE>& chk_data = test_obj.get_data();
	for(int i = 0;i < MAX_BUFFER_SIZE; i++)
		BOOST_CHECK_EQUAL( init_data[i] , chk_data[i]);

	BOOST_MESSAGE( "----- constractor test end -----" );
}

void	endpoint_function_test(){
	BOOST_MESSAGE( "----- endpoint setter & getter test start -----" );

	tcp_data_test test_obj;
	
	boost::asio::ip::tcp::endpoint ref_endpoint;
	boost::asio::ip::tcp::endpoint chk_endpoint;
	
	// unit_test [1] endpoint setter & getter test ip=\"0.0.0.0\" port=0
	std::cout << "[1] endpoint setter & getter test ip=\"0.0.0.0\" port=0" << std::endl;
	ref_endpoint.address(boost::asio::ip::address::from_string("0.0.0.0"));
	ref_endpoint.port(0);
	test_obj.set_endpoint(ref_endpoint);
	chk_endpoint = test_obj.get_endpoint();
	BOOST_CHECK_EQUAL( ref_endpoint, chk_endpoint );

	// unit_test [2] endpoint setter & getter test ip=\"255.255.255.255\" port=USHRT_MAX
	std::cout << "[2] endpoint setter & getter test ip=\"255.255.255.255\" port=USHRT_MAX" << std::endl;
	ref_endpoint.address(boost::asio::ip::address::from_string("255.255.255.255"));
	ref_endpoint.port(USHRT_MAX);
	test_obj.set_endpoint(ref_endpoint);
	chk_endpoint = test_obj.get_endpoint();
	BOOST_CHECK_EQUAL( ref_endpoint, chk_endpoint );

	// unit_test [3] endpoint setter & getter test ip=\"128.128.128.128\" port=USHRT_MAX / 2
	std::cout << "[3] endpoint setter & getter test ip=\"128.128.128.128\" port=USHRT_MAX / 2" << std::endl;
	ref_endpoint.address(boost::asio::ip::address::from_string("128.128.128.128"));
	ref_endpoint.port(USHRT_MAX / 2);
	test_obj.set_endpoint(ref_endpoint);
	chk_endpoint = test_obj.get_endpoint();
	BOOST_CHECK_EQUAL( ref_endpoint, chk_endpoint );

	BOOST_MESSAGE( "----- endpoint setter & getter test end -----" );
}

void	initialize_test(){

	BOOST_MESSAGE( "----- initialize test start -----" );

	tcp_data_test test_obj;
	
	// unit_test [1] initialize test initialize member test "endpoint_info"
	std::cout << "[1] initialize test initialize member test \"endpoint_info\"" << std::endl;
	
	boost::asio::ip::tcp::endpoint set_endpoint;
	boost::asio::ip::tcp::endpoint chk_endpoint;
	
	set_endpoint.address(boost::asio::ip::address::from_string("255.255.255.255"));
	set_endpoint.port(USHRT_MAX);
	test_obj.set_endpoint(set_endpoint);
	
	chk_endpoint = test_obj.get_endpoint();
	BOOST_CHECK( boost::asio::ip::tcp::endpoint() != chk_endpoint );
	
	test_obj.initialize();

	chk_endpoint = test_obj.get_endpoint();

	BOOST_CHECK_EQUAL( boost::asio::ip::tcp::endpoint(), chk_endpoint );


	size_t ref_size = ULONG_MAX;
	boost::array< char , MAX_BUFFER_SIZE> ref_data;
	for(int i = 0;i < MAX_BUFFER_SIZE; i++){
		ref_data[i] = CHAR_MAX;
	}
	
	test_obj.set_size(ref_size);
	test_obj.set_send_size(ref_size);
	test_obj.set_data(ref_data);

	test_obj.initialize();

	// unit_test [2] initialize test initialize member test "data_size"
	std::cout << "[2] initialize test initialize member test \"data_size\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_size(), std::size_t(0) );

	// unit_test [3] initialize test initialize member test "send_size"
	std::cout << "[3] initialize test initialize member test \"send_size\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_send_size(), std::size_t(0) );

	// unit_test [4] initialize test initialize member test "data"
	std::cout << "[4] initialize test initialize member test \"data\"" << std::endl;
	boost::array< char , MAX_BUFFER_SIZE> init_data;
	for(int i = 0;i < MAX_BUFFER_SIZE; i++)
		init_data[i] = '\0';
	boost::array< char , MAX_BUFFER_SIZE>& chk_data = test_obj.get_data();
	for(int i = 0;i < MAX_BUFFER_SIZE; i++)
		BOOST_CHECK_EQUAL( init_data[i] , chk_data[i]);

	BOOST_MESSAGE( "----- initialize test end -----" );
}

void	data_size_function_test(){
	BOOST_MESSAGE( "----- data size setter & getter test start -----" );

	tcp_data_test test_obj;

	size_t set_size;
	size_t get_size;
	
	// unit_test [1] data size setter & getter test set min(0)
	std::cout << "[1] data size setter & getter test set min(0)" << std::endl;
	set_size = 0;
	test_obj.set_size(set_size);
	get_size = test_obj.get_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// unit_test [2] data size setter & getter test set max(MAX_BUFFER_SIZE)
	std::cout << "[2] data size setter & getter test set max(MAX_BUFFER_SIZE)" << std::endl;
	set_size = MAX_BUFFER_SIZE;
	test_obj.set_size(set_size);
	get_size = test_obj.get_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// unit_test [3] data size setter & getter test set mid(MAX_BUFFER_SIZE / 2)
	std::cout << "[3] data size setter & getter test set mid(MAX_BUFFER_SIZE / 2)" << std::endl;
	set_size = MAX_BUFFER_SIZE / 2;
	test_obj.set_size(set_size);
	get_size = test_obj.get_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// unit_test [4] data size setter & getter test set over max(MAX_BUFFER_SIZE + 1)
	std::cout << "[4] data size setter & getter test set over max(MAX_BUFFER_SIZE + 1)" << std::endl;
	set_size = MAX_BUFFER_SIZE + 1;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.set_size(set_size);
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0001,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	get_size = test_obj.get_size();
	BOOST_CHECK( MAX_BUFFER_SIZE == get_size );

	BOOST_MESSAGE( "----- data size setter & getter test end -----" );
}

void	send_size_function_test(){
	BOOST_MESSAGE( "----- send size setter & getter test start -----" );

	tcp_data_test test_obj;
	
	size_t set_size;
	size_t get_size;
	
	// unit_test [1] send size setter & getter test set min(0)
	std::cout << "[1] send size setter & getter test set min(0)" << std::endl;
	set_size = 0;
	test_obj.set_send_size(set_size);
	get_size = test_obj.get_send_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// unit_test [2] send size setter & getter test set max(MAX_BUFFER_SIZE)
	std::cout << "[2] send size setter & getter test set max(MAX_BUFFER_SIZE)" << std::endl;
	set_size = MAX_BUFFER_SIZE;
	test_obj.set_send_size(set_size);
	get_size = test_obj.get_send_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// unit_test [3] send size setter & getter test set mid(MAX_BUFFER_SIZE / 2)
	std::cout << "[3] send size setter & getter test set mid(MAX_BUFFER_SIZE / 2)" << std::endl;
	set_size = MAX_BUFFER_SIZE / 2;
	test_obj.set_send_size(set_size);
	get_size = test_obj.get_send_size();
	BOOST_CHECK_EQUAL( set_size, get_size );

	// unit_test [4] send size setter & getter test set over max(MAX_BUFFER_SIZE + 1)
	std::cout << "[4] send size setter & getter test set over max(MAX_BUFFER_SIZE + 1)" << std::endl;
	set_size = MAX_BUFFER_SIZE + 1;
	l7vs::Logger::putLogError_category = l7vs::LOG_CAT_NONE;
	l7vs::Logger::putLogError_id = 0;
	test_obj.set_send_size(set_size);
	BOOST_CHECK_EQUAL(l7vs::LOG_CAT_L7VSD_SESSION,l7vs::Logger::putLogError_category);
	BOOST_CHECK_EQUAL(0002,l7vs::Logger::putLogError_id);
	std::cout << l7vs::Logger::putLogError_message << std::endl;
	get_size = test_obj.get_send_size();
	BOOST_CHECK( MAX_BUFFER_SIZE == get_size );


	BOOST_MESSAGE( "----- send size setter & getter test end -----" );
}

void	data_function_test(){
	BOOST_MESSAGE( "----- data setter & getter test start -----" );

	tcp_data_test test_obj;

	// unit_test [1] data setter & getter test
	std::cout << "[1] data setter & getter test" << std::endl;
	boost::array< char , MAX_BUFFER_SIZE> ref_data;
			
	for(int set_char = CHAR_MIN;set_char <= CHAR_MAX; set_char++){
//		std::cout << "char code[";
//		std::cout << set_char;
//		std::cout << "] test" << std::endl;
		
		for(int i = 0;i < MAX_BUFFER_SIZE; i++)
			ref_data[i] = set_char;

		test_obj.set_data(ref_data);
		boost::array< char , MAX_BUFFER_SIZE>& chk_data = test_obj.get_data();
		
		for(int i = 0;i < MAX_BUFFER_SIZE; i++)
			BOOST_CHECK_EQUAL(ref_data[i],chk_data[i]);
	}

	BOOST_MESSAGE( "----- data setter & getter test end -----" );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vs::tcp_data class test" );

	ts->add( BOOST_TEST_CASE( &constractor_test ) );
	ts->add( BOOST_TEST_CASE( &initialize_test ) );
	ts->add( BOOST_TEST_CASE( &endpoint_function_test ) );

	ts->add( BOOST_TEST_CASE( &data_size_function_test ) );
	ts->add( BOOST_TEST_CASE( &send_size_function_test ) );
	ts->add( BOOST_TEST_CASE( &data_function_test ) );



	framework::master_test_suite().add( ts );

	return NULL;
}
