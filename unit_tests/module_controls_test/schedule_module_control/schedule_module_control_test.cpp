
#include <iostream>
#include <dlfcn.h>
#include <boost/test/included/unit_test.hpp>
#include "schedule_module_control.h"
#include "schedule_module_control_test.h"

using namespace l7vs;
using namespace boost::unit_test;

void	schedule_module_control_getInstance_test(){
	BOOST_TEST_MESSAGE( "schedule_module_control_getInstance_test" );
	// unit_test[1]  getInstanceメソッドのテスト
	schedule_module_control_testclass& control = schedule_module_control_testclass::getInstance();
	schedule_module_control_testclass& control_2 = schedule_module_control_testclass::getInstance();
	BOOST_CHECK_EQUAL( &control, &control_2 );

	// unit_test[2]  モジュールパスのデフォルト設定の確認
	BOOST_CHECK_EQUAL( control.get_modulefile_path(), "" );
	// unit_test[3]  マップのデフォルト設定の確認
	schedule_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK( loadmodulemap.size() == 0 );
}

void	schedule_module_control_initialize_test(){
	BOOST_TEST_MESSAGE( "schedule_module_control_initialize_test" );
	schedule_module_control_testclass& control = schedule_module_control_testclass::getInstance();

	// unit_test[4]  initializeメソッドのテスト（正常系その１）
	control.initialize( "." );

	// unit_test[5]  initialize 引数で指定されたモジュールパスが設定されているか？
	BOOST_CHECK_EQUAL( control.get_modulefile_path(), "." );
	control.finalize();

	// unit_test[6]  initializeメソッドのテスト（正常系その２） 指定するパスを変えてみる
	control.initialize( "./schedule" );
	// unit_test[7]  initialize 引数で指定されたモジュールパスが設定されているか？
	BOOST_CHECK_EQUAL( control.get_modulefile_path(), "./schedule" );
	control.finalize();
}

void	schedule_module_control_finalize_test(){
	BOOST_TEST_MESSAGE( "schedule_module_control_finalize_test" );
	schedule_module_control_testclass& control = schedule_module_control_testclass::getInstance();

	// unit_test[8]  finalizeメソッドのテスト
	control.finalize();
}

void	schedule_module_control_load_test(){
	BOOST_TEST_MESSAGE( "schedule_module_control_load_test" );
	schedule_module_control_testclass& control = schedule_module_control_testclass::getInstance();
#if 0
	// unit_test[]  load_moduleメソッドのテスト initializeしないでロードした場合
	l7vs::schedule_module_base*		schedule1 = NULL;
	try{
		schedule1 = control.load_module( PM1 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	BOOST_CHECK( schedule1 != NULL );
	BOOST_CHECK( dlerror() == NULL );
	control.unload_module( schedule1 );
#endif
	// unit_test[9]  load_moduleメソッドのテスト ロードするモジュールが存在しない場合
	l7vs::schedule_module_base*		schedule2 = NULL;
	try{
		schedule2 = control.load_module( "error" );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	BOOST_CHECK( schedule2 == NULL );
	control.unload_module( schedule2 );

	// unit_test[10]  load_moduleメソッドのテスト（正常系）
	control.initialize( "." );
	l7vs::schedule_module_base*		schedule3 = NULL;
	try{
		schedule3 = control.load_module( PM1 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	BOOST_CHECK( schedule3 != NULL );
	// unit_test[11]  load_module マップに追加されたか？
	schedule_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK( loadmodulemap.size() == 1 );
	// unit_test[12]  load_module 参照回数の増加の確認0→1
	schedule_module_control::name_module_info_map::iterator it;
	it = loadmodulemap.find( PM1 );
	BOOST_CHECK( it->second.ref_count == 1 );
	// unit_test[13]  load_module create_funcの確認
	BOOST_CHECK( it->second.create_func != NULL );
	// unit_test[14]  load_module destroy_funcの確認
	BOOST_CHECK( it->second.destroy_func != NULL );

	// unit_test[15]  load_moduleメソッドのテスト 同じモジュールを再ロードする
	l7vs::schedule_module_base*		schedule4 = NULL;
	try{
		schedule4 = control.load_module( PM1 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
 	BOOST_CHECK( schedule4 != NULL );
	// unit_test[16]  load_module 別のインスタンスか？
	BOOST_CHECK( schedule3 != schedule4 );

	// unit_test[17]  load_module 参照回数の増加の確認1→2
	it = loadmodulemap.find( PM1 );
	BOOST_CHECK( it->second.ref_count == 2 );

	// unit_test[18]  load_moduleメソッドのテスト 別のモジュールをロードする
	l7vs::schedule_module_base*		schedule5 = NULL;
	try{
		schedule5 = control.load_module( PM2 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
 	BOOST_CHECK( schedule5 != NULL );
	// unit_test[19]  load_module 別のインスタンスか？
	BOOST_CHECK( schedule4 != schedule5 );
	// unit_test[20]  load_module マップが更新されたか？
	BOOST_CHECK( loadmodulemap.size() == 2 );
	// load_module 参照回数の増加の確認0→1
	it = loadmodulemap.find( PM2 );
	BOOST_CHECK( it->second.ref_count == 1 );

	control.unload_module( schedule3 );
	control.unload_module( schedule4 );
	control.unload_module( schedule5 );
	control.finalize();
}

void	schedule_module_control_unload_test(){
	BOOST_TEST_MESSAGE( "schedule_module_control_unload_test" );
	schedule_module_control_testclass& control = schedule_module_control_testclass::getInstance();
	control.initialize( "." );

	// unit_test[21]  unload_moduleメソッドのテスト 1回だけロードしたモジュールを指定する
	l7vs::schedule_module_base*		schedule1 = NULL;
	try{
		schedule1 = control.load_module( PM1 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	control.unload_module( schedule1 );

	// unit_test[22]  unload_moduleメソッドのテスト 2回ロードしたモジュールを指定する
	l7vs::schedule_module_base*		schedule2 = NULL;
	try{
		schedule2 = control.load_module( PM1 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	l7vs::schedule_module_base*		schedule3 = NULL;
	try{
		schedule3 = control.load_module( PM1 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	schedule_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
	BOOST_CHECK( loadmodulemap.size() == 1 );
	schedule_module_control::name_module_info_map::iterator it;
	it = loadmodulemap.find( PM1 );

	control.unload_module( schedule2 );
	BOOST_CHECK( loadmodulemap.size() == 1 );
	// unit_test[23]  unload_module 参照回数の減少の確認2→1
	it = loadmodulemap.find( PM1 );
	BOOST_CHECK( it->second.ref_count == 1 );

	control.unload_module( schedule3 );
	// unload_module 参照回数の減少の確認1→0
	it = loadmodulemap.find( PM1 );
	BOOST_CHECK( it->second.ref_count == 0 );
	// unit_test[24]  unload_module マップから削除されたか？
	BOOST_CHECK( loadmodulemap.size() == 0 );

	// unit_test[25]  unload_moduleメソッドのテスト 引数がNULLの場合
	l7vs::schedule_module_base*		schedule4 = NULL;
	control.unload_module( schedule4 );
	BOOST_CHECK( dlerror() == NULL );

#if 0
	// unit_test[]  unload_moduleメソッドのテスト 引数のモジュールを2回unloadした場合
	l7vs::schedule_module_base*		schedule5 = NULL;
	try{
		schedule5 = control.load_module( PM1 );
	}
	catch(...){
		BOOST_ERROR( "exception : load_module" );
	}
	control.unload_module( schedule5 );
	BOOST_CHECK( dlerror() == NULL );
	control.unload_module( schedule5 );
	BOOST_CHECK( dlerror() == NULL );
#endif
	control.finalize();
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "schedule_module_control" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &schedule_module_control_getInstance_test ) );
	ts->add( BOOST_TEST_CASE( &schedule_module_control_initialize_test ) );
	ts->add( BOOST_TEST_CASE( &schedule_module_control_finalize_test ) );
	ts->add( BOOST_TEST_CASE( &schedule_module_control_load_test ) );
	ts->add( BOOST_TEST_CASE( &schedule_module_control_unload_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
