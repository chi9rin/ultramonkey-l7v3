
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include "session_thread_control.h"

using namespace boost::unit_test;

typedef boost::shared_ptr<l7vs::tcp_session>				session_type;
typedef boost::shared_ptr<l7vs::session_thread_control>		stc_type;

//test case1. 全メソッドの正常動作確認
void	stc_method_test1(){
// unit_test[1]  session_thread_controlオブジェクトの作成
	session_type	session( new l7vs::tcp_session );
	stc_type		stc( new l7vs::session_thread_control( session ) );

//
// unit_test[2]  get_sessionメソッドのテスト
	BOOST_CHECK_EQUAL( session, stc->get_session() );

//スレッドID取得のテスト
// unit_test[3]  のぼりスレッドID取得
	std::cout << "upthread id : " << stc->get_upthread_id() << std::endl;
// unit_test[4]  くだりスレッドID取得
	std::cout << "downthread id : " << stc->get_downthread_id() << std::endl;

//のぼりスレッド

//パターン１
// unit_test[5]  のぼりスレッドスタート
 	stc->startupstream();
 	usleep( 50000 );
//セッションのループが停止
 	stc->get_session()->stop_up_thread();
// unit_test[6]  のぼりスレッドストップ
	stc->stopupstream();
//くだりスレッド

// unit_test[7]  くだりスレッドスタート
	stc->startdownstream();
	usleep( 50000 );
//セッションのループが停止
	stc->get_session()->stop_down_thread();
// unit_test[8]  くだりスレッドストップ
	stc->stopdownstream();

// unit_test[9]  停止スレッドの待ち合わせ
	stc->join();
 	usleep( 1 );
}

//test case2. スレッドの停止と再開
void	stc_method_test2(){
//session_thread_controlオブジェクトの作成
	session_type	session( new l7vs::tcp_session );
	stc_type		stc( new l7vs::session_thread_control( session ) );

//のぼりスレッド
//パターン1
// unit_test[10]  のぼりスレッド停止後の再開
	stc->startupstream();
	usleep( 50000 );
//セッションのループが停止
	stc->get_session()->stop_up_thread();
//のぼりスレッドストップ
	stc->stopupstream();
	usleep( 50000 );
//のぼりスレッド再開
	stc->startupstream();
	usleep( 50000 );
//セッションのループが停止
	stc->get_session()->stop_up_thread();
//のぼりスレッドストップ
	stc->stopupstream();

//パターン２
// unit_test[11]  のぼりスレッドストップせずに再開
	stc->startupstream();
	usleep( 50000 );
//セッションのループが停止
	stc->get_session()->stop_up_thread();
//のぼりスレッドスタート(スレッドストップせずに再開)
	stc->startupstream();
	usleep( 50000 );
//セッションのループが停止
	stc->get_session()->stop_up_thread();
//スレッドストップ
	stc->stopupstream();

//くだりスレッド
//パターン1
// unit_test[10]  くだりスレッド停止後の再開
	stc->startdownstream();
	usleep( 50000 );
//セくだりッションのループが停止
	stc->get_session()->stop_down_thread();
//くだりスレッドストップ
	stc->stopupstream();
	usleep( 50000 );
//くだりスレッド再開
	stc->startdownstream();
	usleep( 50000 );
//くだりセッションのループが停止
	stc->get_session()->stop_down_thread();
//くだりスレッドストップ
	stc->stopdownstream();

//パターン２
// unit_test[11]  くだりスレッドストップせずに再開
	stc->startdownstream();
	usleep( 50000 );
//くだりセッションのループが停止
	stc->get_session()->stop_down_thread();
//くだりスレッドスタート(スレッドストップせずに再開)
	stc->startdownstream();
	usleep( 50000 );
//くだりセッションのループが停止
	stc->get_session()->stop_down_thread();
//くだりスレッドストップ
	stc->stopdownstream();

//停止スレッドの待ち合わせ
	stc->join();
	timespec	wait_time;
	wait_time.tv_sec = 0;
	wait_time.tv_nsec = 10;
	nanosleep( &wait_time, &wait_time );
}

//test case3. 外部スレッドからのメソッドアクセス
void	stc_method_test3(){
//session_thread_controlオブジェクトの作成
	session_type	session( new l7vs::tcp_session );
	stc_type		stc( new l7vs::session_thread_control( session ) );


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

