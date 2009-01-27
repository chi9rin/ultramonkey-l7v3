#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include "module_base.h"

using namespace boost::unit_test_framework;
using namespace l7vs;

//--stub functions--
LOG_LEVEL_TAG	stb_getloglevel(){
	std::cout << "getloglevel called." << std::endl;
	return LOG_LV_NONE;
}
void	stb_putLogFatal( const unsigned int, const std::string&, const char*, int ){
	std::cout << "putLogFatal called." << std::endl;
}
void	stb_putLogError( const unsigned int, const std::string&, const char*, int ){
	std::cout << "putLogError called." << std::endl;
}
void	stb_putLogWarn( const unsigned int, const std::string&, const char*, int ){
	std::cout << "putLogWarn called." << std::endl;
}
void	stb_putLogInfo( const unsigned int, const std::string&, const char*, int ){
	std::cout << "putLogInfo called." << std::endl;
}
void	stb_putLogDebug( const unsigned int, const std::string&, const char*, int ){
	std::cout << "putLogDebug called." << std::endl;
}
void*	stb_replication_pay_memory( const std::string&, unsigned int* ){
	std::cout << "replication_pay_memory called." << std::endl;
	return NULL;
}
void	stb_replication_area_lock(){
	std::cout << "replication_area_lock called." << std::endl;
}
void	stb_replication_area_unlock(){
	std::cout << "replication_area_unlock called." << std::endl;
}


//--test class--
class	module_base_test : public module_base {
public:
module_base_test( std::string in_modulename ) : module_base( in_modulename ){}
~module_base_test(){}
bool	is_tcp(){ return true; }
bool	is_udp(){ return true; }
void	replication_interrupt(){}

void	constractor_test(){

	BOOST_MESSAGE( "----- constractor test start -----" );

	std::string module_name_1 = "cinsert";
	std::string module_name_2 = "";

	module_base_test	module_base_test_1( module_name_1 );
	module_base_test	module_base_test_2( module_name_2 );

	// ## test [1]  constractor parameter set test ("cinsert")
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( module_base_test_1.name, module_name_1 );

	// ## test [2]  constractor parameter set test ("")
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( module_base_test_2.name, module_name_2 );

	BOOST_MESSAGE( "----- constractor test end -----" );
}

void	get_name_test(){

	BOOST_MESSAGE( "----- get_name test start -----" );

	std::string module_name_1 = "cinsert";
	std::string module_name_2 = "";

	module_base_test	module_base_test_1( module_name_1 );
	module_base_test	module_base_test_2( module_name_2 );

	// ## test [1]  get_name get test ("cinsert")
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( module_base_test_1.get_name(), module_name_1 );

	// ## test [2]  get_name get test ("cinsert")
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( module_base_test_1.get_name(), module_base_test_1.name );

	// ## test [3]  get_name get test ("")
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( module_base_test_2.get_name(), module_name_2 );

	// ## test [4]  get_name get test ("")
	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( module_base_test_2.get_name(), module_base_test_2.name );

	BOOST_MESSAGE( "----- get_name test end -----" );
}

void	init_logger_functions_test(
							getloglevel_func_type	ingetloglevel,
							logger_func_type		inputLogFatal,
							logger_func_type		inputLogError,
							logger_func_type		inputLogWarn,
							logger_func_type		inputLogInfo,
							logger_func_type		inputLogDebug ){

	BOOST_MESSAGE( "----- init_logger_functions test start -----" );

	module_base_test	module_base_test_1( "cinsert" );

	std::cout << "0----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.getloglevel	== NULL );
	BOOST_CHECK( module_base_test_1.putLogFatal	== NULL );
	BOOST_CHECK( module_base_test_1.putLogError	== NULL );
	BOOST_CHECK( module_base_test_1.putLogWarn	== NULL );
	BOOST_CHECK( module_base_test_1.putLogInfo	== NULL );
	BOOST_CHECK( module_base_test_1.putLogDebug	== NULL );

	module_base_test_1.init_logger_functions(
												ingetloglevel,
												inputLogFatal,
												inputLogError,
												inputLogWarn,
												inputLogInfo,
												inputLogDebug );

	// ## test [1]  init_logger_functions call test (getloglevel)
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.getloglevel != NULL );
	module_base_test_1.getloglevel();

	// ## test [2]  init_logger_functions call test (putLogFatal)
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.putLogFatal != NULL );
	module_base_test_1.putLogFatal( 0, "", NULL, 0 );

	// ## test [3]  init_logger_functions call test (putLogError)
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.putLogError != NULL );
	module_base_test_1.putLogError( 0, "", NULL, 0 );

	// ## test [4]  init_logger_functions call test (putLogWarn)
	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.putLogWarn != NULL );
	module_base_test_1.putLogWarn( 0, "", NULL, 0 );

	// ## test [5]  init_logger_functions call test (putLogInfo)
	std::cout << "5----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.putLogInfo != NULL );
	module_base_test_1.putLogInfo( 0, "", NULL, 0 );

	// ## test [6]  init_logger_functions call test (putLogDebug)
	std::cout << "6----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.putLogDebug != NULL );
	module_base_test_1.putLogDebug( 0, "", NULL, 0 );

	BOOST_MESSAGE( "----- init_logger_functions test end -----" );
}

void	init_replication_functions_test(
							replicationpaymemory_func_type	inreplication_pay_memory,
							boost::function< void( void ) >	inreplication_area_lock,
							boost::function< void( void ) >	inreplication_area_unlock,
							const boost::asio::ip::tcp::endpoint&	invirtual_service_endpoint_tcp,
							const boost::asio::ip::udp::endpoint&	invirtual_service_endpoint_udp	){

	BOOST_MESSAGE( "----- init_replication_functions test start -----" );

	module_base_test	module_base_test_1( "cinsert" );

	std::cout << "0----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.replication_pay_memory	== NULL );
	BOOST_CHECK( module_base_test_1.replication_area_lock	== NULL );
	BOOST_CHECK( module_base_test_1.replication_area_unlock	== NULL );

	module_base_test_1.init_replication_functions(
												inreplication_pay_memory,
												inreplication_area_lock,
												inreplication_area_unlock,
												invirtual_service_endpoint_tcp,
												invirtual_service_endpoint_udp );

	// ## test [1]  init_replication_functions parameter set test (replication_pay_memory)
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.replication_pay_memory	!= NULL );
	module_base_test_1.replication_pay_memory( "", NULL );

	// ## test [1]  init_replication_functions call test (replication_area_lock)
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.replication_area_lock	!= NULL );
	module_base_test_1.replication_area_lock();

	// ## test [1]  init_replication_functions call test (replication_area_unlock)
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK( module_base_test_1.replication_area_unlock	!= NULL );
	module_base_test_1.replication_area_unlock();

	// ## test [1]  init_replication_functions parameter set test (virtual_service_endpoint_tcp)
	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( module_base_test_1.virtual_service_endpoint_tcp, invirtual_service_endpoint_tcp );

	// ## test [1]  init_replication_functions parameter set test (virtual_service_endpoint_udp)
	std::cout << "5----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( module_base_test_1.virtual_service_endpoint_udp, invirtual_service_endpoint_udp );

	BOOST_MESSAGE( "----- init_replication_functions test end -----" );
}

};

//--test functions--
void	constractor_test(){

	module_base_test	module_base_test_1( "cinsert" );
	module_base_test_1.constractor_test();

}

void	get_name_test(){

	module_base_test	module_base_test_1( "cinsert" );
	module_base_test_1.get_name_test();

}

void	init_logger_functions_test(){

	boost::function< LOG_LEVEL_TAG(void) >	getloglevel = &stb_getloglevel;
	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogFatal	= &stb_putLogFatal;
	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogError	= &stb_putLogError;
	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogWarn		= &stb_putLogWarn;
	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogInfo		= &stb_putLogInfo;
	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogDebug	= &stb_putLogDebug;

	module_base_test	module_base_test_1( "cinsert" );
	module_base_test_1.init_logger_functions_test(	getloglevel,
													putLogFatal,
													putLogError,
													putLogWarn,
													putLogInfo,
													putLogDebug);

}

void	init_replication_functions_test(){

	boost::function< void* ( const std::string&, unsigned int* ) >	replication_pay_memory	=	&stb_replication_pay_memory;
	boost::function< void( void ) >	replication_area_lock	=	&stb_replication_area_lock;
	boost::function< void( void ) >	replication_area_unlock	=	&stb_replication_area_unlock;
	boost::asio::ip::address	address_1;
	unsigned short				port_1 = 1111;
	boost::asio::ip::address	address_2;
	unsigned short				port_2 = 2222;
	boost::asio::ip::tcp::endpoint virtual_service_endpoint_tcp( address_1, port_1 );
	boost::asio::ip::udp::endpoint virtual_service_endpoint_udp( address_2, port_2 );

	module_base_test	module_base_test_1( "cinsert" );
	module_base_test_1.init_replication_functions_test(
													replication_pay_memory,
													replication_area_lock,
													replication_area_unlock,
													virtual_service_endpoint_tcp,
													virtual_service_endpoint_udp );

}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "module_base class test" );

	ts->add( BOOST_TEST_CASE( &constractor_test ) );
	ts->add( BOOST_TEST_CASE( &get_name_test ) );
	ts->add( BOOST_TEST_CASE( &init_logger_functions_test ) );
	ts->add( BOOST_TEST_CASE( &init_replication_functions_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
