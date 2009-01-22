#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>

#include "module_base.h"
#include "logger_enum.h"

using namespace boost::unit_test_framework;


LOG_LEVEL_TAG	getloglevel(){ return LOG_LV_NONE; }
void			putLogFatal( const unsigned int, const std::string&, const char*, int ){}
void			putLogError( const unsigned int, const std::string&, const char*, int ){}
void			putLogWarn( const unsigned int, const std::string&, const char*, int ){}
void			putLogInfo( const unsigned int, const std::string&, const char*, int ){}
void			putLogDebug( const unsigned int, const std::string&, const char*, int ){}

void*			replication_pay_memory( const std::string&, unsigned int* ){ return NULL; }
void			lock_func(){}
void			unlock_func(){}


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

	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_1.name, "cinsert" );

	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_2.name, "" );

	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_3.name, NULL );

	BOOST_MESSAGE( "----- constractor test end -----" );
}

void	get_name_test(){

	BOOST_MESSAGE( "----- get_name test start -----" );

	module_base_test	mod_base_test_1( "cinsert" );
	module_base_test	mod_base_test_2( "" );
	module_base_test	mod_base_test_3( NULL );

	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_1.get_name(), "cinsert" );

	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_2.get_name(), "" );

	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test_3.get_name(), NULL );

	BOOST_MESSAGE( "----- get_name test end -----" );
}

void	init_logger_functions_test(){

	BOOST_MESSAGE( "----- init_logger_functions test start -----" );

	module_base_test	mod_base_test( "cinsert" );

	getloglevel_func_type	getloglevel = &getloglevel;
	logger_func_type		putLogFatal = &putLogFatal;
	logger_func_type		putLogError = &putLogError;
	logger_func_type		putLogWarn = &putLogWarn;
	logger_func_type		putLogInfo = &putLogInfo;
	logger_func_type		putLogDebug = &putLogDebug;

	mod_base_test.init_logger_functions(
											getloglevel,
											putLogFatal,
											putLogError,
											putLogWarn,
											putLogInfo,
											putLogDebug );

	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.getloglevel, getloglevel );

	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.putLogFatal, putLogFatal );

	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.putLogError, putLogError );

	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.putLogWarn, putLogWarn );

	std::cout << "5----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.putLogInfo, putLogInfo );

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

	std::cout << "1----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.replication_pay_memory, replication_pay_memory );

	std::cout << "2----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.lock_func, lock_func );

	std::cout << "3----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.unlock_func, unlock_func );

	std::cout << "4----------------------------------------" << std::endl;
	BOOST_CHECK_EQUAL( mod_base_test.virtual_service_endpoint_tcp, virtual_service_endpoint_tcp );

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

