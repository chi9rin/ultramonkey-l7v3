
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>


#include "tcp_session_stub.h"
#include "../../src/virtualservice_base.cpp"
#include "../../src/virtualservice_tcp.cpp"

#include "stub.h"
#include "./parameter_stub/parameter.h"


#include "virtualservice.h"
#include "vs_tcp.h"

#include "protocol_module_test1.h"
#include "schedule_module_test1.h"

using namespace boost::unit_test;

typedef     boost::asio::ip::tcp::endpoint    tcp_ep_type;
typedef     boost::asio::ip::udp::endpoint    udp_ep_type;

extern l7vs::session_result_message    result;
int    counter;

//Acceptテスト用Client
void    client(){
    boost::system::error_code    b_err;

    boost::asio::io_service    dispatcher;
    boost::asio::ip::tcp::socket    sock( dispatcher );
//    std::cout << "connect" << std::endl;
    sock.connect( tcp_ep_type( boost::asio::ip::address_v4::loopback() , (60000) ) ,b_err);
    if(b_err){
        //ERROR
        std::cout << "connection error [" << b_err.message() << "]" << std::endl;
        return;
    }else{
        std::cout << "connect" << std::endl;
    }

    sock.close( b_err );
}

//sorry状態にならないパターン
void    virtualservice_tcp_sorry_test1(){
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------sorry_test1" );
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code    test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    elem1;

    //set element value
    elem1.udpmode                    = false;
    elem1.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name        = "PMtest1";
    elem1.schedule_module_name        = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection        = 1234LL;
    elem1.sorry_endpoint            = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag                = false;
    elem1.sorry_fwdmode             = l7vs::virtualservice_element::FWD_MASQ;
    elem1.qos_upstream                = 65535ULL;
    elem1.qos_downstream            = 32767ULL;
    elem1.ssl_file_name            = "";
    elem1.sorry_flag                =0;

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );
    //boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    //sorry状態かチェック
    l7vs::session_thread_control* stc1 = NULL;
    l7vs::tcp_session* sess1 = NULL;
    
    vs->get_active_sessions().pop(sess1, stc1);

    // unit_test[xx] sorry_flag test
    //BOOST_CHECK( NULL != sess1 );

    //BOOST_CHECK_EQUAL(sess1->get_sorry_flag(), false);

    //戻しておく
    //vs->get_active_sessions().insert(sess1, stc1);

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();
std::cout << "!" << std::endl;

    usleep( 1000 );
    //cl_thread.join();
    vs_main.join();
std::cout << "!" << std::endl;

    vs_err.setter(false,"");
   // vs->finalize( vs_err );
std::cout << "!" << std::endl;
    //delete vs;
}

//vsオプションでsorry状態にしたパターン
void    virtualservice_tcp_sorry_test2(){
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------sorry_test2" );
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code    test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    elem1;

    //set element value
    elem1.udpmode                    = false;
    elem1.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name        = "PMtest1";
    elem1.schedule_module_name        = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection        = 1234LL;
    elem1.sorry_endpoint            = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag                = false;
    elem1.sorry_fwdmode             = l7vs::virtualservice_element::FWD_MASQ;
    elem1.qos_upstream                = 65535ULL;
    elem1.qos_downstream            = 32767ULL;
    elem1.ssl_file_name            = "";
    elem1.sorry_flag                =1;

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );
    //boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    //usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    //sorry状態かチェック
    l7vs::session_thread_control* stc1 = NULL;
    l7vs::tcp_session* sess1 = NULL;
    
    vs->get_active_sessions().pop(sess1, stc1);

    // unit_test[xx] sorry_flag test
    //BOOST_CHECK( NULL != sess1 );

    //BOOST_CHECK_EQUAL(sess1->get_sorry_flag(), true);

    //戻しておく
    vs->get_active_sessions().insert(sess1, stc1);

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();
std::cout << "!" << std::endl;

    usleep( 1000 );
    //cl_thread.join();
    vs_main.join();
std::cout << "!" << std::endl;

    vs_err.setter(false,"");
   // vs->finalize( vs_err );
std::cout << "!" << std::endl;
    //delete vs;
}

//接続数ででsorry状態になったパターン
void    virtualservice_tcp_sorry_test3(){
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------sorry_test3" );
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code    test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    elem1;

    //set element value
    elem1.udpmode                    = false;
    elem1.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name        = "PMtest1";
    elem1.schedule_module_name        = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection        = 4LL;
    elem1.sorry_endpoint            = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag                = false;
    elem1.sorry_fwdmode             = l7vs::virtualservice_element::FWD_MASQ;
    elem1.qos_upstream                = 65535ULL;
    elem1.qos_downstream            = 32767ULL;
    elem1.ssl_file_name            = "";
    elem1.sorry_flag                =0;

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    //接続数を増やす
    vs->connection_active( tcp_ep_type() );
    vs->connection_active( tcp_ep_type() );
    vs->connection_active( tcp_ep_type() );
    vs->connection_active( tcp_ep_type() );
    std::cout << "connection=" << vs->get_active_count() << std::endl;

    usleep( 2000000 );
    //boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    //usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    //sorry状態かチェック
    l7vs::session_thread_control* stc1 = NULL;
    l7vs::tcp_session* sess1 = NULL;
    
    vs->get_active_sessions().pop(sess1, stc1);

    // unit_test[xx] sorry_flag test
    //BOOST_CHECK( NULL != sess1 );
    //BOOST_CHECK_EQUAL(sess1->get_sorry_flag(), true);

    //戻しておく
    vs->get_active_sessions().insert(sess1, stc1);

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();
std::cout << "!" << std::endl;

    usleep( 1000 );
    //cl_thread.join();
    vs_main.join();
std::cout << "!" << std::endl;

    vs_err.setter(false,"");
   // vs->finalize( vs_err );
std::cout << "!" << std::endl;
    //delete vs;
}

//接続数増えてもsorry_maxconnectionが設定されていないパターン
void    virtualservice_tcp_sorry_test4(){
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------sorry_test4" );
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code    test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    elem1;

    //set element value
    elem1.udpmode                    = false;
    elem1.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name        = "PMtest1";
    elem1.schedule_module_name        = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection        = 0LL;
    elem1.sorry_endpoint            = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag                = false;
    elem1.sorry_fwdmode             = l7vs::virtualservice_element::FWD_MASQ;
    elem1.qos_upstream                = 65535ULL;
    elem1.qos_downstream            = 32767ULL;
    elem1.ssl_file_name            = "";
    elem1.sorry_flag                =0;

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    //接続数を増やす
    vs->connection_active( tcp_ep_type() );
    vs->connection_active( tcp_ep_type() );
    vs->connection_active( tcp_ep_type() );
    vs->connection_active( tcp_ep_type() );
    std::cout << "connection=" << vs->get_active_count() << std::endl;

    //usleep( 2000000 );
    //boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    //usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    //sorry状態かチェック
    l7vs::session_thread_control* stc1 = NULL;
    l7vs::tcp_session* sess1 = NULL;
    
    vs->get_active_sessions().pop(sess1, stc1);

    // unit_test[xx] sorry_flag test
    //BOOST_CHECK( NULL != sess1 );
    //BOOST_CHECK_EQUAL(sess1->get_sorry_flag(), false);

    //戻しておく
    vs->get_active_sessions().insert(sess1, stc1);

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();
std::cout << "!" << std::endl;

    usleep( 1000 );
    //cl_thread.join();
    vs_main.join();
std::cout << "!" << std::endl;

    vs_err.setter(false,"");
   // vs->finalize( vs_err );
std::cout << "!" << std::endl;
    //delete vs;
}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    // create unit test suite
    test_suite* ts = BOOST_TEST_SUITE( "virtualservice_base_sorry_test" );

    // add test case to test suite
    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_sorry_test1 ) );
    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_sorry_test2 ) );
    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_sorry_test3 ) );
    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_sorry_test4 ) );

    framework::master_test_suite().add( ts );

    return 0;
}
