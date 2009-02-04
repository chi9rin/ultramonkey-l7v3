
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "session_thread_control.h"

using namespace boost::unit_test;

typedef boost::shared_ptr<l7vs::tcp_session>				session_type;
typedef boost::shared_ptr<l7vs::session_thread_control>		stc_type;

class	test_thread{
	boost::function<void()>		access_func;
	boost::mutex				func_mutex;
	boost::thread				acc_thread;
	bool						stop_flag;
	boost::mutex				flag_mutex;
	void	run(){
		for( unsigned int i = 0; i < UINT_MAX; ++i ){
			{
				boost::mutex::scoped_lock( flag_mutex );
				if( stop_flag )break;
			}
			boost::mutex::scoped_lock( func_mutex );
			access_func();
		}
	}
public:
	test_thread(){}
	~test_thread(){ stop(); acc_thread.join(); }
	boost::thread::id	get_id(){ return acc_thread.get_id(); }
	void	start( boost::function<void()>	in_func ){
		{
			boost::mutex::scoped_lock( flag_mutex );
			stop_flag = false;
		}
		boost::mutex::scoped_lock( func_mutex );
		access_func = in_func;
		acc_thread = boost::thread( &test_thread::run, this );
	}
	void	stop(){
		boost::mutex::scoped_lock( flag_mutex );
		stop_flag = true;
	}
};


//test case1. 全メソッドの正常動作確認
void	stc_method_test1(){
// unit_test[1]  session_thread_controlオブジェクトの作成
	BOOST_MESSAGE( "-----1" );
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
// 	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ) ) );

//
// unit_test[2]  get_sessionメソッドのテスト
	BOOST_MESSAGE( "-----2" );
// 	BOOST_CHECK_EQUAL( session, stc->get_session() );

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
	usleep( 4000000 );
// unit_test[6]  のぼりスレッドストップ
	BOOST_MESSAGE( "-----6" );
	stc->stopupstream();
//くだりスレッド

// unit_test[7]  くだりスレッドスタート
	BOOST_MESSAGE( "-----7" );
	stc->startdownstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 4000000 );
// unit_test[8]  くだりスレッドストップ
	BOOST_MESSAGE( "-----8" );
	stc->stopdownstream();


// unit_test[9]  sessionののぼりスレッドが外から停止された場合
	BOOST_MESSAGE( "-----9" );
	stc->startupstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 1000 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
	usleep( 1000 );
	stc->stopupstream();
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );

// unit_test[10]  sessionのくだりスレッドが外から停止された場合
	BOOST_MESSAGE( "----10" );
	stc->startdownstream();
	usleep( 1000 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	usleep( 1000 );
	stc->stopdownstream();
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );


// unit_test[11]  停止スレッドの待ち合わせ
	BOOST_MESSAGE( "----11" );
	stc->join();
	usleep( 1 );
}

//test case2. スレッドの停止と再開
void	stc_method_test2(){
//session_thread_controlオブジェクトの作成
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
// 	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ) ) );


// unit_test[12]  のぼりスレッドとくだりスレッドを開始し、順番に停止する(sessionループは自然に終了)
	BOOST_MESSAGE( "----12" );
	stc->startupstream();
	stc->startdownstream();
	usleep( 5000000 );

// unit_test[13]  のぼりスレッドとくだりスレッドを開始し、くだりスレッド・のぼりスレッドの順に停止する(sessionループは外から停止)
	BOOST_MESSAGE( "----13" );
	stc->startupstream();
	stc->startdownstream();
	usleep( 1000 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	usleep( 1000 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
	usleep( 1000 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );

// unit_test[14]  のぼりスレッドを外部から停止後にのぼりスレッドを再開
	BOOST_MESSAGE( "----14" );
	stc->startupstream();
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );
	stc->startupstream();
	usleep( 2000 );
	//停止
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );

// unit_test[15]  くだりスレッドストップ後に再開
	BOOST_MESSAGE( "----15" );
	stc->startdownstream();
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );
	stc->startdownstream();
	usleep( 2000 );
	//停止
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );

// unit_test[16]  のぼりスレッドを連続で開始指示した場合
	BOOST_MESSAGE( "----16" );
	stc->startupstream();
	stc->startupstream();
	stc->startupstream();
	stc->startupstream();
	usleep( 10000 );
	//停止
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );

// unit_test[17]  くだりスレッドを連続で開始指示した場合
	BOOST_MESSAGE( "----17" );
	stc->startdownstream();
	stc->startdownstream();
	stc->startdownstream();
	stc->startdownstream();
	usleep( 10000 );
	//停止
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );

// unit_test[18]  のぼりスレッドを連続で開始・停止指示した場合(ループカウント0で出力)
	BOOST_MESSAGE( "----18" );
	stc->startupstream();
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );

// unit_test[19]  くだりスレッドを連続で開始・停止指示した場合(ループカウント0で出力)
	BOOST_MESSAGE( "----19" );
	stc->startdownstream();
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
	usleep( 100 );
	stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );

//停止スレッドの待ち合わせ
	stc->join();
//join後のスレッドID取得のテスト
// unit_test[20]  のぼりスレッドID取得
	BOOST_MESSAGE( "-----20" );
	std::cout << "upthread id : " << stc->get_upthread_id() << std::endl;
// unit_test[21]  くだりスレッドID取得
	BOOST_MESSAGE( "-----21" );
	std::cout << "downthread id : " << stc->get_downthread_id() << std::endl;

// unit_test[22]  joinを何回も連続で呼ぶ
	BOOST_MESSAGE( "-----22" );
	stc->join();
	stc->join();
	stc->join();
	stc->join();
	stc->join();
	stc->join();
	stc->join();
	stc->join();
	stc->join();
	stc->join();

}

//test case3
void	stc_method_test3(){
// unit_test[23]  joinを呼ばずにオブジェクト廃棄する
	BOOST_MESSAGE( "-----23" );
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
// 	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ) ) );

}

//test case4
void	stc_method_test4(){
// unit_test[24]  スレッドを動かしたまま廃棄する
	BOOST_MESSAGE( "-----24" );
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
// 	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ) ) );

	//start thread
	stc->startupstream();
	stc->startdownstream();
	usleep( 1 );
}

//test case5. 外部スレッドからのメソッドアクセス
void	stc_method_test5(){
//session_thread_controlオブジェクトの作成
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
// 	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ) ) );

	test_thread	thread1;

// unit_test[25]  スレッドを開始して、別スレッドからスレッド停止メソッドを呼ぶ(のぼりスレッド)
	BOOST_MESSAGE( "-----25" );
	std::cout << "thread id : " << thread1.get_id() << std::endl; 
	stc->startupstream();
	usleep( 2000000 );
	thread1.start( boost::bind( &l7vs::session_thread_control::stopupstream, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 2000000 );
	std::cout << "thread id : " << thread1.get_id() << std::endl;
	thread1.stop();

// unit_test[26]  スレッドを開始して、別スレッドからスレッド停止メソッドを呼ぶ(くだりスレッド)
	BOOST_MESSAGE( "-----26" );
	std::cout << "thread id : " << thread1.get_id() << std::endl; 
	stc->startdownstream();
	usleep( 2000000 );
	thread1.start( boost::bind( &l7vs::session_thread_control::stopdownstream, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 2000000 );
	std::cout << "thread id : " << thread1.get_id() << std::endl;
	thread1.stop();

// unit_test[27]  スレッドが停止すると同時にべつすレッドから開始メソッドが呼ばれる(のぼりスレッド)
	BOOST_MESSAGE( "-----27" );
	std::cout << "thread id : " << thread1.get_id() << std::endl; 
	stc->startupstream();
	usleep( 2000000 );
	thread1.start( boost::bind( &l7vs::session_thread_control::startupstream, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 2000000 );
	std::cout << "thread id : " << thread1.get_id() << std::endl;
	thread1.stop();

// unit_test[28]  スレッドが停止すると同時にべつすレッドから開始メソッドが呼ばれる(くだりスレッド)
	BOOST_MESSAGE( "-----28" );
	std::cout << "thread id : " << thread1.get_id() << std::endl; 
	stc->startdownstream();
	usleep( 2000000 );
	thread1.start( boost::bind( &l7vs::session_thread_control::startdownstream, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 2000000 );
	std::cout << "thread id : " << thread1.get_id() << std::endl;
	thread1.stop();


//停止スレッドの待ち合わせ
	stc->join();
}

//test case6. スレッド停止と同時にjoinが呼ばれる・その１(のぼりスレッド)
void	stc_method_test6(){
//session_thread_controlオブジェクトの作成
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
// 	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ) ) );

	test_thread	thread1;

// unit_test[29]  
	BOOST_MESSAGE( "-----29" );
	stc->startupstream();
	usleep( 2000000 );
	thread1.start( boost::bind( &l7vs::session_thread_control::join, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 2000000 );
	thread1.stop();
}

//test case6. スレッド停止と同時にjoinが呼ばれる・その2(くだりスレッド)
void	stc_method_test7(){
//session_thread_controlオブジェクトの作成
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
// 	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ) ) );

	test_thread	thread1;

// unit_test[30]  
	BOOST_MESSAGE( "-----30" );
	stc->startdownstream();
	usleep( 2000000 );
	thread1.start( boost::bind( &l7vs::session_thread_control::join, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 2000000 );
	thread1.stop();
}

//test case6. スレッド停止と同時にjoinが呼ばれる・その3(上下スレッド)
void	stc_method_test8(){
//session_thread_controlオブジェクトの作成
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	l7vs::virtualservice_tcp	tcpservice( vsd, rep, element );
// 	session_type	session( new l7vs::tcp_session( tcpservice, dispatcher ) );
	stc_type		stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ) ) );

	test_thread	thread1;

// unit_test[31]  
	BOOST_MESSAGE( "-----31" );
	stc->startupstream();
	stc->startdownstream();
	usleep( 2000000 );
	thread1.start( boost::bind( &l7vs::session_thread_control::join, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
	usleep( 2000000 );
	thread1.stop();
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "stc_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &stc_method_test1 ) );
	ts->add( BOOST_TEST_CASE( &stc_method_test2 ) );
	ts->add( BOOST_TEST_CASE( &stc_method_test3 ) );
	ts->add( BOOST_TEST_CASE( &stc_method_test4 ) );
	ts->add( BOOST_TEST_CASE( &stc_method_test5 ) );
	ts->add( BOOST_TEST_CASE( &stc_method_test6 ) );
	ts->add( BOOST_TEST_CASE( &stc_method_test7 ) );
	ts->add( BOOST_TEST_CASE( &stc_method_test8 ) );

	framework::master_test_suite().add( ts );

	return 0;
}

