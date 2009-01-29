#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "realserver.h"
#include "logger.h"
#include "parameter.h"

class	l7vs::realserver;

using namespace boost::unit_test;


namespace l7vs{

class realserver_fake : public realserver
{
public:
	//!	constractor
	realserver_fake(){}
	//! destractor
	~realserver_fake(){}

    void	set_active( const int in_active ){ nactive = in_active ; }
    void	set_inact( const int in_inact ){ ninact = in_inact ; }
};

}	//namespace l7vs



//test case1.
void	realserver_test(){
	int	loop;

	int					nactive = 0;
	int					ninact = 0;
	unsigned long long	send_byte = 0ULL;
	int					weight = -1;

	// unit_test[1]  コンストラクタのテスト
	l7vs::realserver	server1;

	BOOST_CHECK_EQUAL( nactive, server1.get_active() );
	BOOST_CHECK_EQUAL( ninact, server1.get_inact() );
	BOOST_CHECK_EQUAL( send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( weight, server1.weight );

	// unit_test[2]  コピーコンストラクタのテスト
	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "11.11.11.11" ), 11 ) ;
	server1.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "22.22.22.22" ), 22 ) ;
	server1.send_byte = 10;
	server1.weight = 0;
	server1.increment_active();
	server1.increment_inact();

	l7vs::realserver	server2( server1 );

	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );


	// unit_test[3]  比較オペレータのテスト
	BOOST_CHECK_EQUAL( ( server1 == server2 ), true );

	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "33.33.33.33" ), 33 ) ;
	BOOST_CHECK_EQUAL( ( server1 == server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint != server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[4]  比較オペレータのテスト２
	server1 = server2;
	server1.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "44.44.44.44" ), 44 ) ;
	BOOST_CHECK_EQUAL( ( server1 == server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint != server1.udp_endpoint );

	// unit_test[5]  比較オペレータのテスト３
	server1 = server2;
	server1.weight = 3;
	BOOST_CHECK_EQUAL( ( server1 == server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK( server2.weight != server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[6]  比較オペレータのテスト４
	server1 = server2;
	server1.increment_active();
	BOOST_CHECK_EQUAL( ( server1 == server2 ), true );
	BOOST_CHECK( server2.get_active() != server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[7]  比較オペレータのテスト５
	server1 = server2;
	server1.increment_inact();
	BOOST_CHECK_EQUAL( ( server1 == server2 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK( server2.get_inact() != server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[8]  比較オペレータのテスト６
	server1 = server2;
	server1.send_byte = 100;
	BOOST_CHECK_EQUAL( ( server1 == server2 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK( server2.send_byte != server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[9]  否定オペレータのテスト
	server1 = server2;

	BOOST_CHECK_EQUAL( ( server1 != server2 ), false);

	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "33.33.33.33" ), 33 ) ;
	BOOST_CHECK_EQUAL( ( server1 != server2 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint != server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[10]  否定オペレータのテスト２
	server1 = server2;
	server1.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "44.44.44.44" ), 44 ) ;
	BOOST_CHECK_EQUAL( ( server1 != server2 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint != server1.udp_endpoint );

	// unit_test[11]  否定オペレータのテスト３
	server1 = server2;
	server1.weight = 3;
	BOOST_CHECK_EQUAL( ( server1 != server2 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK( server2.weight != server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[12]  否定オペレータのテスト４
	server1 = server2;
	server1.increment_active();
	BOOST_CHECK_EQUAL( ( server1 != server2 ), false );
	BOOST_CHECK( server2.get_active() != server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[13]  否定オペレータのテスト５
	server1 = server2;
	server1.increment_inact();
	BOOST_CHECK_EQUAL( ( server1 != server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK( server2.get_inact() != server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[14]  否定オペレータのテスト６
	server1 = server2;
	server1.send_byte = 100;
	BOOST_CHECK_EQUAL( ( server1 != server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK( server2.send_byte != server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[15]  コピーオペレータのテスト
	server1 = l7vs::realserver();
	server2 = server1 ;

	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	BOOST_CHECK( server1 == server2 );

	// unit_test[16]  大小比較オペレータのテスト
	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

	BOOST_CHECK_EQUAL( ( server2 < server1 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint < server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[17]  大小比較オペレータのテスト２
	server1 = server2;
	server1.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

	BOOST_CHECK_EQUAL( ( server2 < server1 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint < server1.udp_endpoint );

	// unit_test[18]  大小比較オペレータのテスト３
	server1 = server2;
	server1.weight = 10;

	BOOST_CHECK_EQUAL( ( server2 < server1 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK( server2.weight < server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[19]  大小比較オペレータのテスト４
	server1 = server2;
	server1.increment_active();
	BOOST_CHECK_EQUAL( ( server2 < server1 ), false );
	BOOST_CHECK( server2.get_active() != server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[20]  大小比較オペレータのテスト５
	server1 = server2;
	server1.increment_inact();
	BOOST_CHECK_EQUAL( ( server2 < server1 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK( server2.get_inact() != server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[21]  大小比較オペレータのテスト６
	server1 = server2;
	server1.send_byte = 100;
	BOOST_CHECK_EQUAL( ( server2 < server1 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK( server2.send_byte != server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[7]  接続数取得メソッドのテスト
	BOOST_CHECK_EQUAL( server1.get_active(), nactive );

	// unit_test[8]  切断数取得メソッドのテスト
	BOOST_CHECK_EQUAL( server1.get_inact(), ninact );

	// unit_test[9]  接続数インクリメントメソッドのテスト
	server1.increment_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 1 );

	// unit_test[10]  接続数デクリメントメソッドのテスト
	server1.decrement_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[11]  接続数インクリメントメソッドのテスト２
//  Loggerを加えたら処理時間40秒が27分になってしまったので却下
//	for ( loop = 0; loop < INT_MAX; loop++ ){
//		server1.increment_active();
//	}
//	BOOST_CHECK_EQUAL( server1.get_active(), 0 );
	l7vs::realserver_fake	server3;

	server3.set_active( INT_MAX - 1 );
	server3.increment_active();
	BOOST_CHECK_EQUAL( server3.get_active(), 0 );

	// unit_test[12]  接続数デクリメントメソッドのテスト２
	server1.decrement_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[13]  切断数インクリメントメソッドのテスト
	server1.increment_inact();
	BOOST_CHECK_EQUAL( server1.get_inact(), 1 );

	// unit_test[14]  切断数インクリメントメソッドのテスト２
//  Loggerを加えたら処理時間40秒が27分になってしまったので却下
//	for ( loop = server1.get_inact(); loop < INT_MAX; loop++ ){
//		server1.increment_inact();
//	}
//	BOOST_CHECK_EQUAL( server1.get_inact(), 0 );

	server3.set_inact( INT_MAX - 1 );
	server3.increment_inact();
	BOOST_CHECK_EQUAL( server3.get_inact(), 0 );
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

