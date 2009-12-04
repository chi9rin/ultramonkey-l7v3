
#include <iostream>
#include <dlfcn.h>
#include <boost/test/included/unit_test.hpp>
#include "protocol_module_control.h"
#include "protocol_module_control_test.h"
#include "logger.h"
#include "parameter.h"

using namespace l7vs;
using namespace boost::unit_test;

void    protocol_module_control_getInstance_test(){
    // unit_test[1]  getInstance 取得したインスタンスが一致していることのチェック
    protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
    protocol_module_control_testclass& control_2 = protocol_module_control_testclass::getInstance();
    BOOST_CHECK_EQUAL( &control, &control_2 );

    // unit_test[2]  getInstance modulefile_pathのデフォルト値のチェック
    BOOST_CHECK_EQUAL( control.get_modulefile_path(), "" );
    // unit_test[3]  getInstance ロードモジュールのマップのデフォルトサイズのチェック
    protocol_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
    BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );
}

void    protocol_module_control_initialize_test(){
    protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();

    // unit_test[4]  initializeメソッドの呼び出しのチェック（正常系その１）
    control.initialize( "." );

    // unit_test[5]  initialize 引数で指定されたモジュールのパスが設定されていることのチェック
    BOOST_CHECK_EQUAL( control.get_modulefile_path(), "." );
    control.finalize();

    // unit_test[6]  initializeメソッドの呼び出しのチェック（正常系その２） 指定するパスを変えた場合
    control.initialize( "./protocol" );
    // unit_test[7]  initialize 引数で指定されたモジュールのパスが設定されていることのチェック
    std::string modulename( "./protocol" );
    control.initialize( modulename );
    BOOST_CHECK_EQUAL( control.get_modulefile_path(), modulename );
    control.finalize();
}

void    protocol_module_control_finalize_test(){
    protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();

    // unit_test[8]  finalizeメソッドの呼び出しのチェック
    control.finalize();
}

void    protocol_module_control_load_test(){
    protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();

#if 0
    // unit_test[]  load_moduleメソッドのテスト initializeしないでロードした場合
    l7vs::protocol_module_base*        protomod1 = NULL;
    try{
        protomod1 = control.load_module( PM1 );
    }
    catch(...){
        BOOST_ERROR( "exception : load_module" );
    }
    BOOST_CHECK( protomod1 != NULL );
    BOOST_CHECK( dlerror() == NULL );
    control.unload_module( protomod1 );
#endif

    // unit_test[9]  load_moduleメソッド ロードするモジュール名が存在しない場合のチェック
    l7vs::protocol_module_base*        protomod2 = NULL;
    try{
        protomod2 = control.load_module( "error" );
    }
    catch(...){
        BOOST_ERROR( "exception : load_module" );
    }
    BOOST_CHECK( protomod2 == NULL );
    control.unload_module( protomod2 );

    // unit_test[10]  load_moduleメソッドのテスト（正常系）戻り値がNULLでないことのチェック
    control.initialize( "." );
    l7vs::protocol_module_base*        protomod3 = NULL;
    try{
        protomod3 = control.load_module( PM1 );
    }
    catch(...){
        BOOST_ERROR( "exception : load_module" );
    }
    BOOST_CHECK( protomod3 != NULL );
    // unit_test[11]  load_module ロードモジュールのマップに追加され、マップのサイズが変わっていることのチェック
    protocol_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
    BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)1 );
    // unit_test[12]  load_module ロードモジュールのマップのロードしたモジュールの参照回数が増加していることのチェック0→1
    protocol_module_control::name_module_info_map::iterator it;
    it = loadmodulemap.find( PM1 );
    BOOST_CHECK_EQUAL( it->second.ref_count, (unsigned int)1 );
    // unit_test[13]  load_module 指定したモジュールのdlopenの戻り値のhandleのチェック
    BOOST_CHECK( it->second.handle != NULL );
    // unit_test[14]  load_module 指定したモジュールのdlsymの戻り値のcreate_funcのチェック
    BOOST_CHECK( it->second.create_func != NULL );
    // unit_test[15]  load_module 指定したモジュールのdlsymの戻り値のdestroy_funcのチェック
    BOOST_CHECK( it->second.destroy_func != NULL );

    // unit_test[16]  load_moduleメソッドのテスト（正常系）同じモジュールを再ロードした場合のチェック
    l7vs::protocol_module_base*        protomod4 = NULL;
    try{
        protomod4 = control.load_module( PM1 );
    }
    catch(...){
        BOOST_ERROR( "exception : load_module" );
    }
     BOOST_CHECK( protomod4 != NULL );
    // unit_test[17]  load_module 前回ロードしたモジュールのインスタンスと別のインスタンスであることのチェック
    BOOST_CHECK( protomod3 != protomod4 );

    // unit_test[18]  load_module ロードモジュールのマップのロードしたモジュールの参照回数が増加していることのチェック1→2
    it = loadmodulemap.find( PM1 );
    BOOST_CHECK_EQUAL( it->second.ref_count, (unsigned int)2 );

    // unit_test[19]  load_moduleメソッドのテスト（正常系）前回のモジュールをロードしたまま、新たに別のモジュールをロードした場合のチェック
    l7vs::protocol_module_base*        protomod5 = NULL;
    try{
        protomod5 = control.load_module( PM2 );
    }
    catch(...){
        BOOST_ERROR( "exception : load_module" );
    }
     BOOST_CHECK( protomod5 != NULL );
    // unit_test[20]  load_module 新たにロードしたモジュールが別のインスタンスであることのチェック
    BOOST_CHECK( protomod4 != protomod5 );
    // unit_test[21]  load_module 新たにロードモジュールのマップに追加され、マップのサイズが変わっていることのチェック
    BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)2 );
    // load_module ロードモジュールのマップのロードしたモジュールの参照回数が増加していることのチェック0→1
    it = loadmodulemap.find( PM2 );
    BOOST_CHECK_EQUAL( it->second.ref_count, (unsigned int)1 );

    control.unload_module( protomod3 );
    control.unload_module( protomod4 );
    control.unload_module( protomod5 );
    control.finalize();
}

void    protocol_module_control_unload_test(){
    protocol_module_control_testclass& control = protocol_module_control_testclass::getInstance();
    control.initialize( "." );

    // unit_test[22]  unload_moduleメソッドのテスト（正常系） 1回だけロードしたモジュールをアンロードする場合のチェック
    l7vs::protocol_module_base*        protomod1 = NULL;
    try{
        protomod1 = control.load_module( PM1 );
    }
    catch(...){
        BOOST_ERROR( "exception : load_module" );
    }
    control.unload_module( protomod1 );

    // unit_test[23]  unload_moduleメソッドのテスト（正常系） 2回ロードしたモジュールをアンロードする場合のチェック
    l7vs::protocol_module_base*        protomod2 = NULL;
    try{
        protomod2 = control.load_module( PM1 );
    }
    catch(...){
        BOOST_ERROR( "exception : load_module" );
    }
    l7vs::protocol_module_base*        protomod3 = NULL;
    try{
        protomod3 = control.load_module( PM1 );
    }
    catch(...){
        BOOST_ERROR( "exception : load_module" );
    }

    protocol_module_control::name_module_info_map& loadmodulemap = control.get_loadmodule_map();
    BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)1 );
    protocol_module_control::name_module_info_map::iterator it;
    it = loadmodulemap.find( PM1 );

    control.unload_module( protomod2 );
    BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)1 );
    // unit_test[24]  unload_module ロードモジュールのマップのロードしたモジュールの参照回数が減少していることのチェック2→1
    it = loadmodulemap.find( PM1 );
    BOOST_CHECK_EQUAL( it->second.ref_count, (unsigned int)1 );

    control.unload_module( protomod3 );
    // unload_module ロードモジュールのマップのロードしたモジュールの参照回数が減少していることのチェック1→0
    it = loadmodulemap.find( PM1 );
    BOOST_CHECK_EQUAL( it->second.ref_count, (unsigned int)0 );
    // unit_test[25]  unload_module ロードモジュールのマップから削除されたことのチェック
    BOOST_CHECK_EQUAL( loadmodulemap.size(), (size_t)0 );

    // unit_test[26]  unload_moduleメソッドのテスト（異常系） 引数がNULLの場合のチェック
    l7vs::protocol_module_base*        protomod4 = NULL;
    control.unload_module( protomod4 );
    BOOST_CHECK( dlerror() == NULL );

#if 0
    // unit_test[]  unload_moduleメソッドのテスト（異常系） アンロードするモジュールを2回unloadした場合のチェック
    l7vs::protocol_module_base*        protomod5 = NULL;
    try{
        protomod5 = control.load_module( PM1 );
    }
    catch(...){
        BOOST_ERROR( "exception : load_module" );
    }
    control.unload_module( protomod5 );
    BOOST_CHECK( dlerror() == NULL );
    control.unload_module( protomod5 );
    BOOST_CHECK( dlerror() == NULL );
#endif
    control.finalize();

}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){

//    Logger logger;
//    l7vs::Parameter param;

    // create unit test suite
    test_suite* ts = BOOST_TEST_SUITE( "protocol_module_control" );

    // add test case to test suite
    ts->add( BOOST_TEST_CASE( &protocol_module_control_getInstance_test ) );
    ts->add( BOOST_TEST_CASE( &protocol_module_control_initialize_test ) );
    ts->add( BOOST_TEST_CASE( &protocol_module_control_finalize_test ) );
    ts->add( BOOST_TEST_CASE( &protocol_module_control_load_test ) );
    ts->add( BOOST_TEST_CASE( &protocol_module_control_unload_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}

