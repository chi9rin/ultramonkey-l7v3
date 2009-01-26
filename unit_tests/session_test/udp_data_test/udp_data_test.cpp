
#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>

#include "data_buff_base.h"

#include "../../../src/udp_data.cpp"


using namespace boost::unit_test_framework;

//--test class--
class	udp_data_test : public l7vs::udp_data {
	public:
		udp_data_test(){
		}
		~udp_data_test(){
		}
};

//--test case--
void	constractor_test(){

	BOOST_MESSAGE( "----- constractor test start -----" );
	
	udp_data_test test_obj;

	// ## constractor test [1]  initialize member test "endpoint_info"
	std::cout << "constractor test [1]  member initialize test \"endpoint_info\"" << std::endl;
	BOOST_CHECK_EQUAL( test_obj.get_endpoint(), boost::asio::ip::udp::endpoint());

	BOOST_MESSAGE( "----- constractor test end -----" );
}

void	endpoint_function_test(){
	BOOST_MESSAGE( "----- endpoint setter & getter test start -----" );

	udp_data_test test_obj;
	
	boost::asio::ip::udp::endpoint ref_endpoint;
	boost::asio::ip::udp::endpoint chk_endpoint;
	
	// ## endpoint setter & getter test [1] ip=\"0.0.0.0\" port=0
	std::cout << "endpoint setter & getter test [1] ip=\"0.0.0.0\" port=0" << std::endl;
	ref_endpoint.address(boost::asio::ip::address::from_string("0.0.0.0"));
	ref_endpoint.port(0);
	test_obj.set_endpoint(ref_endpoint);
	chk_endpoint = test_obj.get_endpoint();
	BOOST_CHECK_EQUAL( ref_endpoint, chk_endpoint );

	// ## endpoint setter & getter test [2] ip=\"255.255.255.255\" port=USHRT_MAX
	std::cout << "endpoint setter & getter test [2] ip=\"255.255.255.255\" port=USHRT_MAX" << std::endl;
	ref_endpoint.address(boost::asio::ip::address::from_string("255.255.255.255"));
	ref_endpoint.port(USHRT_MAX);
	test_obj.set_endpoint(ref_endpoint);
	chk_endpoint = test_obj.get_endpoint();
	BOOST_CHECK_EQUAL( ref_endpoint, chk_endpoint );
	// ## endpoint setter & getter test [3] ip=\"128.128.128.128\" port=USHRT_MAX / 2
	std::cout << "endpoint setter & getter test [3] ip=\"128.128.128.128\" port=USHRT_MAX / 2" << std::endl;
	ref_endpoint.address(boost::asio::ip::address::from_string("128.128.128.128"));
	ref_endpoint.port(USHRT_MAX / 2);
	test_obj.set_endpoint(ref_endpoint);
	chk_endpoint = test_obj.get_endpoint();
	BOOST_CHECK_EQUAL( ref_endpoint, chk_endpoint );

	BOOST_MESSAGE( "----- endpoint setter & getter test end -----" );
}

void	initialize_test(){

	BOOST_MESSAGE( "----- initialize test start -----" );

	udp_data_test test_obj;
	
	// ## initialize test [1]  initialize member test "endpoint_info"
	std::cout << "initialize test [1]  initialize member test \"endpoint_info\"" << std::endl;
	
	boost::asio::ip::udp::endpoint set_endpoint;
	boost::asio::ip::udp::endpoint chk_endpoint;
	
	set_endpoint.address(boost::asio::ip::address::from_string("255.255.255.255"));
	set_endpoint.port(USHRT_MAX);
	test_obj.set_endpoint(set_endpoint);
	
	chk_endpoint = test_obj.get_endpoint();
	BOOST_CHECK( boost::asio::ip::udp::endpoint() != chk_endpoint );
	
	test_obj.initialize();

	chk_endpoint = test_obj.get_endpoint();

	BOOST_CHECK_EQUAL( boost::asio::ip::udp::endpoint(), chk_endpoint );

	// ## initialize test [2] call base class initialize function"
	std::cout << "initialize test [2] call base class initialize function" << std::endl;

	test_obj.b_initialize_call_chk = false;

	test_obj.initialize();

	BOOST_CHECK( true == test_obj.b_initialize_call_chk );

	BOOST_MESSAGE( "----- initialize test end -----" );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vs::data_buff_base class test" );

	ts->add( BOOST_TEST_CASE( &constractor_test ) );
	ts->add( BOOST_TEST_CASE( &endpoint_function_test ) );
	ts->add( BOOST_TEST_CASE( &initialize_test ) );

	framework::master_test_suite().add( ts );

	return NULL;
}
