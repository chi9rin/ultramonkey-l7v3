#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>

#include "module_base.h"
#include "logger_enum.h"

using namespace boost::unit_test_framework;


//--stub functions--

LOG_LEVEL_TAG	getloglevel(){ return LOG_LV_NONE; }
void	putLogFatal( const unsigned int, const std::string&, const char*, int ){}
void	putLogError( const unsigned int, const std::string&, const char*, int ){}
void	putLogWarn( const unsigned int, const std::string&, const char*, int ){}
void	putLogInfo( const unsigned int, const std::string&, const char*, int ){}
void	putLogDebug( const unsigned int, const std::string&, const char*, int ){}
void*	replication_pay_memory( const std::string&, unsigned int* ){ return NULL; }
void	lock_func(){}
void	unlock_func(){}

//--test class--

class	module_base_test : public l7vs::module_base {
public:
	module_base_test( std::string in_modulename ) : module_base( in_modulename ){};
	~module_base_test();
};


//--tests--

void	constractor_test(){

	BOOST_MESSAGE( "----- constractor test start -----" );

	module_base_test	mod_base_test_1( "cinsert" );
	module_base_test	mod_base_test_2( "" );
	module_base_test	mod_base_test_3( NULL );

	// ## test [1]  constractor parameter set test ("cinsert")
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_1.name, "cinsert" );

	// ## test [2]  constractor parameter set test ("")
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_2.name, "" );

	// ## test [3]  constractor parameter set test (null)
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_3.name, NULL );

	BOOST_MESSAGE( "----- constractor test end -----" );
}

void	get_name_test(){

	BOOST_MESSAGE( "----- get_name test start -----" );

	module_base_test	mod_base_test_1( "cinsert" );
	module_base_test	mod_base_test_2( "" );
	module_base_test	mod_base_test_3( NULL );

	// ## test [1]  get_name get test ("cinsert")
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_1.get_name(), "cinsert" );

	// ## test [2]  get_name get test ("cinsert")
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_1.get_name(), mod_base_test_1.name );

	// ## test [3]  get_name get test ("")
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_2.get_name(), "" );

	// ## test [4]  get_name get test ("")
	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_2.get_name(), mod_base_test_2.name );

	// ## test [5]  get_name get test (null)
	std::cout << "5----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_3.get_name(), NULL );

	// ## test [6]  get_name get test (null)
	std::cout << "6----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_3.get_name(), mod_base_test_3.name );

	BOOST_MESSAGE( "----- get_name test end -----" );
}

void	init_logger_functions_test(){

	BOOST_MESSAGE( "----- init_logger_functions test start -----" );

	module_base_test	mod_base_test( "cinsert" );

	getloglevel_func_type	getloglevel	= &getloglevel;
	logger_func_type		putLogFatal	= &putLogFatal;
	logger_func_type		putLogError	= &putLogError;
	logger_func_type		putLogWarn	= &putLogWarn;
	logger_func_type		putLogInfo	= &putLogInfo;
	logger_func_type		putLogDebug	= &putLogDebug;

	mod_base_test.init_logger_functions(
											getloglevel,
											putLogFatal,
											putLogError,
											putLogWarn,
											putLogInfo,
											putLogDebug );

	// ## test [1]  init_logger_functions parameter set test (getloglevel)
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.getloglevel, getloglevel );

	// ## test [2]  init_logger_functions parameter set test (putLogFatal)
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.putLogFatal, putLogFatal );

	// ## test [3]  init_logger_functions parameter set test (putLogError)
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.putLogError, putLogError );

	// ## test [4]  init_logger_functions parameter set test (putLogWarn)
	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.putLogWarn, putLogWarn );

	// ## test [5]  init_logger_functions parameter set test (putLogInfo)
	std::cout << "5----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.putLogInfo, putLogInfo );

	// ## test [6]  init_logger_functions parameter set test (putLogDebug)
	std::cout << "6----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.putLogDebug, putLogDebug );

	BOOST_MESSAGE( "----- init_logger_functions test end -----" );
}

void	init_replication_functions_test(){

	BOOST_MESSAGE( "----- init_replication_functions test start -----" );

	module_base_test	mod_base_test( "cinsert" );

	replicationpaymemory_func_type			replication_pay_memory	= &replication_pay_memory;
	boost::function< void( void ) >			lock_func				= &lock_func;
	boost::function< void( void ) >			unlock_func				= &unlock_func;
	const boost::asio::ip::tcp::endpoint&	virtual_service_endpoint_tcp( "10.10.10.10:6555" );
	const boost::asio::ip::udp::endpoint&	virtual_service_endpoint_udp( "20.20.20.20:4444" );

	mod_base_test.init_replication_functions(
												replication_pay_memory,
												lock_func,
												unlock_func,
												virtual_service_endpoint_tcp,
												virtual_service_endpoint_udp );

	// ## test [1]  init_replication_functions parameter set test (replication_pay_memory)
	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.replication_pay_memory, replication_pay_memory );

	// ## test [1]  init_replication_functions parameter set test (lock_func)
	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.lock_func, lock_func );

	// ## test [1]  init_replication_functions parameter set test (unlock_func)
	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.unlock_func, unlock_func );

	// ## test [1]  init_replication_functions parameter set test (virtual_service_endpoint_tcp)
	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.virtual_service_endpoint_tcp, virtual_service_endpoint_tcp );

	// ## test [1]  init_replication_functions parameter set test (virtual_service_endpoint_udp)
	std::cout << "5----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.virtual_service_endpoint_udp, virtual_service_endpoint_udp );

	BOOST_MESSAGE( "----- init_replication_functions test end -----" );
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

