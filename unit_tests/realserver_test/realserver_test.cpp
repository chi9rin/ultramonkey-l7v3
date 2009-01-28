#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "realserver.h"
#include "logger.h"
#include "parameter.h"

class	l7vs::realserver;

using namespace boost::unit_test;

//test case1.
void	realserver_test(){
	int	loop;

	// unit_test[1]  コンストラクタのテスト
	l7vs::realserver	server1;
	// unit_test[2]  コピーコンストラクタのテスト
	l7vs::realserver	server2( server1 );

	// unit_test[3]  比較オペレータのテスト
	BOOST_CHECK( server1 == server2 );

	// unit_test[4]  否定オペレータのテスト
	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.87" ), 22 ) ;

	BOOST_CHECK( server1 != server2 );

	// unit_test[5]  コピーオペレータのテスト
	server2 = server1 ;

	BOOST_CHECK( server1 == server2 );

	// unit_test[6]  大小比較オペレータのテスト
	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

	BOOST_CHECK( server1 < server2 );

	// unit_test[7]  接続数取得メソッドのテスト
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[8]  切断数取得メソッドのテスト
	BOOST_CHECK_EQUAL( server1.get_inact(), 0 );

	// unit_test[9]  接続数インクリメントメソッドのテスト
	server1.increment_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 1 );

	// unit_test[10]  接続数デクリメントメソッドのテスト
	server1.decrement_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[11]  接続数インクリメントメソッドのテスト２
	for ( loop = 0; loop < INT_MAX; loop++ ){
		server1.increment_active();
	}
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[12]  接続数デクリメントメソッドのテスト２
	server1.decrement_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[13]  切断数インクリメントメソッドのテスト
	server1.increment_inact();
	BOOST_CHECK_EQUAL( server1.get_inact(), 1 );

	// unit_test[14]  切断数インクリメントメソッドのテスト２
	for ( loop = server1.get_inact(); loop < INT_MAX; loop++ ){
		server1.increment_inact();
	}
	BOOST_CHECK_EQUAL( server1.get_inact(), 0 );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){
	l7vs::Logger	logger;
	l7vs::Parameter	parameter;

	logger.loadConf();
	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "realserver_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &realserver_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

