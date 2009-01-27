
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include "session_thread_control.h"

using namespace boost::unit_test;

typedef boost::shared_ptr<l7vs::tcp_session>				session_type;
typedef boost::shared_ptr<l7vs::session_thread_control>		stc_type;

class	test_thread{
	boost::function<void()>	access_func;
	boost::thread			acc_thread;
	void					run(){
		for( unsigned int i = 0; i < UINT_MAX; ++i ){
			access_func();
		}
	}
public:
	test_thread(){}
	void	set_function( boost::function<void()> in_func ){
		access_func = in_func;
	}
	void	strat(){
	}
	void	stop(){}
};


//test case1. 全メソッドの正常動作確認
void	stc_method_test1(){
	timespec	wait_time;
	wait_time.tv_sec = 0;
	wait_time.tv_nsec = 10;
// unit_test[1]  session_thread_controlオブジェクトの作成
	BOOST_MESSAGE( "-----1" );
	l7vs::l7vsd					vsd;
	l7vs::replication			rep;
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
	boost::asio::io_service	dispatcher;
	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( session ) );

//
// unit_test[2]  get_sessionメソッドのテスト
	BOOST_MESSAGE( "-----2" );
	BOOST_CHECK_EQUAL( session, stc->get_session() );

//スレッドID取得のテスト
// unit_test[3]  のぼりスレッドID取得
	BOOST_MESSAGE( "-----3" );
	std::cout << "upthread id : " << stc->get_upthread_id() << std::endl;
// unit_test[4]  くだりスレッドID取得
	BOOST_MESSAGE( "-----4" );
	std::cout << "downthread id : " << stc->get_downthread_id() << std::endl;

//のぼりスレッド

//パターン１
// unit_test[5]  のぼりスレッドスタート
	BOOST_MESSAGE( "-----5" );
	stc->startupstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 1000 );
// unit_test[6]  のぼりスレッドストップ
	BOOST_MESSAGE( "-----6" );
	stc->stopupstream();
//くだりスレッド

// unit_test[7]  くだりスレッドスタート
	BOOST_MESSAGE( "-----7" );
	stc->startdownstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 1000 );
// unit_test[8]  くだりスレッドストップ
	BOOST_MESSAGE( "-----8" );
	stc->stopdownstream();


// unit_test[9]  sessionののぼりスレッドが外から停止された場合
	BOOST_MESSAGE( "-----9" );
	stc->startupstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 1000 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_END );
	usleep( 1000 );
	stc->stopupstream();

// unit_test[10]  sessionのくだりスレッドが外から停止された場合
	BOOST_MESSAGE( "----10" );
	stc->startdownstream();
	usleep( 1000 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	stc->stopdownstream();


// unit_test[11]  停止スレッドの待ち合わせ
	BOOST_MESSAGE( "----11" );
	stc->join();
	usleep( 1 );
}

//test case2. スレッドの停止と再開
void	stc_method_test2(){
	timespec	wait_time;
	wait_time.tv_sec = 0;
	wait_time.tv_nsec = 10;
//session_thread_controlオブジェクトの作成
	l7vs::l7vsd					vsd;
	l7vs::replication			rep;
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
	boost::asio::io_service	dispatcher;
	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( session ) );
// 	session->set_upthread_stop( boost::bind( &l7vs::session_thread_control::stopupstream, stc ) );
// 	session->set_downthread_stop( boost::bind( &l7vs::session_thread_control::stopdownstream, stc ) );


// unit_test[12]  のぼりスレッドとくだりスレッドを開始し、順番に停止する(sessionループは自然に終了)
	BOOST_MESSAGE( "----12" );
	stc->startupstream();
	stc->startdownstream();
	usleep( 3000 );

// unit_test[13]  のぼりスレッドとくだりスレッドを開始し、くだりスレッド・のぼりスレッドの順に停止する(sessionループは外から停止)
	BOOST_MESSAGE( "----13" );
	stc->startupstream();
	stc->startdownstream();
	usleep( 1000 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	usleep( 3000 );

// unit_test[14]  のぼりスレッドを外部から停止後にのぼりスレッドを再開
	BOOST_MESSAGE( "----14" );
	stc->startupstream();
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_END );
	usleep( 100 );
	stc->startupstream();
	usleep( 2000 );

// unit_test[15]  くだりスレッドストップ後に再開
	BOOST_MESSAGE( "----15" );
	stc->startdownstream();
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	usleep( 100 );
	stc->startdownstream();
	usleep( 2000 );

// unit_test[16]  のぼりスレッドを連続で開始指示した場合
	BOOST_MESSAGE( "----16" );
	stc->startupstream();
	stc->startupstream();
	stc->startupstream();
	stc->startupstream();
	usleep( 10000 );

// unit_test[17]  くだりスレッドを連続で開始指示した場合
	BOOST_MESSAGE( "----17" );
	stc->startdownstream();
	stc->startdownstream();
	stc->startdownstream();
	stc->startdownstream();
	usleep( 10000 );

// unit_test[18]  のぼりスレッドを連続で開始・停止指示した場合(ループカウント0で出力)
	BOOST_MESSAGE( "----18" );
	stc->startupstream();
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_END );
	usleep( 100 );

// unit_test[19]  くだりスレッドを連続で開始・停止指示した場合(ループカウント0で出力)
	BOOST_MESSAGE( "----19" );
	stc->startdownstream();
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	usleep( 100 );

//停止スレッドの待ち合わせ
	stc->join();
//そのまま終わるとスレッド停止前にオブジェクトがDeleteされるのでSleepを入れる
	usleep(1);
}

//test case3. 外部スレッドからのメソッドアクセス
void	stc_method_test3(){
	timespec	wait_time;
	wait_time.tv_sec = 0;
	wait_time.tv_nsec = 10;
//session_thread_controlオブジェクトの作成
	l7vs::l7vsd					vsd;
	l7vs::replication			rep;
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
	boost::asio::io_service	dispatcher;
	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( session ) );

	test_thread	thread1;
	test_thread	thread2;

//停止スレッドの待ち合わせ
	stc->join();
//そのまま終わるとスレッド停止前にオブジェクトがDeleteされるのでSleepを入れる
	usleep(1);
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "stc_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &stc_method_test1 ) );
	ts->add( BOOST_TEST_CASE( &stc_method_test2 ) );
	ts->add( BOOST_TEST_CASE( &stc_method_test3 ) );

	framework::master_test_suite().add( ts );

	return 0;
}

