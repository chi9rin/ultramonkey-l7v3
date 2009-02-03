#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/thread/condition.hpp>
#include "realserver.h"
#include "logger.h"
#include "parameter.h"

class	l7vs::realserver;

using namespace boost::unit_test;


namespace l7vs{


class realserver_fake : public realserver
{
public:
	boost::mutex		starting_mutex;
	boost::condition	starting_condition;

	//!	constractor
	realserver_fake(){}
	//! destractor
	~realserver_fake(){}

    void	set_active( const int in_active ){ nactive = in_active ; }
    void	set_inact( const int in_inact ){ ninact = in_inact ; }

	void	increment_active2( const std::string& msg1, const std::string& msg2 ){
		boost::mutex::scoped_lock	lock( starting_mutex );
		starting_condition.wait( lock );

		BOOST_MESSAGE( msg1 );
		increment_active();
		BOOST_MESSAGE( msg2 );
	}

	void	decrement_active2( const std::string& msg1, const std::string& msg2 ){
		boost::mutex::scoped_lock	lock( starting_mutex );
		starting_condition.wait( lock );

		BOOST_MESSAGE( msg1 );
		decrement_active();
		BOOST_MESSAGE( msg2 );
	}

	void	increment_inact2( const std::string& msg1, const std::string& msg2 ){
		boost::mutex::scoped_lock	lock( starting_mutex );
		starting_condition.wait( lock );

		BOOST_MESSAGE( msg1 );
		increment_inact();
		BOOST_MESSAGE( msg2 );
	}
};

}	//namespace l7vs

l7vs::realserver_fake	rush_server;

void starting_thread( int id )
{
	{
		std::stringstream	msg1, msg2;

		msg1 << "start increment_active <Thread:" << id << ">";
		msg2 << "complete increment_active <Thread:" << id << ">";
		rush_server.increment_active2( msg1.str(), msg2.str() );
	}
	{
		std::stringstream	msg1, msg2;

		msg1 << "start decrement_active <Thread:" << id << ">";
		msg2 << "complete decrement_active <Thread:" << id << ">";
		rush_server.decrement_active2( msg1.str(), msg2.str() );
	}
	{
		std::stringstream	msg1, msg2;

		msg1 << "start increment_inact <Thread:" << id << ">";
		msg2 << "complete increment_inact <Thread:" << id << ">";
		rush_server.increment_inact2( msg1.str(), msg2.str() );
	}
};


//test case1.
void	realserver_test(){
	int	loop;

	int					nactive = 0;
	int					ninact = 0;
	unsigned long long	send_byte = 0ULL;
	int					weight = -1;

	// unit_test[1]  コンストラクタのテスト（全てが初期化済み）
	l7vs::realserver	server1;

	BOOST_CHECK_EQUAL( nactive, server1.get_active() );
	BOOST_CHECK_EQUAL( ninact, server1.get_inact() );
	BOOST_CHECK_EQUAL( send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( weight, server1.weight );

	// unit_test[2]  コピーコンストラクタのテスト（全てがコピー元と同じ）
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


	// unit_test[3]  比較オペレータのテスト（TCPエンドポイントが異なるとエラー）
	BOOST_CHECK_EQUAL( ( server1 == server2 ), true );

	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "33.33.33.33" ), 33 ) ;
	BOOST_CHECK_EQUAL( ( server1 == server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint != server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[4]  比較オペレータのテスト２（UDPエンドポイントが異なるとエラー）
	server1 = server2;
	server1.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "44.44.44.44" ), 44 ) ;
	BOOST_CHECK_EQUAL( ( server1 == server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint != server1.udp_endpoint );

	// unit_test[5]  比較オペレータのテスト３（weightが異なるとエラー）
	server1 = server2;
	server1.weight = 3;
	BOOST_CHECK_EQUAL( ( server1 == server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK( server2.weight != server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[6]  比較オペレータのテスト４（接続数が異なっても問題無し）
	server1 = server2;
	server1.increment_active();
	BOOST_CHECK_EQUAL( ( server1 == server2 ), true );
	BOOST_CHECK( server2.get_active() != server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[7]  比較オペレータのテスト５（切断数が異なっても問題無し）
	server1 = server2;
	server1.increment_inact();
	BOOST_CHECK_EQUAL( ( server1 == server2 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK( server2.get_inact() != server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[8]  比較オペレータのテスト６（送信バイト数が異なっても問題無し）
	server1 = server2;
	server1.send_byte = 100;
	BOOST_CHECK_EQUAL( ( server1 == server2 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK( server2.send_byte != server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[9]  否定オペレータのテスト（TCPエンドポイントが異なると検出）
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

	// unit_test[10]  否定オペレータのテスト２（UDPエンドポイントが異なると検出）
	server1 = server2;
	server1.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "44.44.44.44" ), 44 ) ;
	BOOST_CHECK_EQUAL( ( server1 != server2 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint != server1.udp_endpoint );

	// unit_test[11]  否定オペレータのテスト３（weightが異なると検出）
	server1 = server2;
	server1.weight = 3;
	BOOST_CHECK_EQUAL( ( server1 != server2 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK( server2.weight != server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[12]  否定オペレータのテスト４（接続数が異なっても問題無し）
	server1 = server2;
	server1.increment_active();
	BOOST_CHECK_EQUAL( ( server1 != server2 ), false );
	BOOST_CHECK( server2.get_active() != server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[13]  否定オペレータのテスト５（切断数が異なっても問題無し）
	server1 = server2;
	server1.increment_inact();
	BOOST_CHECK_EQUAL( ( server1 != server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK( server2.get_inact() != server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[14]  否定オペレータのテスト６（送信バイト数が異なっても問題無し）
	server1 = server2;
	server1.send_byte = 100;
	BOOST_CHECK_EQUAL( ( server1 != server2 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK( server2.send_byte != server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[15]  コピーオペレータのテスト（全てがコピー元と同じ）
	server1 = l7vs::realserver();
	server2 = server1 ;

	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	BOOST_CHECK( server1 == server2 );

	// unit_test[16]  大小比較オペレータのテスト（TCPエンドポイントが大きいと検出）
	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

	BOOST_CHECK_EQUAL( ( server2 < server1 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint < server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[17]  大小比較オペレータのテスト２（UDPエンドポイントが異なっても問題無し）
	server1 = server2;
	server1.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

	BOOST_CHECK_EQUAL( ( server2 < server1 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint < server1.udp_endpoint );

	// unit_test[18]  大小比較オペレータのテスト３（TCPが等しくweightが大きいと検出）
	server1 = server2;
	server1.weight = 10;

	BOOST_CHECK_EQUAL( ( server2 < server1 ), true );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK( server2.weight < server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[19]  大小比較オペレータのテスト４（接続数が異なっても問題無し）
	server1 = server2;
	server1.increment_active();
	BOOST_CHECK_EQUAL( ( server2 < server1 ), false );
	BOOST_CHECK( server2.get_active() != server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[20]  大小比較オペレータのテスト５（切断数が異なっても問題無し）
	server1 = server2;
	server1.increment_inact();
	BOOST_CHECK_EQUAL( ( server2 < server1 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK( server2.get_inact() != server1.get_inact() );
	BOOST_CHECK_EQUAL( server2.send_byte, server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[21]  大小比較オペレータのテスト６（送信バイト数が異なっても問題無し）
	server1 = server2;
	server1.send_byte = 100;
	BOOST_CHECK_EQUAL( ( server2 < server1 ), false );
	BOOST_CHECK_EQUAL( server2.get_active(), server1.get_active() );
	BOOST_CHECK_EQUAL( server2.get_inact(), server1.get_inact() );
	BOOST_CHECK( server2.send_byte != server1.send_byte );
	BOOST_CHECK_EQUAL( server2.weight, server1.weight );
	BOOST_CHECK( server2.tcp_endpoint == server1.tcp_endpoint );
	BOOST_CHECK( server2.udp_endpoint == server1.udp_endpoint );

	// unit_test[22]  接続数取得メソッドのテスト
	BOOST_CHECK_EQUAL( server1.get_active(), nactive );

	// unit_test[23]  切断数取得メソッドのテスト
	BOOST_CHECK_EQUAL( server1.get_inact(), ninact );

	// unit_test[24]  接続数インクリメントメソッドのテスト
	server1.increment_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 1 );

	// unit_test[25]  接続数デクリメントメソッドのテスト
	server1.decrement_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[26]  接続数インクリメントメソッドのテスト２（上限INT_MAXに達すると0にクリア）
	for ( loop = 0; loop < INT_MAX; loop++ ){
		server1.increment_active();
	}
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	l7vs::realserver_fake	server3;

	nactive = INT_MAX - 1 ;
	server3.set_active( nactive );
	BOOST_CHECK_EQUAL( server3.get_active(), nactive );

	server3.increment_active();
	BOOST_CHECK_EQUAL( server3.get_active(), 0 );

	// unit_test[27]  接続数デクリメントメソッドのテスト２（0に対しては何も行わない）
	server1.decrement_active();
	BOOST_CHECK_EQUAL( server1.get_active(), 0 );

	// unit_test[28]  切断数インクリメントメソッドのテスト
	server1.increment_inact();
	BOOST_CHECK_EQUAL( server1.get_inact(), 1 );

	// unit_test[29]  切断数インクリメントメソッドのテスト２（上限INT_MAXに達すると0にクリア）
	for ( loop = server1.get_inact(); loop < INT_MAX; loop++ ){
		server1.increment_inact();
	}
	BOOST_CHECK_EQUAL( server1.get_inact(), 0 );

	ninact = INT_MAX - 1 ;
	server3.set_inact( ninact );
	BOOST_CHECK_EQUAL( server3.get_inact(), ninact );

	server3.increment_inact();
	BOOST_CHECK_EQUAL( server3.get_inact(), 0 );


	boost::thread	thread_item1( boost::bind ( &starting_thread, 1 ) );
	boost::thread	thread_item2( boost::bind ( &starting_thread, 2 ) );
	boost::thread	thread_item3( boost::bind ( &starting_thread, 3 ) );
	boost::thread	thread_item4( boost::bind ( &starting_thread, 4 ) );
	boost::thread	thread_item5( boost::bind ( &starting_thread, 5 ) );

	// unit_test[30]  接続数インクリメントメソッドのテスト３（複数スレッドから同時アクセス）
	BOOST_MESSAGE( "sleep in" );
	sleep( 1 );
	rush_server.starting_condition.notify_all();
	sleep( 1 );
	BOOST_CHECK_EQUAL( rush_server.get_active(), 5 );

	// unit_test[31]  接続数デクリメントメソッドのテスト３（複数スレッドから同時アクセス）
	BOOST_MESSAGE( "sleep in" );
	sleep( 1 );
	rush_server.starting_condition.notify_all();
	sleep( 1 );
	BOOST_CHECK_EQUAL( rush_server.get_active(), 0 );

	// unit_test[32]  切断数インクリメントメソッドのテスト３（複数スレッドから同時アクセス）
	BOOST_MESSAGE( "sleep in" );
	sleep( 1 );
	rush_server.starting_condition.notify_all();
	sleep( 1 );
	BOOST_CHECK_EQUAL( rush_server.get_inact(), 5 );

	thread_item1.join();
	thread_item2.join();
	thread_item3.join();
	thread_item4.join();
	thread_item5.join();
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){
//	l7vs::Logger	logger;
//	l7vs::Parameter	parameter;

//	logger.loadConf();

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "realserver_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &realserver_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

