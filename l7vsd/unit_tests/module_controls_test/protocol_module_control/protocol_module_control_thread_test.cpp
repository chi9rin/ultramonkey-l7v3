
#include <iostream>
#include <dlfcn.h>
#include <boost/test/included/unit_test.hpp>
#include "protocol_module_control.h"
#include "protocol_module_control_test.h"
#include "logger.h"
#include "parameter.h"

using namespace l7vs;
using namespace boost::unit_test;

std::vector<l7vs::protocol_module_base*> test1_vec;
std::vector<l7vs::protocol_module_base*> test2_vec;
#define THREAD_LOOP_COUNT 100
void	thread1(){
	BOOST_TEST_MESSAGE( "thread1 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	test1_vec.clear();
	l7vs::protocol_module_base*		protomod1 = NULL;
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		try{
			protomod1 = control.load_module( PM1 );
		}
		catch(...){
			BOOST_ERROR( "exception : load_module thread1" );
		}
		test1_vec.push_back(protomod1);
	}
	BOOST_CHECK_EQUAL( test1_vec.size(), (size_t)THREAD_LOOP_COUNT );

	std::vector<l7vs::protocol_module_base*>::iterator test1_it = test1_vec.begin();
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		control.unload_module(*test1_it);
		test1_it = test1_vec.erase(test1_it);
		test1_it = test1_vec.begin();
	}
	BOOST_CHECK_EQUAL( test1_vec.size(), (size_t)0 );
	BOOST_TEST_MESSAGE( "thread1 stop" );
}

void	thread2(){
	BOOST_TEST_MESSAGE( "thread2 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	test2_vec.clear();
	l7vs::protocol_module_base*		protomod2 = NULL;
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		try{
			protomod2 = control.load_module( PM2 );
		}
		catch(...){
			BOOST_ERROR( "exception : load_module thread2" );
		}
		test2_vec.push_back(protomod2);
	}
	BOOST_CHECK_EQUAL( test2_vec.size(), (size_t)THREAD_LOOP_COUNT );

	std::vector<l7vs::protocol_module_base*>::iterator test2_it = test2_vec.begin();
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		control.unload_module(*test2_it);
		test2_it = test2_vec.erase(test2_it);
		test2_it = test2_vec.begin();
	}
	BOOST_CHECK_EQUAL( test2_vec.size(), (size_t)0 );
	BOOST_TEST_MESSAGE( "thread2 stop" );
}

void	thread3(){
	BOOST_TEST_MESSAGE( "thread3 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	test1_vec.clear();
	l7vs::protocol_module_base*		protomod1 = NULL;
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		try{
			protomod1 = control.load_module( PM1 );
		}
		catch(...){
			BOOST_ERROR( "exception : load_module thread3" );
		}
		test1_vec.push_back(protomod1);
	}
	BOOST_CHECK_EQUAL( test1_vec.size(), (size_t)THREAD_LOOP_COUNT );
	BOOST_TEST_MESSAGE( "thread3 stop" );
}

void	thread4(){
	BOOST_TEST_MESSAGE( "thread4 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	test2_vec.clear();
	l7vs::protocol_module_base*		protomod2 = NULL;
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		try{
			protomod2 = control.load_module( PM1 );
		}
		catch(...){
			BOOST_ERROR( "exception : load_module thread4" );
		}
		test2_vec.push_back(protomod2);
	}
	BOOST_CHECK_EQUAL( test2_vec.size(), (size_t)THREAD_LOOP_COUNT );
	BOOST_TEST_MESSAGE( "thread4 stop" );
}

void	thread5(){
	BOOST_TEST_MESSAGE( "thread5 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	BOOST_CHECK_EQUAL( test1_vec.size(), (size_t)THREAD_LOOP_COUNT );
	std::vector<l7vs::protocol_module_base*>::iterator test1_it = test1_vec.begin();
#if 1
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		control.unload_module(*test1_it);
		test1_vec.erase(test1_it);
		test1_it = test1_vec.begin();
	}
#else
	for( test1_it = test1_vec.begin(); test1_it != test1_vec.end(); test1_it++ ){
		control.unload_module(*test1_it);
	}
	test1_vec.clear();
#endif
	BOOST_TEST_MESSAGE( "thread5 stop" );
}

void	thread6(){
	BOOST_TEST_MESSAGE( "thread6 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	BOOST_CHECK_EQUAL( test2_vec.size(), (size_t)THREAD_LOOP_COUNT );
	std::vector<l7vs::protocol_module_base*>::iterator test2_it = test2_vec.begin();
#if 1
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		control.unload_module(*test2_it);
		test2_vec.erase(test2_it);
		test2_it = test2_vec.begin();
	}
#else
	for( test2_it = test2_vec.begin(); test2_it != test2_vec.end(); test2_it++ ){
		control.unload_module(*test2_it);
	}
	test2_vec.clear();
#endif
	BOOST_TEST_MESSAGE( "thread6 stop" );
}

void	thread7(){
	BOOST_TEST_MESSAGE( "thread7 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	test1_vec.clear();
	l7vs::protocol_module_base*		protomod1 = NULL;
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		try{
			protomod1 = control.load_module( PM1 );
		}
		catch(...){
			BOOST_ERROR( "exception : load_module thread7" );
		}
		test1_vec.push_back(protomod1);
	}
	BOOST_CHECK_EQUAL( test1_vec.size(), (size_t)THREAD_LOOP_COUNT );

	std::vector<l7vs::protocol_module_base*>::iterator test1_it = test1_vec.begin();
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		control.unload_module(*test1_it);
		test1_it = test1_vec.erase(test1_it);
		test1_it = test1_vec.begin();
	}
	BOOST_CHECK_EQUAL( test1_vec.size(), (size_t)0 );
	BOOST_TEST_MESSAGE( "thread7 stop" );
}

void	thread8(){
	BOOST_TEST_MESSAGE( "thread8 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	test2_vec.clear();
	l7vs::protocol_module_base*		protomod2 = NULL;
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		try{
			protomod2 = control.load_module( PM1 );
		}
		catch(...){
			BOOST_ERROR( "exception : load_module thread8" );
		}
		test2_vec.push_back(protomod2);
	}
	BOOST_CHECK_EQUAL( test2_vec.size(), (size_t)THREAD_LOOP_COUNT );

	std::vector<l7vs::protocol_module_base*>::iterator test2_it = test2_vec.begin();
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		control.unload_module(*test2_it);
		test2_it = test2_vec.erase(test2_it);
		test2_it = test2_vec.begin();
	}
	BOOST_CHECK_EQUAL( test2_vec.size(), (size_t)0 );
	BOOST_TEST_MESSAGE( "thread8 stop" );
}

void	thread9(){
	BOOST_TEST_MESSAGE( "thread9 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	l7vs::protocol_module_base*		protomod1 = NULL;
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		try{
			protomod1 = control.load_module( PM1 );
		}
		catch(...){
			BOOST_ERROR( "exception : load_module thread9" );
		}
		control.unload_module( protomod1 );
	}
	BOOST_TEST_MESSAGE( "thread9 stop" );
}

void	thread10(){
	BOOST_TEST_MESSAGE( "thread10 start" );
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	l7vs::protocol_module_base*		protomod2 = NULL;
	for( int i = 0; i < THREAD_LOOP_COUNT; i++ ){
		try{
			protomod2 = control.load_module( PM2 );
		}
		catch(...){
			BOOST_ERROR( "exception : load_module thread10" );
		}
		control.unload_module( protomod2 );
	}
	BOOST_TEST_MESSAGE( "thread10 stop" );
}

void	load2_unload2_thread( int id, const std::string& modulename ){
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	l7vs::protocol_module_base*		protomod1 = NULL;
	protomod1 = control.load_module2( id, modulename );
	control.unload_module2( id, protomod1 );

	control.finalize();
}

void	load_unload2_thread( int id, const std::string& modulename ){
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	l7vs::protocol_module_base*		protomod1 = NULL;
	protomod1 = control.load_module( modulename );
	control.unload_module2( id, protomod1 );

	control.finalize();
}

void	load2_unload_thread( int id, const std::string& modulename ){
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	l7vs::protocol_module_base*		protomod1 = NULL;
	protomod1 = control.load_module2( id, modulename );
	control.unload_module( protomod1 );

	control.finalize();
}

void	protocol_module_control_test_thread1(){
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	control.initialize( "." );

	protocol_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
#if 1
	// unit_test[1] スレッド１はPM1のロードとアンロード、スレッド２はPM2のロードとアンロードを行い、最終的にロードモジュールのマップのサイズが0となっていることを確認する
	boost::thread	thd1( &thread1 );
	boost::thread	thd2( &thread2 );
	thd1.join();
	thd2.join();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
#endif
	control.finalize();
}

void	protocol_module_control_test_thread2(){
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	control.initialize( "." );

	protocol_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
#if 1
	// unit_test[2] スレッド１はPM1のロード、スレッド２はPM1のロードを行い、最終的にロードモジュールのマップのサイズが1となっていることを確認する
	boost::thread	thd1( &thread3 );
	boost::thread	thd2( &thread4 );
	thd1.join();
	thd2.join();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)1 );

	// unit_test[3] スレッド３はPM1のアンロード、スレッド４はPM1のアンロードを行い、最終的にロードモジュールのマップのサイズが0となっていることを確認する
	boost::thread	thd3( &thread5 );
	boost::thread	thd4( &thread6 );
	thd3.join();
	thd4.join();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
#endif
	control.finalize();
}

void	protocol_module_control_test_thread3(){
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	control.initialize( "." );

	protocol_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
	// unit_test[4] スレッド１はPM1のロードとアンロード、スレッド２はPM1のロードとアンロードを行い、最終的にロードモジュールのマップのサイズが0となっていることを確認する
	boost::thread	thd1( &thread7 );
	boost::thread	thd2( &thread8 );
	thd1.join();
	thd2.join();
	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
	control.finalize();
}

void	protocol_module_control_test_thread4(){
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	control.initialize( "." );

	protocol_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
	// unit_test[5] スレッド１はPM1のロード/アンロード、スレッド２はPM2のロード/アンロードをそれぞれセットで行い、最終的にロードモジュールのマップのサイズが0となっていることを確認する
	boost::thread	thd1( &thread9 );
	boost::thread	thd2( &thread10 );
	thd1.join();
	thd2.join();
	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
	control.finalize();
}

void	protocol_module_control_test_thread5(){
	protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
	control.initialize( "." );
	protocol_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );

	// unit_test[6] load_moduleを複数スレッドからアクセスした場合の動作チェック（同じモジュールを指定した場合）
	boost::thread	thd1( boost::bind( &load2_unload2_thread, 1, PM1 ) );
	boost::thread	thd2( boost::bind( &load2_unload2_thread, 2, PM1 ) );
	boost::thread	thd3( boost::bind( &load2_unload2_thread, 3, PM1 ) );
	boost::thread	thd4( boost::bind( &load2_unload2_thread, 4, PM1 ) );
	boost::thread	thd5( boost::bind( &load2_unload2_thread, 5, PM1 ) );

	sleep(1);
	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
	BOOST_MESSAGE( "notify_all" );
	control.load_condition.notify_all();

	sleep(1);
	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)1 );
	// unit_test[7] unload_moduleを複数スレッドからアクセスした場合の動作チェック（同じモジュールを指定した場合）
	BOOST_MESSAGE( "notify_all" );
	control.load_condition.notify_all();

	thd1.join();
	thd2.join();
	thd3.join();
	thd4.join();
	thd5.join();

	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );

	// unit_test[8] load_moduleとunload_moduleを複数スレッドからアクセスした場合の動作チェック（同じモジュールを指定した場合）
	boost::thread	thd6( boost::bind( &load2_unload_thread, 6, PM1 ) );
	boost::thread	thd7( boost::bind( &load_unload2_thread, 7, PM1 ) );

	sleep(1);
	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)1 );
	BOOST_MESSAGE( "notify_all" );
	control.load_condition.notify_all();

	thd6.join();
	thd7.join();

	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );

	// unit_test[9] load_moduleを複数スレッドからアクセスした場合の動作チェック（それぞれ別のモジュールを指定した場合）
	boost::thread	thd8( boost::bind( &load2_unload2_thread, 8, PM1 ) );
	boost::thread	thd9( boost::bind( &load2_unload2_thread, 9, PM2 ) );

	sleep(1);
	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
	BOOST_MESSAGE( "notify_all" );
	control.load_condition.notify_all();

	sleep(1);
	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)2 );
	// unit_test[10] unload_moduleを複数スレッドからアクセスした場合の動作チェック（それぞれ別のモジュールを指定した場合）
	BOOST_MESSAGE( "notify_all" );
	control.load_condition.notify_all();

	thd8.join();
	thd9.join();

	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );

	// unit_test[11] load_moduleとunload_moduleを複数スレッドからアクセスした場合の動作チェック（それぞれ別のモジュールを指定した場合）
	boost::thread	thd10( boost::bind( &load2_unload_thread, 10, PM1 ) );
	boost::thread	thd11( boost::bind( &load_unload2_thread, 11, PM2 ) );

	sleep(1);
	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)1 );
	BOOST_MESSAGE( "notify_all" );
	control.load_condition.notify_all();

	thd10.join();
	thd11.join();

	loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
	control.finalize();
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

//	Logger logger;
//	l7vs::Parameter param;

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_control_thread_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &protocol_module_control_test_thread1 ) );
	ts->add( BOOST_TEST_CASE( &protocol_module_control_test_thread2 ) );
	ts->add( BOOST_TEST_CASE( &protocol_module_control_test_thread3 ) );
	ts->add( BOOST_TEST_CASE( &protocol_module_control_test_thread4 ) );
	ts->add( BOOST_TEST_CASE( &protocol_module_control_test_thread5 ) );

	framework::master_test_suite().add( ts );

	return 0;
}
