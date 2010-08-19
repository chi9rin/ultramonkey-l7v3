
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "stub.h"
#include "./parameter_stub/parameter.h"


#include "virtualservice.h"
#include "vs_tcp.h"

#include "protocol_module_test1.h"
#include "schedule_module_test1.h"

using namespace boost::unit_test;

typedef    boost::asio::ip::tcp::endpoint    tcp_ep_type;
typedef    boost::asio::ip::udp::endpoint    udp_ep_type;


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


//test case1  create,initialize,run,stop,finalize,destroy(normal case)
void    virtualservice_tcp_test1(){
    counter = 1;

    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().param_exist_flag() = false;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    //set element value
    element.udpmode                    = false;
    element.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    element.udp_recv_endpoint        = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    element.realserver_vector.clear();
    element.protocol_module_name    = "PMtest1";
    element.schedule_module_name    = "SMtest1";
    element.protocol_args.clear();
    element.protocol_args.push_back( "testarg" );
    element.protocol_args.push_back( "testarg2" );
    element.sorry_maxconnection        = 1234LL;
    element.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    element.sorry_flag                = false;
    element.qos_upstream            = 65535ULL;
    element.qos_downstream            = 32767ULL;
   
    element.ssl_file_name = "";
 
    // unit_test[1]  object create
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------1" );
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );

    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( element == vs->get_element() );
    l7vs::error_code    vs_err;
    // unit_test[2]  initialize method call
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------2" );
    BOOST_CHECK( NULL == vs->get_protocol_module() );
    BOOST_CHECK( NULL == vs->get_schedule_module() );

    result.flag = false;
    vs_err.setter(false,"");
    vs->initialize( vs_err );

    bool result_flg;

    if( vs_err == true ) {
        result_flg = true;
    } else if ( vs_err == false ) {
        result_flg = false;
    }

    BOOST_CHECK( result_flg == false );

    //protocol_module_controlのモジュールロードを呼んでいるか(モジュールロード正常終了)
    BOOST_CHECK( NULL != vs->get_protocol_module() );
    //schedule_module_controlのモジュールロードを呼んでいるか(モジュールロード正常終了)
    BOOST_CHECK( NULL != vs->get_schedule_module() );

    unsigned int pool_default_size = l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT;
    unsigned int pool_size = vs->get_pool_sessions().size();

std::cout << "pool_size = [" << pool_size << "]\n";

    //session_poolを作成しているか(デフォルトサイズで作成)
    BOOST_CHECK( pool_default_size == pool_size );

    // set option
    bool& defer_accept_opt = vs->get_defer_accept_opt();
    defer_accept_opt = true;
    int& defer_accept_val = vs->get_defer_accept_val();
    defer_accept_val = 1;

    // unit_test[3]  run method test
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------3" );
    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );

    usleep( 1500000 );
    //1秒待って、ProtocolModule/ScheduleModuleのreplication_interruptが呼ばれることを確認
    //デフォルト設定は500msなので2回
    BOOST_MESSAGE( debugg_flug_struct::getInstance().get_pm_rep_count() );
    BOOST_MESSAGE( debugg_flug_struct::getInstance().get_sm_rep_count() );
    BOOST_CHECK( 2 == debugg_flug_struct::getInstance().get_pm_rep_count() );
    BOOST_CHECK( 2 == debugg_flug_struct::getInstance().get_sm_rep_count() );

    // unit_test[54]  set TCP_DEFER_ACCEPT test
    int val = 0;
    socklen_t len = sizeof(val);
    boost::system::error_code ec;
    int err = ::getsockopt(vs->get_acceptor().native(),IPPROTO_TCP,TCP_DEFER_ACCEPT,&val,&len);
    BOOST_CHECK(!err);
    BOOST_CHECK(val);
    
    // unit_test[4]  stop method test
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------4" );
    vs->stop();
    usleep( 1000 );

    // unit_test[5]  release_session method test(run直後はwaiting_sessionsに入っているため、release_sessionではプールに戻らない)
    std::cout << counter++ << std::endl;
//     BOOST_MESSAGE( "-------5" );
//     BOOST_CHECK( vs->get_pool_sessions().size() == (l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-1) );
//     BOOST_CHECK( vs->get_waiting_sessions().size() == 1 );
//     BOOST_CHECK( vs->get_active_sessions().size() == 0 );
//     vs->release_session( vs->get_waiting_sessions().begin()->second->get_upthread_id() );
//     BOOST_CHECK( vs->get_pool_sessions().size() == static_cast<size_t>( l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT) );
//     BOOST_CHECK( vs->get_active_sessions().size() == 0 );
//     BOOST_CHECK( vs->get_waiting_sessions().size() == 0 );

    // unit_test[6]  stop method test(call twice)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------6" );
    vs->stop();

    vs_main.join();

    // unit_test[7]  finalize method call
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------7" );
    vs_err.setter(false,"");
    vs->finalize( vs_err );
    BOOST_CHECK( vs_err == false );

    // unit_test[8]  finalizeを2回連続で呼んでみる
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------8" );
    vs_err.setter(false,"");
    vs->finalize( vs_err );
    BOOST_CHECK( vs_err == false );


    // unit_test[9]  object destroy
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------9" );
    delete vs;
}

//test case2 method call
void    virtualservice_tcp_test2(){
    l7vs::error_code    vs_err;

    bool result_flg;
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    //set element value



    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );
    // unit_test[10]  initialize method call(poolsize from parameter file:value = 512)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------10" );
    debugg_flug_struct::getInstance().param_exist_flag() = true;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
    vs_err.setter(false,"");
    vs->initialize( vs_err );
    BOOST_CHECK( SESSION_POOL_NUM_PARAM == vs->get_pool_sessions().size() );
    BOOST_CHECK( vs_err == false );
    vs->finalize( vs_err );
    BOOST_CHECK( vs_err == false );
    delete vs;
    vs = NULL;

    vs = new l7vs::vs_tcp( vsd, rep, element );
    // unit_test[11]  initialize method call(procotol_module_control::module_load error case)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------11" );
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = true;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
    BOOST_CHECK( NULL == vs->get_protocol_module() );
    BOOST_CHECK( NULL == vs->get_schedule_module() );
    vs->initialize( vs_err );
    BOOST_CHECK( NULL == vs->get_protocol_module() );
    BOOST_CHECK( NULL == vs->get_schedule_module() );
    BOOST_CHECK( vs_err == true );
    BOOST_MESSAGE( vs_err.get_message() );
    BOOST_CHECK( PROTOMOD_LOAD_ERROR_MSG == vs_err.get_message() );

    vs->finalize( vs_err );
    delete vs;

    vs = new l7vs::vs_tcp( vsd, rep, element );
    // unit_test[12]  initialize method call(procotol_module_control::module_load error case)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------12" );
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = true;
    BOOST_CHECK( NULL == vs->get_protocol_module() );
    BOOST_CHECK( NULL == vs->get_schedule_module() );
    vs->initialize( vs_err );
    BOOST_CHECK( NULL != vs->get_protocol_module() );
    BOOST_CHECK( NULL == vs->get_schedule_module() );
    BOOST_CHECK( vs_err == true );
    BOOST_MESSAGE( vs_err.get_message() );
    BOOST_CHECK( SCHEDMOD_LOAD_ERROR_MSG == vs_err.get_message() );

    vs->finalize( vs_err );
    delete vs;

    vs = new l7vs::vs_tcp( vsd, rep, element );
    vs->initialize( vs_err );
    l7vs::vs_tcp*    vs2 = new l7vs::vs_tcp( vsd, rep, element );
    vs2->initialize( vs_err );
    // unit_test[13]  initialize method call(procotol_module_control::module_load error case)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------13" );
    BOOST_CHECK( *vs == *vs2 );
    vs->finalize( vs_err );
    delete vs;
    vs2->finalize( vs_err );
    delete vs2;

    // unit_test[14]  initialize method call(procotol_module_control::module_load error case)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------14" );
    element.udpmode = false;
    element.tcp_accept_endpoint    = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
    vs = new l7vs::vs_tcp( vsd, rep, element );
    vs->initialize( vs_err );

    element.tcp_accept_endpoint    = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
    vs2 = new l7vs::vs_tcp( vsd, rep, element );
    vs2->initialize( vs_err );

    BOOST_CHECK( *vs != *vs2 );
    vs->finalize( vs_err );
    delete vs;
    vs2->finalize( vs_err );
    delete vs2;

    element.udpmode = false;
    element.tcp_accept_endpoint    = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
    vs = new l7vs::vs_tcp( vsd, rep, element );
    vs->initialize( vs_err );

    // unit_test[15]  add_realserver method call
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------15" );
    l7vs::virtualservice_element    add_element;
    add_element.udpmode = false;
    add_element.tcp_accept_endpoint = element.tcp_accept_endpoint;
    for( unsigned int i = 0; i < 10; ++i ){
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) );
        add_element.realserver_vector.push_back( rs_elem );
    }

    vs_err.setter(false,"");
    vs->add_realserver( add_element, vs_err );
    std::list<l7vs::realserver>&    rslist = vs->get_rs_list();
    //RSの数が10であることを確認
    BOOST_CHECK( 10 == rslist.size() );
    //vs_errがfalseであることを確認
    if( vs_err == true ) {
        result_flg = true;
    } else if ( vs_err == false ) {
        result_flg = false;
    }
    BOOST_CHECK( result_flg == false );
    BOOST_MESSAGE( vs_err.get_message() );
    BOOST_MESSAGE( element.tcp_accept_endpoint );
    BOOST_MESSAGE( add_element.tcp_accept_endpoint );
    //各要素が一致するか確認
    for( std::list<l7vs::realserver>::iterator    itr = rslist.begin();
        itr != rslist.end(); ++itr ){
        for( int i = 0; i <= 10; ++i ){
            if( 10 <= i )BOOST_ERROR( "error add_realserver : endpoint unmatch" );
            if( itr->tcp_endpoint == tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) ) ){
                BOOST_CHECK( itr->tcp_endpoint == tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) ) );
                break;
            }
        }
    }

    // unit_test[16]  add_realserver method call(VSのEndpointが違う場合はエラーが返ること)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------16" );
    l7vs::virtualservice_element    add_err_element;
    add_err_element.tcp_accept_endpoint = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (80) );
    l7vs::realserver_element        rs_adderr_elem;
    rs_adderr_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.100.10" ), (8080) );
    add_err_element.realserver_vector.push_back( rs_adderr_elem );
    vs->add_realserver( add_err_element, vs_err );
    //RSの数が10のままであることを確認
    BOOST_CHECK( 10 == rslist.size() );
    //vs_errがtrueでメッセージが存在することを確認
    BOOST_CHECK( vs_err == true );
    BOOST_MESSAGE( vs_err.get_message() );

    // unit_test[17]  add_realserver method call(既にあるRS(エンドポイントが同じ)追加はエラーが返ること)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------17" );
    add_err_element.tcp_accept_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
    rs_adderr_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
    add_err_element.realserver_vector.clear();
    add_err_element.realserver_vector.push_back( rs_adderr_elem );
    vs->add_realserver( add_err_element, vs_err );
    //RSの数が10のままであることを確認
    BOOST_CHECK( 10 == rslist.size() );
    //vs_errがtrueでメッセージが存在することを確認
    BOOST_CHECK( vs_err == true );
    BOOST_MESSAGE( vs_err.get_message() );

    // unit_test[18]  del_realserver method call(VSのエンドポイントが異なる場合はエラーが返ること)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------18" );
    l7vs::virtualservice_element    del_err_element;
    l7vs::realserver_element        rs_delerr_elem;
    del_err_element.tcp_accept_endpoint    = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (80) );
    rs_delerr_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
    del_err_element.realserver_vector.clear();
    del_err_element.realserver_vector.push_back( rs_delerr_elem );
    vs->del_realserver( del_err_element, vs_err );
    //RSの数が10のままであることを確認
    BOOST_CHECK( 10 == rslist.size() );
    //vs_errがtrueでメッセージが存在することを確認
    BOOST_CHECK( vs_err == true );
    BOOST_MESSAGE( vs_err.get_message() );

    // unit_test[19]  del_realserver method call(一致するRSが無い場合はエラーが返ること)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------19" );
    del_err_element.tcp_accept_endpoint    = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
    rs_delerr_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "100.90.80.70" ), (60000) );
    del_err_element.realserver_vector.clear();
    del_err_element.realserver_vector.push_back( rs_delerr_elem );
    vs->del_realserver( del_err_element, vs_err );
    //RSの数が10のままであることを確認
    BOOST_CHECK( 10 == rslist.size() );
    //vs_errがtrueでメッセージが存在することを確認
    BOOST_CHECK( vs_err == true );
    BOOST_MESSAGE( vs_err.get_message() );

    // unit_test[20]  del_realserver method call
    //addしたものと同じものを指定して、全削除されることを確認
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------20" );
    vs_err.setter(false,"");
    vs->del_realserver( add_element, vs_err );
    //RSの数が0であることを確認
    BOOST_CHECK( 0 == rslist.size() );
    //vs_errがfalseでメッセージが存在しないことを確認
    if( vs_err == true ) {
        result_flg = true;
    } else if ( vs_err == false ) {
         result_flg = false;
    }
    BOOST_CHECK( result_flg == false );
    BOOST_MESSAGE( vs_err.get_message() );
    BOOST_CHECK( vs_err.get_message() == "" );

    // unit_test[21]  add_realserver method call(再度追加)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------21" );
    vs_err.setter(false,"");
    vs->add_realserver( add_element, vs_err );
    //RSの数が10であることを確認
    BOOST_CHECK( 10 == rslist.size() );
    //各要素が一致するか確認
    for( std::list<l7vs::realserver>::iterator    itr = rslist.begin();
        itr != rslist.end(); ++itr ){
        for( int i = 0; i <= 10; ++i ){
            if( 10 <= i )BOOST_ERROR( "error add_realserver : endpoint unmatch" );
            if( itr->tcp_endpoint == tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) ) ){
                BOOST_CHECK( itr->tcp_endpoint == tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) ) );
                break;
            }
        }
    }

    // unit_test[22]  connection_active method call
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------22" );
    tcp_ep_type    ep = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
    vs->connection_active( ep );
    for( std::list<l7vs::realserver>::iterator    itr = rslist.begin();
         itr != rslist.end(); ++itr ){
        if( itr->tcp_endpoint == ep ){
            BOOST_CHECK( 1 == itr->get_active() );
            break;
        }
    }
    // unit_test[xx]  active_connection test
    BOOST_CHECK_EQUAL( vs->get_active_count(), 1ULL );

    // unit_test[23]  connection_inactive method call
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------23" );
    vs->connection_inactive( ep );
    for( std::list<l7vs::realserver>::iterator    itr = rslist.begin();
         itr != rslist.end(); ++itr ){
        if( itr->tcp_endpoint == ep ){
            BOOST_CHECK( 0 == itr->get_active() );
            BOOST_CHECK( 1 == itr->get_inact() );
            break;
        }
    }
    // unit_test[xx]  active_connection test
    BOOST_CHECK_EQUAL( vs->get_active_count(), 0ULL );

    vs_err.setter(false,"");
    vs->finalize( vs_err );
    delete vs;
}

//test case3
void    virtualservice_tcp_test3(){
    l7vs::error_code            vs_err;
    boost::system::error_code    test_err;
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    //set element value

    element.udpmode                    = false;
    element.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    element.udp_recv_endpoint        = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    element.realserver_vector.clear();
    element.protocol_module_name    = "PMtest1";
    element.schedule_module_name    = "SMtest1";
    element.protocol_args.clear();
    element.sorry_maxconnection        = 1234LL;
    element.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    element.sorry_flag                = false;
    element.qos_upstream            = 65535ULL;
    element.qos_downstream            = 32767ULL;

    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );
    vs_err.setter(false,"");
    vs->initialize( vs_err );
    BOOST_CHECK( vs_err == false );
    BOOST_MESSAGE( vs_err.get_message() );

    // unit_test[24]  replication dataが作成できるか確認(初回作成)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------24" );
    //まず、stubのレプリケーションエリア作成をする
    debugg_flug_struct::getInstance().create_rep_area();

    //レプリケーションデータ作成
    vs->call_handle_replication_interrupt( test_err );

    l7vs::virtualservice_base::replication_header*    rep_head =
        reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
    //data_numが1になってることを確認
    BOOST_CHECK( rep_head->data_num == 1 );
//    l7vs::virtualservice_base::replication_data*    rep_data =
//        reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );

    // unit_test[25]  edit_virtualserviceのテスト
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------25" );
    //変更前の値確認
    BOOST_CHECK( false == vs->get_element().udpmode );
    BOOST_CHECK( tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) ) ==  vs->get_element().tcp_accept_endpoint );
    BOOST_CHECK( udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) ) ==  vs->get_element().udp_recv_endpoint );
    BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
    BOOST_CHECK( "SMtest1" == vs->get_element().schedule_module_name );
    BOOST_CHECK( 1234LL == vs->get_element().sorry_maxconnection );
    BOOST_CHECK( tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) ) == vs->get_element().sorry_endpoint );
    BOOST_CHECK( false == vs->get_element().sorry_flag );
    BOOST_CHECK( 65535ULL == vs->get_element().qos_upstream );
    BOOST_CHECK( 32767ULL == vs->get_element().qos_downstream );

    //変更値の設定
    l7vs::virtualservice_element    elem2;
    elem2.udpmode                    = false;
    elem2.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem2.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem2.realserver_vector.clear();
    elem2.protocol_module_name        = "PMtest1";
    elem2.schedule_module_name        = "SMtest2";
    elem2.protocol_args.clear();
    elem2.sorry_maxconnection        = 5678LL;
    elem2.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.86" ), (80) );
    elem2.sorry_flag                = true;
    elem2.qos_upstream                = 1024ULL;
    elem2.qos_downstream            = 4096ULL;

    debugg_flug_struct::getInstance().pmcontrol_err_flag() = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag() = false;
    vs_err.setter(false,"");
    vs->edit_virtualservice( elem2, vs_err );
    BOOST_CHECK( vs_err == false );
    BOOST_MESSAGE( vs_err.get_message() );
    //変更後の確認
    BOOST_CHECK( false == vs->get_element().udpmode );
    BOOST_CHECK( elem2.tcp_accept_endpoint    ==  vs->get_element().tcp_accept_endpoint );
    BOOST_CHECK( element.udp_recv_endpoint    ==  vs->get_element().udp_recv_endpoint );
    BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
    BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
    BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
    BOOST_CHECK( elem2.sorry_endpoint == vs->get_element().sorry_endpoint );
    BOOST_CHECK( true == vs->get_element().sorry_flag );
    BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
    BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

    // unit_test[26]  edit_virtualserviceのテスト(udpmodeが一致しないものはエラーが返る＆値が反映されない)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------26" );
    l7vs::virtualservice_element    elem3;
    elem3.udpmode                    = true;
    elem3.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem3.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
    elem3.realserver_vector.clear();
    elem3.protocol_module_name        = "PMtest1";
    elem3.schedule_module_name        = "SMtest2";
    elem3.protocol_args.clear();
    elem3.sorry_maxconnection        = 65535LL;
    elem3.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.86" ), (80) );
    elem3.sorry_flag                = false;
    elem3.qos_upstream                = 1024ULL;
    elem3.qos_downstream            = 4096ULL;

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem3, vs_err );
    //vs_errがtrueなのをチェック
    BOOST_CHECK( vs_err == true );

    BOOST_CHECK( false == vs->get_element().udpmode );
    BOOST_CHECK( elem2.tcp_accept_endpoint    ==  vs->get_element().tcp_accept_endpoint );
    BOOST_CHECK( element.udp_recv_endpoint    ==  vs->get_element().udp_recv_endpoint );
    BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
    BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
    BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
    BOOST_CHECK( elem2.sorry_endpoint == vs->get_element().sorry_endpoint );
    BOOST_CHECK( true == vs->get_element().sorry_flag );
    BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
    BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

    // unit_test[27]  edit_virtualserviceのテスト(tcp_accept_endpointが一致しないものはエラーが返る＆値が反映されない)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------27" );
    elem3.udpmode                    = false;
    elem3.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (8080) );
    elem3.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
    elem3.protocol_module_name        = "PMtest1";

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem3, vs_err );
    //vs_errがtrueなのをチェック
    BOOST_CHECK( vs_err == true );

    BOOST_CHECK( false == vs->get_element().udpmode );
    BOOST_CHECK( elem2.tcp_accept_endpoint    ==  vs->get_element().tcp_accept_endpoint );
    BOOST_CHECK( element.udp_recv_endpoint    ==  vs->get_element().udp_recv_endpoint );
    BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
    BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
    BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
    BOOST_CHECK( elem2.sorry_endpoint == vs->get_element().sorry_endpoint );
    BOOST_CHECK( true == vs->get_element().sorry_flag );
    BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
    BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

    // unit_test[28]  edit_virtualserviceのテスト(protocol_module_nameが一致しないものはエラーが返る＆値が反映されない)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------28" );
    elem3.udpmode                    = false;
    elem3.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (60000) );
    elem3.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
    elem3.protocol_module_name        = "PMtest2";

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem3, vs_err );
    //vs_errがtrueなのをチェック
    BOOST_CHECK( vs_err == true );

    BOOST_CHECK( false == vs->get_element().udpmode );
    BOOST_CHECK( elem2.tcp_accept_endpoint    ==  vs->get_element().tcp_accept_endpoint );
    BOOST_CHECK( element.udp_recv_endpoint    ==  vs->get_element().udp_recv_endpoint );
    BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
    BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
    BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
    BOOST_CHECK( elem2.sorry_endpoint == vs->get_element().sorry_endpoint );
    BOOST_CHECK( true == vs->get_element().sorry_flag );
    BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
    BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

    // unit_test[29]  edit_virtualserviceのテスト(sorry_endpointの変更無し)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------29" );
    elem2.udpmode                    = false;
    elem2.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem2.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
    elem2.realserver_vector.clear();
    elem2.protocol_module_name        = "PMtest1";
    elem2.schedule_module_name        = "SMtest2";
    elem2.protocol_args.clear();
    elem2.sorry_maxconnection        = 5678LL;
    elem2.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "0.0.0.0" ), (0) );
    elem2.sorry_flag                = true;
    elem2.qos_upstream                = 1024ULL;
    elem2.qos_downstream            = 4096ULL;

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem2, vs_err );
    //vs_errがtrueなのをチェック
    BOOST_CHECK( vs_err == false );

    BOOST_CHECK( false == vs->get_element().udpmode );
    BOOST_CHECK( elem2.tcp_accept_endpoint    ==  vs->get_element().tcp_accept_endpoint );
    BOOST_CHECK( element.udp_recv_endpoint    ==  vs->get_element().udp_recv_endpoint );
    BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
    BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
    BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
    BOOST_CHECK( tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.86" ), (80) ) == vs->get_element().sorry_endpoint );
    BOOST_CHECK( true == vs->get_element().sorry_flag );
    BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
    BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

    // unit_test[30]  edit_virtualserviceのテスト(sorry_endpointクリア)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------30" );
    elem2.udpmode                    = false;
    elem2.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem2.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
    elem2.realserver_vector.clear();
    elem2.protocol_module_name        = "PMtest1";
    elem2.schedule_module_name        = "SMtest2";
    elem2.protocol_args.clear();
    elem2.sorry_maxconnection        = 5678LL;
    elem2.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "255.255.255.255" ), (0) );
    elem2.sorry_flag                = true;
    elem2.qos_upstream                = 1024ULL;
    elem2.qos_downstream            = 4096ULL;

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem2, vs_err );
    //vs_errがtrueなのをチェック
    BOOST_CHECK( vs_err == false );

    BOOST_CHECK( false == vs->get_element().udpmode );
    BOOST_CHECK( elem2.tcp_accept_endpoint    ==  vs->get_element().tcp_accept_endpoint );
    BOOST_CHECK( element.udp_recv_endpoint    ==  vs->get_element().udp_recv_endpoint );
    BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
    BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
    BOOST_CHECK( 0LL == vs->get_element().sorry_maxconnection );
    BOOST_CHECK( tcp_ep_type() == vs->get_element().sorry_endpoint );
    BOOST_CHECK( false == vs->get_element().sorry_flag );
    BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
    BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

//    // unit_test[31]  replication dataが作成できるか確認(既にあるデータを上書き)
//    // V2-> V3 時点での機能確認が必要であったが、機能が保証されていない。
//    // 作業後回し(handle_replication_interrupt)
//    std::cout << counter++ << std::endl;
//    BOOST_MESSAGE( "-------31" );
//    vs->call_handle_replication_interrupt( test_err );
//    rep_head =    reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
//    //data_numが1になってることを確認
//    BOOST_CHECK( rep_head->data_num == 1 );
//    rep_data =    reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );
//    //udpmode
//    BOOST_CHECK( rep_data->udpmode == false );
//    //tcp_endpoint
//    tmp_tcp_ep.str( "" );
//    tmp_tcp_ep << elem2.tcp_accept_endpoint;
//    BOOST_CHECK( 0 == strncmp( rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) );
//    //udp_endpoint
//    tmp_udp_ep.str( "" );
//    tmp_udp_ep << element.udp_recv_endpoint;
//    BOOST_CHECK( 0 == strncmp( rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47 ) );
//    //sorry_maxconnection
//    BOOST_CHECK( rep_data->sorry_maxconnection == 0LL );
//    //sorry_endpoint
//    tmp_sorry_ep.str( "" );
//    tmp_sorry_ep << tcp_ep_type();
//    BOOST_CHECK( 0 == strncmp( rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47 ) );
//    //sorry_flag
//    BOOST_CHECK( rep_data->sorry_flag == false );
//    //qos_up
//    BOOST_CHECK( rep_data->qos_up == 1024ULL );
//    //qos_down
//    BOOST_CHECK( rep_data->qos_down == 4096ULL );
//
//    // unit_test[32]  vsをもう一つ作って、そこからもレプリケーションデータを作成する
//    // V2-> V3 時点での機能確認が必要であったが、機能が保証されていない。
//    // 作業後回し(handle_replication_interrupt)
//    std::cout << counter++ << std::endl;
//    BOOST_MESSAGE( "-------32" );
//    elem3.udpmode                    = false;
//    elem3.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (65000) );
//    elem3.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
//    elem3.realserver_vector.clear();
//    elem3.protocol_module_name        = "PMtest1";
//    elem3.schedule_module_name        = "SMtest1";
//    elem3.protocol_args.clear();
//    elem3.sorry_maxconnection        = 7890LL;
//    elem3.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.86" ), (80) );
//    elem3.sorry_flag                = false;
//    elem3.qos_upstream                = 14142ULL;
//    elem3.qos_downstream            = 22362ULL;
//
//
//    l7vs::vs_tcp*    vs2 = new l7vs::vs_tcp( vsd, rep, elem3 );
//
//    vs_err.setter(false,"");
//    vs2->initialize( vs_err );
//    BOOST_CHECK( vs_err == false );
//    BOOST_MESSAGE( vs_err.get_message() );
//
//    //data_numが2になってることを確認
//    vs2->call_handle_replication_interrupt( test_err );
//    rep_head =    reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
//    //data_numが2になってることを確認
//    BOOST_CHECK( rep_head->data_num == 2 );
//    rep_data =    reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );
//    ++rep_data;
//    //udpmode
//    BOOST_CHECK( rep_data->udpmode == false );
//    //tcp_endpoint
//    tmp_tcp_ep.str( "" );
//    tmp_tcp_ep << elem3.tcp_accept_endpoint;
//    BOOST_CHECK( 0 == strncmp( rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) );
//    //udp_endpoint
//    tmp_udp_ep.str( "" );
//    tmp_udp_ep << elem3.udp_recv_endpoint;
//    BOOST_CHECK( 0 == strncmp( rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47 ) );
//    //sorry_maxconnection
//    BOOST_CHECK( rep_data->sorry_maxconnection == 7890LL );
//    //sorry_endpoint
//    tmp_sorry_ep.str( "" );
//    tmp_sorry_ep << elem3.sorry_endpoint;
//    BOOST_CHECK( 0 == strncmp( rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47 ) );
//    //sorry_flag
//    BOOST_CHECK( rep_data->sorry_flag == false );
//    //qos_up
//    BOOST_CHECK( rep_data->qos_up == 14142ULL );
//    //qos_down
//    BOOST_CHECK( rep_data->qos_down == 22362ULL );
//
//    vs_err.setter(false,"");
//    //一端削除
//    vs->finalize( vs_err );
//    delete vs;
//
//    // unit_test[33]  VSを削除したら、レプリケーションデータが読み出せないことを確認
//    std::cout << counter++ << std::endl;
//    BOOST_MESSAGE( "-------33" );
//    vs = new l7vs::vs_tcp( vsd, rep, element );
//    vs_err.setter(false,"");
//    vs->initialize( vs_err );
//
//    //replication dataの確認
//    vs->call_handle_replication_interrupt( test_err );
//    vs2->call_handle_replication_interrupt( test_err );
//    rep_head =    reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
//    //data_numが2になってることを確認
//    BOOST_CHECK( rep_head->data_num == 3 );
//    rep_data =    reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );
//    //udpmode
//    BOOST_CHECK( rep_data->udpmode == element.udpmode );
//    //tcp_endpoint
//    tmp_tcp_ep.str( "" );
//    tmp_tcp_ep << element.tcp_accept_endpoint;
//    BOOST_CHECK( 0 == strncmp( rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) );
//    //udp_endpoint
//    tmp_udp_ep.str( "" );
//    tmp_udp_ep << element.udp_recv_endpoint;
//    BOOST_CHECK( 0 == strncmp( rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47 ) );
//    //sorry_maxconnection
//    BOOST_CHECK( rep_data->sorry_maxconnection == 1234LL );
//    BOOST_MESSAGE( rep_data->sorry_maxconnection );
//    //sorry_endpoint
//    tmp_sorry_ep.str( "" );
//    tmp_sorry_ep << tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
//    BOOST_CHECK( 0 == strncmp( rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47 ) );
//    //sorry_flag
//    BOOST_CHECK( rep_data->sorry_flag == false );
//    //qos_up
//    BOOST_CHECK( rep_data->qos_up == 65535ULL );
//    //qos_down
//    BOOST_CHECK( rep_data->qos_down == 32767ULL );
//
//    //古いデータの確認
//    BOOST_MESSAGE( "sorry max_conn : " << vs->get_element().sorry_maxconnection );
//    BOOST_CHECK( 1234LL == vs->get_element().sorry_maxconnection );
//    BOOST_MESSAGE( "sorry endpoint : " << vs->get_element().sorry_endpoint );
//    BOOST_CHECK( tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) ) 
//                    == vs->get_element().sorry_endpoint );
//    BOOST_MESSAGE( "sorry flag     : " << vs->get_element().sorry_flag );
//    BOOST_CHECK( false == vs->get_element().sorry_flag );
//    BOOST_MESSAGE( "QoS upstream   : " << vs->get_element().qos_upstream );
//    BOOST_CHECK( 65535ULL == vs->get_element().qos_upstream );
//    BOOST_MESSAGE( "QoS downstream : " << vs->get_element().qos_downstream );
//    BOOST_CHECK( 32767ULL == vs->get_element().qos_downstream );
//
//
//    //テストが終わったらStubのレプリケーションエリアを削除
//    debugg_flug_struct::getInstance().create_rep_area();
//
    // unit_test[34]  edit_virtualserviceのテスト(schedule_moduleのロードに失敗するケース：致命的エラー)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------34" );
    debugg_flug_struct::getInstance().smcontrol_err_flag() = true;
    elem2.schedule_module_name        = "SMtest3";
    vs_err.setter(false,"");
    vs->edit_virtualservice( elem2, vs_err );
    //vs_errがtrueなのをチェック
    BOOST_CHECK( vs_err == true );
    BOOST_MESSAGE( vs_err.get_message() );

//    vs_err.setter(false,"");
//    vs2->finalize( vs_err );
//    delete vs2;
    vs_err.setter(false,"");
    vs->finalize( vs_err );
    delete vs;
}

//IPv6関係のテスト
void    virtualservice_tcp_test4(){
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
            tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
    elem1.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name        = "PMtest1";
    elem1.schedule_module_name        = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection        = 1234LL;
    elem1.sorry_endpoint            = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag                = false;
    elem1.qos_upstream                = 65535ULL;
    elem1.qos_downstream            = 32767ULL;

    // unit_test[35]  IPv6アドレスでVS作成
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------35" );
    l7vs::vs_tcp*    vs;
    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    vs_err.setter(false,"");
    vs->initialize( vs_err );
    BOOST_CHECK( vs_err == false );
    BOOST_MESSAGE( vs_err.get_message() );

    // unit_test[36]  IPv6アドレスでレプリケーションデータ作成
    //まず、stubのレプリケーションエリア作成をする
    debugg_flug_struct::getInstance().create_rep_area();
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------36" );
    vs->call_handle_replication_interrupt( test_err );
    l7vs::virtualservice_base::replication_header*    rep_head =    
        reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
    //data_numが0になってることを確認
    BOOST_CHECK( rep_head->data_num == 1 );
    l7vs::virtualservice_base::replication_data*    rep_data =    
        reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );
    //udpmode
    BOOST_CHECK( rep_data->udpmode == false );
    //tcp_endpoint
    tmp_tcp_ep.str( "" );
    tmp_tcp_ep << elem1.tcp_accept_endpoint;
    BOOST_CHECK( 0 == strncmp( rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) );
    //udp_endpoint
    tmp_udp_ep.str( "" );
    tmp_udp_ep << elem1.udp_recv_endpoint;
    BOOST_CHECK( 0 == strncmp( rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47 ) );
    //sorry_maxconnection
    BOOST_CHECK( rep_data->sorry_maxconnection == elem1.sorry_maxconnection );
    //sorry_endpoint
    tmp_sorry_ep.str( "" );
    tmp_sorry_ep << elem1.sorry_endpoint;
    BOOST_CHECK( 0 == strncmp( rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47 ) );
    //sorry_flag
    BOOST_CHECK( rep_data->sorry_flag == elem1.sorry_flag );
    //qos_up
    BOOST_CHECK( rep_data->qos_up == elem1.qos_upstream );
    //qos_down
    BOOST_CHECK( rep_data->qos_down == elem1.qos_downstream );

    //比較用VS作成
    l7vs::vs_tcp*    vs2 = new l7vs::vs_tcp( vsd, rep, elem1 );
    bool    chkflg;
    // unit_test[37]  IPv6アドレスでoperator==のテスト(VSが一致するケース)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------37" );
    chkflg = ( *vs == *vs2 );
    BOOST_CHECK( chkflg );
    // unit_test[38]  IPv6アドレスでoperator!=のテスト(VSが一致するケース)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------38" );
    chkflg = ( *vs != *vs2 );
    BOOST_CHECK( !chkflg );
    delete vs2;
    vs2 = NULL;

    //比較用VS作成
    //set element-2
    l7vs::virtualservice_element    elem2;
    elem2.udpmode                    = false;
    elem2.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v6::loopback(), (55000) );
    elem2.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem2.realserver_vector.clear();
    elem2.protocol_module_name        = "PMtest1";
    elem2.schedule_module_name        = "SMtest1";
    elem2.protocol_args.clear();
    elem2.sorry_maxconnection        = 1234LL;
    elem2.sorry_endpoint            = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem2.sorry_flag                = false;
    elem2.qos_upstream                = 65535ULL;
    elem2.qos_downstream            = 32767ULL;
    vs2 = new l7vs::vs_tcp( vsd, rep, elem2 );
    // unit_test[39]  IPv6アドレスでoperator==のテスト(VSが一致しないケース)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------39" );
    chkflg = ( *vs == *vs2 );
    BOOST_CHECK( !chkflg );
    // unit_test[40]  IPv6アドレスでoperator!=のテスト(VSが一致しないケース)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------40" );
    chkflg = ( *vs != *vs2 );
    BOOST_CHECK( chkflg );
    delete vs2;
    vs2 = NULL;

    // unit_test[41]  IPv6アドレスでVS編集
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------41" );
    l7vs::virtualservice_element    elem3;
    elem3.udpmode                    = false;
    elem3.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
    elem3.udp_recv_endpoint            = 
            udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem3.realserver_vector.clear();
    elem3.protocol_module_name        = "PMtest1";
    elem3.schedule_module_name        = "SMtest1";
    elem3.protocol_args.clear();
    elem3.sorry_maxconnection        = 3333LL;
    elem3.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "255.255.255.255" ), (0) );
    elem3.sorry_flag                = true;
    elem3.qos_upstream                = 10ULL;
    elem3.qos_downstream            = 20ULL;


    vs_err.setter(false,"");
    vs->edit_virtualservice( elem3, vs_err );
    BOOST_CHECK( vs_err == false );
    BOOST_MESSAGE( vs_err.get_message() );
    BOOST_CHECK( vs->get_element().sorry_endpoint == tcp_ep_type() );
    BOOST_CHECK( vs->get_element().sorry_maxconnection == 0LL );
    BOOST_CHECK( vs->get_element().sorry_flag == false );
    BOOST_CHECK( vs->get_element().qos_upstream == elem3.qos_upstream );
    BOOST_CHECK( vs->get_element().qos_downstream == elem3.qos_downstream );

    // unit_test[42]  IPv6アドレスでレプリケーションデータの読み出し：削除したら作りなおしたデータになること
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------42" );

    vs_err.setter(false,"");
    //一旦VSを削除
    vs->finalize( vs_err );
    delete vs;
    //elem3でVS作成
    vs = new l7vs::vs_tcp( vsd, rep, elem3 );
    BOOST_CHECK( vs_err == false );
    vs->call_read_replicationdata();
    BOOST_CHECK( vs->get_element().udpmode == elem3.udpmode );
    BOOST_CHECK( vs->get_element().tcp_accept_endpoint == elem3.tcp_accept_endpoint );
    BOOST_CHECK( vs->get_element().udp_recv_endpoint == elem3.udp_recv_endpoint );
    BOOST_CHECK( vs->get_element().protocol_module_name == elem3.protocol_module_name );
    BOOST_CHECK( vs->get_element().schedule_module_name == elem3.schedule_module_name );
    BOOST_CHECK( vs->get_element().sorry_maxconnection == 0LL );
    BOOST_CHECK( vs->get_element().sorry_endpoint == tcp_ep_type() );
    BOOST_CHECK( vs->get_element().sorry_flag == false );
    BOOST_CHECK( vs->get_element().qos_upstream == elem3.qos_upstream );
    BOOST_CHECK( vs->get_element().qos_downstream == elem3.qos_downstream );


    // unit_test[43]  IPv6アドレスでVS変更(TCP endpoint不一致による失敗ケース)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------43" );
    l7vs::virtualservice_element    elem4;
    elem4.udpmode                    = false;
    elem4.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678:90AB:CDEF:0000:0000:FEDC:BA00" ), (60000) );
    elem4.udp_recv_endpoint            = 
            udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem4.realserver_vector.clear();
    elem4.protocol_module_name        = "PMtest1";
    elem4.schedule_module_name        = "SMtest1";
    elem4.protocol_args.clear();
    elem4.sorry_maxconnection        = 3333LL;
    elem4.sorry_endpoint            = tcp_ep_type();
    elem4.sorry_flag                = true;
    elem4.qos_upstream                = 65535ULL;
    elem4.qos_downstream            = 32767ULL;

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem4, vs_err );
    BOOST_CHECK( vs_err == true );
    BOOST_CHECK( vs->get_element().sorry_maxconnection == 0LL );
    BOOST_MESSAGE( vs->get_element().sorry_maxconnection );
    BOOST_CHECK( vs->get_element().sorry_endpoint == tcp_ep_type() );
    BOOST_MESSAGE( vs->get_element().sorry_endpoint );
    BOOST_CHECK( vs->get_element().sorry_flag == false );
    BOOST_MESSAGE( vs->get_element().sorry_flag );
    BOOST_MESSAGE( vs->get_element().qos_upstream );
    BOOST_CHECK( vs->get_element().qos_upstream == elem3.qos_upstream );
    BOOST_MESSAGE( vs->get_element().qos_downstream );
    BOOST_CHECK( vs->get_element().qos_downstream == elem3.qos_downstream );

    // unit_test[44]  IPv6アドレスでRS追加
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------44" );
    //RSパラメータ設定
    l7vs::virtualservice_element    elem_add_rs1;
    elem_add_rs1.udpmode                    = false;
    elem_add_rs1.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
    elem_add_rs1.udp_recv_endpoint            = 
            udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem_add_rs1.realserver_vector.clear();
    elem_add_rs1.protocol_module_name        = "PMtest1";
    elem_add_rs1.schedule_module_name        = "SMtest1";
    elem_add_rs1.protocol_args.clear();
    elem_add_rs1.sorry_maxconnection        = 1234LL;
    elem_add_rs1.sorry_endpoint                = tcp_ep_type();
    elem_add_rs1.sorry_flag                    = false;
    elem_add_rs1.qos_upstream                = 65535ULL;
    elem_add_rs1.qos_downstream                = 32767ULL;
    for( size_t i = 0; i < 10; ++i ){
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = 
            tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (i+8080) );
        elem_add_rs1.realserver_vector.push_back( rs_elem );
        rs_elem.weight    = i+1;
    }
    vs_err.setter(false,"");
    vs->add_realserver( elem_add_rs1, vs_err );
    BOOST_CHECK( vs_err == false );

    // unit_test[45]  IPv6アドレスでRS追加(既に追加されているendpointを追加しようとして失敗ケース)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------45" );
    //RSパラメータ設定
    l7vs::virtualservice_element    elem_add_rs2;
    {
        elem_add_rs2.udpmode                    = false;
        elem_add_rs2.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
        elem_add_rs2.udp_recv_endpoint            = 
                udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
        elem_add_rs2.realserver_vector.clear();
        elem_add_rs2.protocol_module_name        = "PMtest1";
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
        rs_elem.weight    = 10;
        elem_add_rs2.realserver_vector.push_back( rs_elem );
    }

    vs_err.setter(false,"");
    vs->add_realserver( elem_add_rs2, vs_err );
    BOOST_CHECK( vs_err == true );

    // unit_test[46]  IPv6アドレスでRS追加(VSが不一致で失敗ケース)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------46" );
    l7vs::virtualservice_element    elem_add_rs3;
    {
        elem_add_rs3.udpmode                    = false;
        elem_add_rs3.tcp_accept_endpoint        = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678:90AB:CDEF:0000:0000:FEDC:0009" ), (60000) );
        elem_add_rs3.udp_recv_endpoint            = 
                udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
        elem_add_rs3.realserver_vector.clear();
        elem_add_rs3.protocol_module_name        = "PMtest1";
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (6006) );
        rs_elem.weight    = 10;
        elem_add_rs3.realserver_vector.push_back( rs_elem );
    }

    vs_err.setter(false,"");
    vs->add_realserver( elem_add_rs2, vs_err );
    BOOST_CHECK( vs_err == true );

    // unit_test[47]  IPv6アドレスでRS編集
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------47" );
    l7vs::virtualservice_element    elem_edit_rs1;
    {
        elem_edit_rs1.udpmode                    = false;
        elem_edit_rs1.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
        elem_edit_rs1.udp_recv_endpoint            = 
                udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
        elem_edit_rs1.realserver_vector.clear();
        elem_edit_rs1.protocol_module_name        = "PMtest1";
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
        rs_elem.weight    = 10;
        elem_edit_rs1.realserver_vector.push_back( rs_elem );
    }

    vs_err.setter(false,"");
    vs->edit_realserver( elem_edit_rs1, vs_err );
    BOOST_CHECK( vs_err == false );
    BOOST_CHECK( vs->get_rs_list().begin()->weight == 10 );

    // unit_test[48]  IPv6アドレスでRS編集(リストに一致するRSが存在しないので失敗する)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------48" );
    l7vs::virtualservice_element    elem_edit_rs2;
    {
        elem_edit_rs2.udpmode                    = false;
        elem_edit_rs2.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
        elem_edit_rs2.udp_recv_endpoint            = 
                udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
        elem_edit_rs2.realserver_vector.clear();
        elem_edit_rs2.protocol_module_name        = "PMtest1";
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:3891" ), (8080) );
        rs_elem.weight    = 10;
        elem_edit_rs2.realserver_vector.push_back( rs_elem );
    }
    vs_err.setter(false,"");
    vs->edit_realserver( elem_edit_rs2, vs_err );
    BOOST_CHECK( vs_err == true );

    // unit_test[49]  IPv6アドレスでRS編集(VSが不一致で失敗する)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------49" );
    l7vs::virtualservice_element    elem_edit_rs3;
    {
        elem_edit_rs3.udpmode                    = false;
        elem_edit_rs3.tcp_accept_endpoint        = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678:90AB:CDEF:0000:0000:FEDC:0009" ), (60000) );
        elem_edit_rs3.udp_recv_endpoint            = 
                udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
        elem_edit_rs3.realserver_vector.clear();
        elem_edit_rs3.protocol_module_name        = "PMtest1";
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
        rs_elem.weight    = 10;
        elem_edit_rs3.realserver_vector.push_back( rs_elem );
    }

    vs_err.setter(false,"");
    vs->edit_realserver( elem_edit_rs3, vs_err );
    BOOST_CHECK( vs_err == true );

    // unit_test[50]  IPv6アドレスでRS削除(VSが一致しないので失敗する)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------50" );
    l7vs::virtualservice_element    elem_del_rs1;
    {
        elem_del_rs1.udpmode                    = false;
        elem_del_rs1.tcp_accept_endpoint        = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678:90AB:CDEF:0000:0000:FEDC:0009" ), (60000) );
        elem_del_rs1.udp_recv_endpoint            = 
                udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
        elem_del_rs1.realserver_vector.clear();
        elem_del_rs1.protocol_module_name        = "PMtest1";
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
        rs_elem.weight    = 10;
        elem_del_rs1.realserver_vector.push_back( rs_elem );
    }
    vs_err.setter(false,"");
    vs->del_realserver( elem_del_rs1, vs_err );
    BOOST_CHECK( vs_err == true );

    // unit_test[51]  IPv6アドレスでRS削除(リストに一致するRSが存在しないので失敗する)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------51" );
    l7vs::virtualservice_element    elem_del_rs2;
    {
        elem_del_rs2.udpmode                    = false;
        elem_del_rs2.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
        elem_del_rs2.udp_recv_endpoint            = 
                udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
        elem_del_rs2.realserver_vector.clear();
        elem_del_rs2.protocol_module_name        = "PMtest1";
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:1111:CDEF" ), (8080) );
        rs_elem.weight    = 10;
        elem_del_rs2.realserver_vector.push_back( rs_elem );
    }

    vs_err.setter(false,"");
    vs->del_realserver( elem_del_rs2, vs_err );
    BOOST_CHECK( vs_err == true );

    // unit_test[52]  IPv6アドレスでRS削除
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------52" );
    l7vs::virtualservice_element    elem_del_rs3;
    {
        elem_del_rs3.udpmode                    = false;
        elem_del_rs3.tcp_accept_endpoint        = 
            tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
        elem_del_rs3.udp_recv_endpoint            = 
                udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
        elem_del_rs3.realserver_vector.clear();
        elem_del_rs3.protocol_module_name        = "PMtest1";
        l7vs::realserver_element    rs_elem;
        rs_elem.tcp_endpoint = 
                tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
        rs_elem.weight    = 10;
        elem_del_rs3.realserver_vector.push_back( rs_elem );
    }

    vs_err.setter(false,"");
    vs->del_realserver( elem_del_rs3, vs_err );
    BOOST_CHECK( vs_err == false );
    BOOST_CHECK( vs->get_rs_list().size() == 9 );

    //テストが終わったらStubのレプリケーションエリアを削除
    debugg_flug_struct::getInstance().create_rep_area();

    vs_err.setter(false,"");
    vs->finalize( vs_err );
    delete vs;
}

//実際にAcceptさせるテスト
void    virtualservice_tcp_test5(){
    // unit_test[53]  ClientからConnectさせてAcceptすることを確認する
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------53" );
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
    elem1.qos_upstream                = 65535ULL;
    elem1.qos_downstream            = 32767ULL;
    elem1.ssl_file_name            = "";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );
    boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );
    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();
std::cout << "!" << std::endl;

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();
std::cout << "!" << std::endl;

    vs_err.setter(false,"");
   // vs->finalize( vs_err );
std::cout << "!" << std::endl;
    delete vs;
}

//test case6  set_socket_option test
void    virtualservice_tcp_set_socket_option(){
    counter = 1;

    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().param_exist_flag() = false;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    //set element value
    element.udpmode                    = false;
    element.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    element.udp_recv_endpoint        = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    element.realserver_vector.clear();
    element.protocol_module_name    = "PMtest1";
    element.schedule_module_name    = "SMtest1";
    element.protocol_args.clear();
    element.protocol_args.push_back( "testarg" );
    element.protocol_args.push_back( "testarg2" );
    element.sorry_maxconnection        = 1234LL;
    element.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    element.sorry_flag                = false;
    element.qos_upstream            = 65535ULL;
    element.qos_downstream            = 32767ULL;
    
//    std::cout << counter++ << std::endl;
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );

    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( element == vs->get_element() );
    
    
    std::cout << "parse_socket_option normal case 01 (all socket option off)" << std::endl;
    // parse_socket_option normal case 01 (all socket option off)
    {

        vs->get_element().socket_option_tcp_defer_accept   = 0;
        vs->get_element().socket_option_tcp_nodelay        = 0;
        vs->get_element().socket_option_tcp_cork           = 0;
        vs->get_element().socket_option_tcp_quickack       = 0;

        vs->set_socket_option();


        //! TCP_DEFER_ACCEPT option
        // unit_test[56] parse_socket_option normal case 01 (set option TCP_DEFER_ACCEPT) check
        BOOST_CHECK_EQUAL( vs->get_defer_accept_opt(), false );
        //! TCP_DEFER_ACCEPT option value
        // unit_test[57] parse_socket_option normal case 01 (set option TCP_DEFER_ACCEPT value) check
        BOOST_CHECK_EQUAL( vs->get_defer_accept_val(), 0 );
        //! TCP_NODELAY   (false:not set,true:set option)
        // unit_test[58] parse_socket_option normal case 01 (set option TCP_NODELAY) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().nodelay_opt, false );
        //! TCP_NODELAY option value  (false:off,true:on)
        // unit_test[59] parse_socket_option normal case 01 (set option TCP_NODELAY value) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().nodelay_val, false );
        //! TCP_CORK      (false:not set,true:set option)
        // unit_test[60] parse_socket_option normal case 01 (set option TCP_CORK) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().cork_opt, false );
        //! TCP_CORK option value     (false:off,true:on)
        // unit_test[61] parse_socket_option normal case 01 (set option TCP_CORK value) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().cork_val, false );
        //! TCP_QUICKACK  (false:not set,true:set option)
        // unit_test[62] parse_socket_option normal case 01 (set option TCP_QUICKACK) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().quickack_opt, false );
        //! TCP_QUICKACK option value (false:off,true:on)
        // unit_test[63] parse_socket_option normal case 01 (set option TCP_QUICKACK value) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().quickack_val, false );

    }

    std::cout << "parse_socket_option normal case 02 (all socket option on)" << std::endl;
    // parse_socket_option normal case 02 (all socket option on)
    {

        vs->get_element().socket_option_tcp_defer_accept   = 1;
        vs->get_element().socket_option_tcp_nodelay        = 1;
        vs->get_element().socket_option_tcp_cork           = 1;
        vs->get_element().socket_option_tcp_quickack       = 1;

        vs->set_socket_option();


        //! TCP_DEFER_ACCEPT option
        // unit_test[64] parse_socket_option normal case 01 (set option TCP_DEFER_ACCEPT) check
        BOOST_CHECK_EQUAL( vs->get_defer_accept_opt(), true );
        //! TCP_DEFER_ACCEPT option value
        // unit_test[65] parse_socket_option normal case 01 (set option TCP_DEFER_ACCEPT value) check
        BOOST_CHECK_EQUAL( vs->get_defer_accept_val(), 1 );
        //! TCP_NODELAY   (false:not set,true:set option)
        // unit_test[66] parse_socket_option normal case 01 (set option TCP_NODELAY) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().nodelay_opt, true );
        //! TCP_NODELAY option value  (false:off,true:on)
        // unit_test[67] parse_socket_option normal case 01 (set option TCP_NODELAY value) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().nodelay_val, true );
        //! TCP_CORK      (false:not set,true:set option)
        // unit_test[68] parse_socket_option normal case 01 (set option TCP_CORK) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().cork_opt, true );
        //! TCP_CORK option value     (false:off,true:on)
        // unit_test[69] parse_socket_option normal case 01 (set option TCP_CORK value) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().cork_val, true );
        //! TCP_QUICKACK  (false:not set,true:set option)
        // unit_test[70] parse_socket_option normal case 01 (set option TCP_QUICKACK) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().quickack_opt, true );
        //! TCP_QUICKACK option value (false:off,true:on)
        // unit_test[71] parse_socket_option normal case 01 (set option TCP_QUICKACK value) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().quickack_val, true );

    }


    std::cout << "parse_socket_option normal case 03 (all socket option on)" << std::endl;
    // parse_socket_option normal case 03 (all socket option on)
    {

        element.socket_option_tcp_defer_accept   = 1;
        element.socket_option_tcp_nodelay        = 1;
        element.socket_option_tcp_cork           = 1;
        element.socket_option_tcp_quickack       = 2;

        vs->set_socket_option();


        //! TCP_DEFER_ACCEPT option
        // unit_test[72] parse_socket_option normal case 01 (set option TCP_DEFER_ACCEPT) check
        BOOST_CHECK_EQUAL( vs->get_defer_accept_opt(), true );
        //! TCP_DEFER_ACCEPT option value
        // unit_test[73] parse_socket_option normal case 01 (set option TCP_DEFER_ACCEPT value) check
        BOOST_CHECK_EQUAL( vs->get_defer_accept_val(), 1 );
        //! TCP_NODELAY   (false:not set,true:set option)
        // unit_test[74] parse_socket_option normal case 01 (set option TCP_NODELAY) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().nodelay_opt, true );
        //! TCP_NODELAY option value  (false:off,true:on)
        // unit_test[75] parse_socket_option normal case 01 (set option TCP_NODELAY value) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().nodelay_val, true );
        //! TCP_CORK      (false:not set,true:set option)
        // unit_test[76] parse_socket_option normal case 01 (set option TCP_CORK) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().cork_opt, true );
        //! TCP_CORK option value     (false:off,true:on)
        // unit_test[77] parse_socket_option normal case 01 (set option TCP_CORK value) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().cork_val, true );
        //! TCP_QUICKACK  (false:not set,true:set option)
        // unit_test[78] parse_socket_option normal case 01 (set option TCP_QUICKACK) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().quickack_opt, true );
        //! TCP_QUICKACK option value (false:off,true:on)
        // unit_test[79] parse_socket_option normal case 01 (set option TCP_QUICKACK value) check
        BOOST_CHECK_EQUAL( vs->get_set_sock_opt().quickack_val, true );

    }


    BOOST_MESSAGE( "-------9" );
    delete vs;

    
}


// get_ssl_password test
void    virtualservice_tcp_get_ssl_password(){
    counter = 1;

    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().param_exist_flag() = false;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication              rep;
    l7vs::virtualservice_element   element;
    //set element value
    element.udpmode                = false;
    element.tcp_accept_endpoint    = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    element.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    element.realserver_vector.clear();
    element.protocol_module_name   = "PMtest1";
    element.schedule_module_name   = "SMtest1";
    element.protocol_args.clear();
    element.protocol_args.push_back( "testarg" );
    element.protocol_args.push_back( "testarg2" );
    element.sorry_maxconnection    = 1234LL;
    element.sorry_endpoint         = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    element.sorry_flag             = false;
    element.qos_upstream           = 65535ULL;
    element.qos_downstream         = 32767ULL;

//    std::cout << counter++ << std::endl;
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );

    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( element == vs->get_element() );

    std::cout << "get_ssl_password normal case 01" << std::endl;
    // get_ssl_password normal case 01
    {
        std::string       ssl_passwd           = "ssl_test";
        std::string       ssl_passwd_dir       = "./sslfiles/";
        std::string       ssl_passwd_file      = "ssl_password_file1";
        std::string       ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream     ofs( ssl_passwd_full_path.c_str() );
        ofs << ssl_passwd << "\n";
        ofs.close();

        vs->set_private_key_passwd_dir( ssl_passwd_dir );
        vs->set_private_key_passwd_file( ssl_passwd_file );

        std::string return_ssl_password = vs->get_ssl_password();

        //! TCP_DEFER_ACCEPT option
        // unit_test[80] ssl password check
        BOOST_CHECK_EQUAL( return_ssl_password, ssl_passwd );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "get_ssl_password error case 01 dir empty" << std::endl;
    // get_ssl_password normal error case 01 dir empty
    {
        std::string       ssl_passwd           = "";
        std::string       ssl_passwd_dir       = "";
        std::string       ssl_passwd_file      = "ssl_password_file1";

        vs->set_private_key_passwd_dir( ssl_passwd_dir );
        vs->set_private_key_passwd_file( ssl_passwd_file );

        std::string return_ssl_password = vs->get_ssl_password();

        //! TCP_DEFER_ACCEPT option
        // unit_test[81] ssl password check
        BOOST_CHECK_EQUAL( return_ssl_password, ssl_passwd );

    }

    std::cout <<"get_ssl_password error case 02 password size over"<<std::endl;
    // get_ssl_password normal error case 02 password size over
    {
        std::string       ssl_passwd           = "";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";
        ssl_passwd += "1234567890";

        std::string       ssl_passwd_dir       = "./sslfiles/";
        std::string       ssl_passwd_file      = "ssl_password_file1";

        std::string       ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;
        std::ofstream     ofs( ssl_passwd_full_path.c_str() );
        ofs << ssl_passwd << "\n";
        ofs.close();

        vs->set_private_key_passwd_dir( ssl_passwd_dir );
        vs->set_private_key_passwd_file( ssl_passwd_file );

        std::string return_ssl_password = vs->get_ssl_password();

        //! TCP_DEFER_ACCEPT option
        // unit_test[82] ssl password check
        BOOST_CHECK_EQUAL( return_ssl_password, "" );

    }

    std::cout << "get_ssl_password error case 03 file empty" << std::endl;
    // get_ssl_password normal error case 03 file empty
    {
        std::string       ssl_passwd           = "";
        std::string       ssl_passwd_dir       = "./sslfiles/";
        std::string       ssl_passwd_file      = "ssl_password_file1";
        std::string       ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream     ofs( ssl_passwd_full_path.c_str() );
        ofs.close();

        vs->set_private_key_passwd_dir( ssl_passwd_dir );
        vs->set_private_key_passwd_file( ssl_passwd_file );

        std::string return_ssl_password = vs->get_ssl_password();

        //! TCP_DEFER_ACCEPT option
        // unit_test[83] ssl password check
        BOOST_CHECK_EQUAL( return_ssl_password, ssl_passwd );

        unlink( ssl_passwd_full_path.c_str() );


    }


    BOOST_MESSAGE( "-------??" );
    delete vs;

}

// conv_verify_option test
void    virtualservice_tcp_conv_verify_option(){
    counter = 1;

    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().param_exist_flag() = false;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    //set element value
    element.udpmode                    = false;
    element.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    element.udp_recv_endpoint        = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    element.realserver_vector.clear();
    element.protocol_module_name    = "PMtest1";
    element.schedule_module_name    = "SMtest1";
    element.protocol_args.clear();
    element.protocol_args.push_back( "testarg" );
    element.protocol_args.push_back( "testarg2" );
    element.sorry_maxconnection        = 1234LL;
    element.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    element.sorry_flag                = false;
    element.qos_upstream            = 65535ULL;
    element.qos_downstream            = 32767ULL;

//    std::cout << counter++ << std::endl;
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );

    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( element == vs->get_element() );

    std::cout << "conv_verify_option SSL_VERIFY_NONE case" << std::endl;
    // conv_verify_option SSL_VERIFY_NONE case
    {
        std::string input_verify_option = "SSL_VERIFY_NONE";
    
        int return_verify_option = vs->conv_verify_option(input_verify_option);
    
        //! SSL_VERIFY_NONE option
        // unit_test[84] verify_option check
        BOOST_CHECK_EQUAL( return_verify_option, SSL_VERIFY_NONE );
    
    }

    std::cout << "conv_verify_option SSL_VERIFY_PEER case" << std::endl;
    // conv_verify_option SSL_VERIFY_PEER case
    {
        std::string input_verify_option = "SSL_VERIFY_PEER";
    
        int return_verify_option = vs->conv_verify_option(input_verify_option);
    
        //! SSL_VERIFY_PEER option
        // unit_test[85] verify_option check
        BOOST_CHECK_EQUAL( return_verify_option, SSL_VERIFY_PEER );
    
    }

    std::cout << "conv_verify_option SSL_VERIFY_FAIL_IF_NO_PEER_CERT case" << std::endl;
    // conv_verify_option SSL_VERIFY_FAIL_IF_NO_PEER_CERT case
    {
        std::string input_verify_option = "SSL_VERIFY_FAIL_IF_NO_PEER_CERT";
    
        int return_verify_option = vs->conv_verify_option(input_verify_option);
    
        //! SSL_VERIFY_FAIL_IF_NO_PEER_CERT option
        // unit_test[86] verify_option check
        BOOST_CHECK_EQUAL( return_verify_option, SSL_VERIFY_FAIL_IF_NO_PEER_CERT );
    
    }

    std::cout << "conv_verify_option SSL_VERIFY_CLIENT_ONCE case" << std::endl;
    // conv_verify_option SSL_VERIFY_CLIENT_ONCE case
    {
        std::string input_verify_option = "SSL_VERIFY_CLIENT_ONCE";
    
        int return_verify_option = vs->conv_verify_option(input_verify_option);
    
        //! SSL_VERIFY_CLIENT_ONCE option
        // unit_test[87] verify_option check
        BOOST_CHECK_EQUAL( return_verify_option, SSL_VERIFY_CLIENT_ONCE );
    
    }


    std::cout << "conv_verify_option error case" << std::endl;
    // conv_verify_option error case
    {
        std::string input_verify_option = "error";
    
        int return_verify_option = vs->conv_verify_option(input_verify_option);
    
        //! error option
        // unit_test[88] verify_option error check
        BOOST_CHECK_EQUAL( return_verify_option, -1 );
    
    }

    BOOST_MESSAGE( "-------??" );
    delete vs;

}


// conv_ssl_option test
void    virtualservice_tcp_conv_ssl_option(){
    counter = 1;

    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().param_exist_flag() = false;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    //set element value
    element.udpmode                    = false;
    element.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    element.udp_recv_endpoint        = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    element.realserver_vector.clear();
    element.protocol_module_name    = "PMtest1";
    element.schedule_module_name    = "SMtest1";
    element.protocol_args.clear();
    element.protocol_args.push_back( "testarg" );
    element.protocol_args.push_back( "testarg2" );
    element.sorry_maxconnection        = 1234LL;
    element.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    element.sorry_flag                = false;
    element.qos_upstream            = 65535ULL;
    element.qos_downstream            = 32767ULL;

//    std::cout << counter++ << std::endl;
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );

    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( element == vs->get_element() );

    std::cout << "conv_ssl_option SSL_OP_MICROSOFT_SESS_ID_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_MICROSOFT_SESS_ID_BUG case
    {
        std::string input_ssl_option = "SSL_OP_MICROSOFT_SESS_ID_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_MICROSOFT_SESS_ID_BUG option
        // unit_test[89] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_MICROSOFT_SESS_ID_BUG );
    
    }

    std::cout << "conv_ssl_option SSL_OP_NETSCAPE_CHALLENGE_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_NETSCAPE_CHALLENGE_BUG case
    {
        std::string input_ssl_option = "SSL_OP_NETSCAPE_CHALLENGE_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_NETSCAPE_CHALLENGE_BUG option
        // unit_test[90] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_NETSCAPE_CHALLENGE_BUG );
    
    }

    std::cout << "conv_ssl_option SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG case
    {
        std::string input_ssl_option = "SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG option
        // unit_test[91] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG );
    
    }

    std::cout << "conv_ssl_option SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG case
    {
        std::string input_ssl_option = "SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG option
        // unit_test[92] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG );
    
    }

    std::cout << "conv_ssl_option SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER case" << std::endl;
    // conv_ssl_option SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER case
    {
        std::string input_ssl_option = "SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER option
        // unit_test[93] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER );
    
    }

    std::cout << "conv_ssl_option SSL_OP_MSIE_SSLV2_RSA_PADDING case" << std::endl;
    // conv_ssl_option SSL_OP_MSIE_SSLV2_RSA_PADDING case
    {
        std::string input_ssl_option = "SSL_OP_MSIE_SSLV2_RSA_PADDING";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_MSIE_SSLV2_RSA_PADDING option
        // unit_test[94] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_MSIE_SSLV2_RSA_PADDING );
    
    }

    std::cout << "conv_ssl_option SSL_OP_SSLEAY_080_CLIENT_DH_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_SSLEAY_080_CLIENT_DH_BUG case
    {
        std::string input_ssl_option = "SSL_OP_SSLEAY_080_CLIENT_DH_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_SSLEAY_080_CLIENT_DH_BUG option
        // unit_test[95] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_SSLEAY_080_CLIENT_DH_BUG );
    
    }

    std::cout << "conv_ssl_option SSL_OP_TLS_D5_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_TLS_D5_BUG case
    {
        std::string input_ssl_option = "SSL_OP_TLS_D5_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_TLS_D5_BUG option
        // unit_test[96] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_TLS_D5_BUG );
    
    }

    std::cout << "conv_ssl_option SSL_OP_TLS_BLOCK_PADDING_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_TLS_BLOCK_PADDING_BUG case
    {
        std::string input_ssl_option = "SSL_OP_TLS_BLOCK_PADDING_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_TLS_BLOCK_PADDING_BUG option
        // unit_test[97] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_TLS_BLOCK_PADDING_BUG );
    
    }

    std::cout << "conv_ssl_option SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS case" << std::endl;
    // conv_ssl_option SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS case
    {
        std::string input_ssl_option = "SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS option
        // unit_test[98] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS );
    
    }

    std::cout << "conv_ssl_option SSL_OP_ALL case" << std::endl;
    // conv_ssl_option SSL_OP_ALL case
    {
        std::string input_ssl_option = "SSL_OP_ALL";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_ALL option
        // unit_test[99] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_ALL );
    
    }

    std::cout << "conv_ssl_option SSL_OP_NO_QUERY_MTU case" << std::endl;
    // conv_ssl_option SSL_OP_NO_QUERY_MTU case
    {
        std::string input_ssl_option = "SSL_OP_NO_QUERY_MTU";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_NO_QUERY_MTU option
        // unit_test[100] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_NO_QUERY_MTU );
    
    }

    std::cout << "conv_ssl_option SSL_OP_COOKIE_EXCHANGE case" << std::endl;
    // conv_ssl_option SSL_OP_COOKIE_EXCHANGE case
    {
        std::string input_ssl_option = "SSL_OP_COOKIE_EXCHANGE";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_COOKIE_EXCHANGE option
        // unit_test[101] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_COOKIE_EXCHANGE );
    
    }

    std::cout << "conv_ssl_option SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION case" << std::endl;
    // conv_ssl_option SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION case
    {
        std::string input_ssl_option = "SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION option
        // unit_test[102] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION );
    
    }

    std::cout << "conv_ssl_option SSL_OP_SINGLE_ECDH_USE case" << std::endl;
    // conv_ssl_option SSL_OP_SINGLE_ECDH_USE case
    {
        std::string input_ssl_option = "SSL_OP_SINGLE_ECDH_USE";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_SINGLE_ECDH_USE option
        // unit_test[103] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_SINGLE_ECDH_USE );
    
    }

    std::cout << "conv_ssl_option SSL_OP_SINGLE_DH_USE case" << std::endl;
    // conv_ssl_option SSL_OP_SINGLE_DH_USE case
    {
        std::string input_ssl_option = "SSL_OP_SINGLE_DH_USE";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_SINGLE_DH_USE option
        // unit_test[104] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_SINGLE_DH_USE );
    
    }

    std::cout << "conv_ssl_option SSL_OP_EPHEMERAL_RSA case" << std::endl;
    // conv_ssl_option SSL_OP_EPHEMERAL_RSA case
    {
        std::string input_ssl_option = "SSL_OP_EPHEMERAL_RSA";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_EPHEMERAL_RSA option
        // unit_test[105] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_EPHEMERAL_RSA );
    
    }

    std::cout << "conv_ssl_option SSL_OP_CIPHER_SERVER_PREFERENCE case" << std::endl;
    // conv_ssl_option SSL_OP_CIPHER_SERVER_PREFERENCE case
    {
        std::string input_ssl_option = "SSL_OP_CIPHER_SERVER_PREFERENCE";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_CIPHER_SERVER_PREFERENCE option
        // unit_test[106] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_CIPHER_SERVER_PREFERENCE );
    
    }

    std::cout << "conv_ssl_option SSL_OP_TLS_ROLLBACK_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_TLS_ROLLBACK_BUG case
    {
        std::string input_ssl_option = "SSL_OP_TLS_ROLLBACK_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_TLS_ROLLBACK_BUG option
        // unit_test[107] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_TLS_ROLLBACK_BUG );
    
    }

    std::cout << "conv_ssl_option SSL_OP_NO_SSLv2 case" << std::endl;
    // conv_ssl_option SSL_OP_NO_SSLv2 case
    {
        std::string input_ssl_option = "SSL_OP_NO_SSLv2";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_NO_SSLv2 option
        // unit_test[108] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_NO_SSLv2 );
    
    }

    std::cout << "conv_ssl_option SSL_OP_NO_SSLv3 case" << std::endl;
    // conv_ssl_option SSL_OP_NO_SSLv3 case
    {
        std::string input_ssl_option = "SSL_OP_NO_SSLv3";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_NO_SSLv3 option
        // unit_test[109] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_NO_SSLv3 );
    
    }


    std::cout << "conv_ssl_option SSL_OP_NO_TLSv1 case" << std::endl;
    // conv_ssl_option SSL_OP_NO_TLSv1 case
    {
        std::string input_ssl_option = "SSL_OP_NO_TLSv1";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_NO_TLSv1 option
        // unit_test[110] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_NO_TLSv1 );
    
    }

    std::cout << "conv_ssl_option SSL_OP_PKCS1_CHECK_1 case" << std::endl;
    // conv_ssl_option SSL_OP_PKCS1_CHECK_1 case
    {
        std::string input_ssl_option = "SSL_OP_PKCS1_CHECK_1";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_PKCS1_CHECK_1 option
        // unit_test[111] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_PKCS1_CHECK_1 );
    
    }

    std::cout << "conv_ssl_option SSL_OP_PKCS1_CHECK_2 case" << std::endl;
    // conv_ssl_option SSL_OP_PKCS1_CHECK_2 case
    {
        std::string input_ssl_option = "SSL_OP_PKCS1_CHECK_2";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_PKCS1_CHECK_2 option
        // unit_test[112] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_PKCS1_CHECK_2 );
    
    }

    std::cout << "conv_ssl_option SSL_OP_NETSCAPE_CA_DN_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_NETSCAPE_CA_DN_BUG case
    {
        std::string input_ssl_option = "SSL_OP_NETSCAPE_CA_DN_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_NETSCAPE_CA_DN_BUG option
        // unit_test[113] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_NETSCAPE_CA_DN_BUG );
    
    }

    std::cout << "conv_ssl_option SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG case" << std::endl;
    // conv_ssl_option SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG case
    {
        std::string input_ssl_option = "SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG option
        // unit_test[114] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG );
    
    }

    std::cout << "conv_ssl_option error case" << std::endl;
    // conv_ssl_option error case
    {
        std::string input_ssl_option = "error";
    
        long int return_ssl_option = vs->conv_ssl_option(input_ssl_option);
    
        //! error option
        // unit_test[115] conv_ssl_option check
        BOOST_CHECK_EQUAL( return_ssl_option, -1 );
    
    }

    BOOST_MESSAGE( "-------??" );
    delete vs;

}

// get_ssl_parameter test sub func
void virtualservice_tcp_get_ssl_parameter_clear( l7vs::vs_tcp* vs ){
    std::string empty_data = "";
    int err_data = -1;

    // read param clear
    vs->set_ca_dir(empty_data);
    vs->set_ca_file(empty_data);
    vs->set_cert_chain_dir(empty_data);
    vs->set_cert_chain_file(empty_data);
    vs->set_private_key_dir(empty_data);
    vs->set_private_key_file(empty_data);
    vs->set_private_key_filetype(DEFAULT_SSL_PRIVATE_KEY_FILETYPE);
    vs->set_private_key_passwd_dir(empty_data);
    vs->set_private_key_passwd_file(empty_data);
    vs->set_verify_options(0);
    vs->set_verify_cert_depth(err_data);
    vs->set_ssl_options(0);
    vs->set_is_tmp_dh_use(false);
    vs->set_tmp_dh_dir(empty_data);
    vs->set_tmp_dh_file(empty_data);
    vs->set_cipher_list(empty_data);
    vs->set_is_session_cache_use(false);
    vs->set_session_cache_mode(err_data);
    vs->set_session_cache_size(err_data);
    vs->set_session_cache_timeout(err_data);
    vs->set_handshake_timeout(err_data);
    vs->set_ssl_file_name(empty_data);

}

// get_ssl_parameter test
void virtualservice_tcp_get_ssl_parameter(){
    counter = 1;

    debugg_flug_struct::getInstance().pmcontrol_err_flag() = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag() = false;
    debugg_flug_struct::getInstance().param_exist_flag()   = false;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication              rep;
    l7vs::virtualservice_element   element;
    //set element value
    element.udpmode                = false;
    element.tcp_accept_endpoint    = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    element.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    element.realserver_vector.clear();
    element.protocol_module_name   = "PMtest1";
    element.schedule_module_name   = "SMtest1";
    element.protocol_args.clear();
    element.protocol_args.push_back( "testarg" );
    element.protocol_args.push_back( "testarg2" );
    element.sorry_maxconnection    = 1234LL;
    element.sorry_endpoint         = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    element.sorry_flag             = false;
    element.qos_upstream           = 65535ULL;
    element.qos_downstream         = 32767ULL;

    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );

    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( element == vs->get_element() );

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter normal case01" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter normal case01
    {
        // create config file
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE true
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;
        int         input_handshake_timeout = 30;

        std::string       ssl_passwd           = "test";
        std::string       ssl_passwd_dir       = "./sslfiles/";
        std::string       ssl_passwd_file      = "ssl_password_file01.cf";
        std::string       ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream     ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_dir=\""                  << input_ca_dir << "\"\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_dir=\""          << input_cert_chain_dir << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_dir=\""         << input_private_key_dir << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_filetype=\""    << input_private_key_filetype << "\"\n";
        ofs << "private_key_passwd_dir=\""  << input_private_key_passwd_dir << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "verify_options=\""          << "SSL_VERIFY_PEER" << "\"\n";
        ofs << "verify_options=\""          << "SSL_VERIFY_FAIL_IF_NO_PEER_CERT" << "\"\n";
        ofs << "verify_cert_depth="         << input_verify_cert_depth << "\n";
        ofs << "ssl_options=\""             << "SSL_OP_ALL" << "\"\n";
        ofs << "ssl_options=\""             << "SSL_OP_NO_SSLv2" << "\"\n";
        ofs << "ssl_options=\""             << "SSL_OP_SINGLE_DH_USE" << "\"\n";
        ofs << "tmp_dh_dir=\""              << input_tmp_dh_dir << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "cipher_list=\""             << input_cipher_list << "\"\n";
        ofs << "session_cache=\""           << "on" << "\"\n";
        ofs << "session_cache_size="        << input_session_cache_size << "\n";
        ofs << "session_cache_timeout="     << input_session_cache_timeout << "\n";
        ofs << "timeout_sec="               << input_handshake_timeout << "\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[116] get_ssl_parameter normal case01
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, true );
        BOOST_CHECK_EQUAL( vs->get_ca_dir() , input_ca_dir );
        BOOST_CHECK_EQUAL( vs->get_ca_file() , input_ca_file );
        BOOST_CHECK_EQUAL( vs->get_cert_chain_dir() , input_cert_chain_dir );
        BOOST_CHECK_EQUAL( vs->get_cert_chain_file() , input_cert_chain_file );
        BOOST_CHECK_EQUAL( vs->get_private_key_dir() , input_private_key_dir );
        BOOST_CHECK_EQUAL( vs->get_private_key_file() , input_private_key_file );
        BOOST_CHECK( vs->get_private_key_filetype() == boost::asio::ssl::context::pem );
        BOOST_CHECK_EQUAL( vs->get_private_key_passwd_dir() , input_private_key_passwd_dir );
        BOOST_CHECK_EQUAL( vs->get_private_key_passwd_file() , input_private_key_passwd_file );
        BOOST_CHECK_EQUAL( vs->get_verify_options() , input_verify_options );
        BOOST_CHECK_EQUAL( vs->get_verify_cert_depth() , input_verify_cert_depth );
        BOOST_CHECK_EQUAL( vs->get_ssl_options() , input_ssl_options );
        BOOST_CHECK_EQUAL( vs->get_is_tmp_dh_use() , input_is_tmp_dh_use );
        BOOST_CHECK_EQUAL( vs->get_tmp_dh_dir() , input_tmp_dh_dir );
        BOOST_CHECK_EQUAL( vs->get_tmp_dh_file() , input_tmp_dh_file );
        BOOST_CHECK_EQUAL( vs->get_cipher_list() , input_cipher_list );
        BOOST_CHECK_EQUAL( vs->get_is_session_cache_use() , input_is_session_cache_use );
        BOOST_CHECK_EQUAL( vs->get_session_cache_mode() , input_session_cache_mode );
        BOOST_CHECK_EQUAL( vs->get_session_cache_size() , input_session_cache_size );
        BOOST_CHECK_EQUAL( vs->get_session_cache_timeout() , input_session_cache_timeout );
        BOOST_CHECK_EQUAL( vs->get_handshake_timeout() , input_handshake_timeout );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter normal case02 default data(err default root)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter normal case02 default data(err default root)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file02.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[117] get_ssl_parameter normal case02 default data(err default root)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, true );
        BOOST_CHECK_EQUAL( vs->get_ca_dir() , DEFAULT_SSL_CA_DIR );
        BOOST_CHECK_EQUAL( vs->get_ca_file() , input_ca_file );
        BOOST_CHECK_EQUAL( vs->get_cert_chain_dir() , DEFAULT_SSL_CERT_CHAIN_DIR );
        BOOST_CHECK_EQUAL( vs->get_cert_chain_file() , input_cert_chain_file );
        BOOST_CHECK_EQUAL( vs->get_private_key_dir() , DEFAULT_SSL_PRIVATE_KEY_DIR );
        BOOST_CHECK_EQUAL( vs->get_private_key_file() , input_private_key_file );
        BOOST_CHECK( vs->get_private_key_filetype() == DEFAULT_SSL_PRIVATE_KEY_FILETYPE );
        BOOST_CHECK_EQUAL( vs->get_private_key_passwd_dir() , DEFAULT_SSL_PRIVATE_KEY_PASSWD_DIR );
        BOOST_CHECK_EQUAL( vs->get_private_key_passwd_file() , input_private_key_passwd_file );
        BOOST_CHECK_EQUAL( vs->get_verify_options() , DEFAULT_SSL_VERIFY_OPTIONS );
        BOOST_CHECK_EQUAL( vs->get_verify_cert_depth() , DEFAULT_SSL_VERIFY_CERT_DEPTH );
        BOOST_CHECK_EQUAL( vs->get_ssl_options() , DEFAULT_SSL_OPTIONS );
        BOOST_CHECK_EQUAL( vs->get_is_tmp_dh_use() , input_is_tmp_dh_use );
        BOOST_CHECK_EQUAL( vs->get_tmp_dh_dir() , DEFAULT_SSL_TMP_DH_DIR );
        BOOST_CHECK_EQUAL( vs->get_tmp_dh_file() , input_tmp_dh_file );
        BOOST_CHECK_EQUAL( vs->get_cipher_list() , DEFAULT_SSL_CIPHER_LIST );
        BOOST_CHECK_EQUAL( vs->get_is_session_cache_use() , input_is_session_cache_use );
        BOOST_CHECK_EQUAL( vs->get_session_cache_mode() , DEFAULT_SSL_SESSION_CACHE_MODE );
        BOOST_CHECK_EQUAL( vs->get_session_cache_size() , DEFAULT_SSL_SESSION_CACHE_SIZE );
        BOOST_CHECK_EQUAL( vs->get_session_cache_timeout() , DEFAULT_SSL_SESSION_CACHE_TIMEOUT );
        BOOST_CHECK_EQUAL( vs->get_handshake_timeout() , DEFAULT_SSL_HANDSHAKE_TIMEOUT );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter normal case03 default data(empty default root)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter normal case03 default data(empty default root)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
        int         input_verify_options = SSL_VERIFY_PEER;
        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;
        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file03.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_dir=\""                  << "\"\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_dir=\""          << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_dir=\""         << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_filetype=\""    << "\"\n";
        ofs << "private_key_passwd_dir=\""  << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "verify_options=\""          << "SSL_VERIFY_PEER" << "\"\n";
        ofs << "verify_cert_depth="         << input_verify_cert_depth << "\n";
        ofs << "tmp_dh_dir=\""              << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "cipher_list=\""             << "\"\n";
        ofs << "session_cache=\""           << "\"\n";
        ofs << "session_cache_size="        << input_session_cache_size << "\n";
        ofs << "session_cache_timeout="     << input_session_cache_timeout << "\n";
        ofs << "timeout_sec="               << input_handshake_timeout << "\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[118] get_ssl_parameter normal case03 default data(empty default root)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, true );
        BOOST_CHECK_EQUAL( vs->get_ca_dir() , DEFAULT_SSL_CA_DIR );
        BOOST_CHECK_EQUAL( vs->get_ca_file() , input_ca_file );
        BOOST_CHECK_EQUAL( vs->get_cert_chain_dir() , DEFAULT_SSL_CERT_CHAIN_DIR );
        BOOST_CHECK_EQUAL( vs->get_cert_chain_file() , input_cert_chain_file );
        BOOST_CHECK_EQUAL( vs->get_private_key_dir() , DEFAULT_SSL_PRIVATE_KEY_DIR );
        BOOST_CHECK_EQUAL( vs->get_private_key_file() , input_private_key_file );
        BOOST_CHECK( vs->get_private_key_filetype() == DEFAULT_SSL_PRIVATE_KEY_FILETYPE );
        BOOST_CHECK_EQUAL( vs->get_private_key_passwd_dir() , DEFAULT_SSL_PRIVATE_KEY_PASSWD_DIR );
        BOOST_CHECK_EQUAL( vs->get_private_key_passwd_file() , input_private_key_passwd_file );
        BOOST_CHECK_EQUAL( vs->get_verify_options() , input_verify_options );
        BOOST_CHECK_EQUAL( vs->get_verify_cert_depth() , input_verify_cert_depth );
        BOOST_CHECK_EQUAL( vs->get_ssl_options() , DEFAULT_SSL_OPTIONS );
        BOOST_CHECK_EQUAL( vs->get_is_tmp_dh_use() , input_is_tmp_dh_use );
        BOOST_CHECK_EQUAL( vs->get_tmp_dh_dir() , DEFAULT_SSL_TMP_DH_DIR );
        BOOST_CHECK_EQUAL( vs->get_tmp_dh_file() , input_tmp_dh_file );
        BOOST_CHECK_EQUAL( vs->get_cipher_list() , DEFAULT_SSL_CIPHER_LIST );
        BOOST_CHECK_EQUAL( vs->get_is_session_cache_use() , input_is_session_cache_use );
        BOOST_CHECK_EQUAL( vs->get_session_cache_mode() , DEFAULT_SSL_SESSION_CACHE_MODE );
        BOOST_CHECK_EQUAL( vs->get_session_cache_size() , input_session_cache_size );
        BOOST_CHECK_EQUAL( vs->get_session_cache_timeout() , input_session_cache_timeout );
        BOOST_CHECK_EQUAL( vs->get_handshake_timeout() , input_handshake_timeout );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter normal case04 (filetype_str,cache_str variation)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter normal case04 (filetype_str,cache_str variation)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

        // create config file
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        std::string input_private_key_filetype = "SSL_FILETYPE_ASN1";
        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE true
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = false;
        int         input_handshake_timeout = 30;

        std::string       ssl_passwd           = "test";
        std::string       ssl_passwd_dir       = "./sslfiles/";
        std::string       ssl_passwd_file      = "ssl_password_file04.cf";
        std::string       ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream     ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_dir=\""                  << input_ca_dir << "\"\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_dir=\""          << input_cert_chain_dir << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_dir=\""         << input_private_key_dir << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_filetype=\""    << input_private_key_filetype << "\"\n";
        ofs << "private_key_passwd_dir=\""  << input_private_key_passwd_dir << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "verify_options=\""          << "SSL_VERIFY_PEER" << "\"\n";
        ofs << "verify_options=\""          << "SSL_VERIFY_FAIL_IF_NO_PEER_CERT" << "\"\n";
        ofs << "verify_cert_depth="         << input_verify_cert_depth << "\n";
        ofs << "ssl_options=\""             << "SSL_OP_ALL" << "\"\n";
        ofs << "ssl_options=\""             << "SSL_OP_NO_SSLv2" << "\"\n";
        ofs << "ssl_options=\""             << "SSL_OP_SINGLE_DH_USE" << "\"\n";
        ofs << "tmp_dh_dir=\""              << input_tmp_dh_dir << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "cipher_list=\""             << input_cipher_list << "\"\n";
        ofs << "session_cache=\""           << "off" << "\"\n";
        ofs << "timeout_sec="               << input_handshake_timeout << "\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[119] get_ssl_parameter normal case04 (filetype_str,cache_str variation)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, true );
        BOOST_CHECK_EQUAL( vs->get_ca_dir() , input_ca_dir );
        BOOST_CHECK_EQUAL( vs->get_ca_file() , input_ca_file );
        BOOST_CHECK_EQUAL( vs->get_cert_chain_dir() , input_cert_chain_dir );
        BOOST_CHECK_EQUAL( vs->get_cert_chain_file() , input_cert_chain_file );
        BOOST_CHECK_EQUAL( vs->get_private_key_dir() , input_private_key_dir );
        BOOST_CHECK_EQUAL( vs->get_private_key_file() , input_private_key_file );
        BOOST_CHECK( vs->get_private_key_filetype() == boost::asio::ssl::context::asn1 );
        BOOST_CHECK_EQUAL( vs->get_private_key_passwd_dir() , input_private_key_passwd_dir );
        BOOST_CHECK_EQUAL( vs->get_private_key_passwd_file() , input_private_key_passwd_file );
        BOOST_CHECK_EQUAL( vs->get_verify_options() , input_verify_options );
        BOOST_CHECK_EQUAL( vs->get_verify_cert_depth() , input_verify_cert_depth );
        BOOST_CHECK_EQUAL( vs->get_ssl_options() , input_ssl_options );
        BOOST_CHECK_EQUAL( vs->get_is_tmp_dh_use() , input_is_tmp_dh_use );
        BOOST_CHECK_EQUAL( vs->get_tmp_dh_dir() , input_tmp_dh_dir );
        BOOST_CHECK_EQUAL( vs->get_tmp_dh_file() , input_tmp_dh_file );
        BOOST_CHECK_EQUAL( vs->get_cipher_list() , input_cipher_list );
        BOOST_CHECK_EQUAL( vs->get_is_session_cache_use() , input_is_session_cache_use );
        BOOST_CHECK_EQUAL( vs->get_handshake_timeout() , input_handshake_timeout );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (no file)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (no file)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

        std::string       ssl_passwd           = "test";
        std::string       ssl_passwd_dir       = "./sslfiles/";
        std::string       ssl_passwd_file      = "ssl_password_file05.cf";
        std::string       ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[120] get_ssl_parameter err (no file)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );
    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (ca_file err)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (ca_file err)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
//!        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file06.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
//!        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[121] get_ssl_parameter check(ca_file err)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (cert_chain_file err)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (cert_chain_file err)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
//!        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file07.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
//!        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[122] get_ssl_parameter check(cert_chain_file err)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (cert_chain_file empty)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (cert_chain_file empty)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file08.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[123] get_ssl_parameter check(cert_chain_file empty)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (private_key_file err)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (private_key_file err)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
//!        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file09.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
//!        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[124] get_ssl_parameter check(private_key_file err)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (private_key_file empty)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (private_key_file empty)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file10.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[125] get_ssl_parameter check(private_key_file empty)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (other filetype_str)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (other filetype_str)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        std::string input_private_key_filetype = "SSL_FILETYPE_HOGE";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file11.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_filetype=\""        << input_private_key_filetype << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[126] get_ssl_parameter check(other filetype_str)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (private_key_passwd_file err)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (private_key_passwd_file err)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
//!        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file12.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
//!        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[127] get_ssl_parameter check(private_key_passwd_file err)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (private_key_passwd_file empty)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (private_key_passwd_file empty)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file13.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[128] get_ssl_parameter check(private_key_passwd_file empty)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (other verify_option)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (other verify_option)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file14.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "verify_options=\""          << "SSL_VERIFY_HOGE" << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[129] get_ssl_parameter check(other verify_option)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (verify_cert_depth MIN)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (verify_cert_depth MIN)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
        int         input_verify_cert_depth = -1;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file15.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "verify_cert_depth="         << input_verify_cert_depth << "\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[130] get_ssl_parameter check(verify_cert_depth MIN)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (verify_cert_depth MAX)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (verify_cert_depth MAX)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
        int         input_verify_cert_depth = INT_MAX;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file16.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "verify_cert_depth="         << input_verify_cert_depth << "0000000\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // MAX over case confirmation on a desk
        // MAX case
        // unit_test[131] get_ssl_parameter check(verify_cert_depth MAX)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, true );
        BOOST_CHECK_EQUAL( vs->get_verify_cert_depth() , DEFAULT_SSL_VERIFY_CERT_DEPTH );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (tmp_dh_file err)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (tmp_dh_file err)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
//!        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file17.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
//!        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[132] get_ssl_parameter check(tmp_dh_file err)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (tmp_dh_file empty)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (tmp_dh_file empty)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file18.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[133] get_ssl_parameter check(tmp_dh_file empty)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (other cache_str)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (other cache_str)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file19.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "session_cache=\""           << "hoge" << "\"\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[134] get_ssl_parameter check(other cache_str)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (session_cache_size MIN)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (session_cache_size MIN)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = -1;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file20.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "session_cache_size="        << input_session_cache_size << "\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[135] get_ssl_parameter check(session_cache_size MIN)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (session_cache_size MAX)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (session_cache_size MAX)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = INT_MAX;
//        long        input_session_cache_timeout = 300;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file21.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "session_cache_size="        << input_session_cache_size << "0000000\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // MAX over case confirmation on a desk
        // MAX case
        // unit_test[136] get_ssl_parameter check(session_cache_size MAX)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, true );
        BOOST_CHECK_EQUAL( vs->get_session_cache_size() , DEFAULT_SSL_SESSION_CACHE_SIZE );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (session_cache_timeout MIN)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (session_cache_timeout MIN)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = -1;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file22.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "session_cache_timeout="     << input_session_cache_timeout << "\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[137] get_ssl_parameter check(session_cache_timeout MIN)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (session_cache_timeout MAX)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (session_cache_timeout MAX)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = INT_MAX;
//        int         input_handshake_timeout = 30;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file23.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "session_cache_timeout="     << input_session_cache_timeout << "00000\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // MAX over case confirmation on a desk
        // MAX case
        // unit_test[138] get_ssl_parameter check(session_cache_timeout MAX)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, true );
        BOOST_CHECK_EQUAL( vs->get_session_cache_timeout() , DEFAULT_SSL_SESSION_CACHE_TIMEOUT );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (handshake_timeout MIN)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (handshake_timeout MIN)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
        int         input_handshake_timeout = -1;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file24.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "timeout_sec="               << input_handshake_timeout << "\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // unit_test[139] get_ssl_parameter check(handshake_timeout MIN)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, false );

        unlink( ssl_passwd_full_path.c_str() );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "get_ssl_parameter err (handshake_timeout MAX)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // get_ssl_parameter err (handshake_timeout MAX)
    {
        // read parameter clear
        virtualservice_tcp_get_ssl_parameter_clear(vs);

//        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_ca_file = "CAfile.pem";
//        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
//        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
//        std::string input_private_key_filetype = "SSL_FILETYPE_PEM";
//        std::string input_private_key_passwd_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";
//        int         input_verify_options = (SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
//        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE);
//        bool        input_is_tmp_dh_use = true;  // SSL_OP_SINGLE_DH_USE or defalut root is true
//        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
//        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
//        bool        input_is_session_cache_use = true; // "on" deffault root is true
//        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
//        long        input_session_cache_size = 20480;
//        long        input_session_cache_timeout = 300;
        int         input_handshake_timeout = INT_MAX;

        std::string  ssl_passwd           = "test";
        std::string  ssl_passwd_dir       = "./sslfiles/";
        std::string  ssl_passwd_file      = "ssl_password_file25.cf";
        std::string  ssl_passwd_full_path = ssl_passwd_dir + ssl_passwd_file;

        std::ofstream ofs( ssl_passwd_full_path.c_str() );

        ofs << "[ssl]\n";
        ofs << "ca_file=\""                 << input_ca_file << "\"\n";
        ofs << "cert_chain_file=\""         << input_cert_chain_file << "\"\n";
        ofs << "private_key_file=\""        << input_private_key_file << "\"\n";
        ofs << "private_key_passwd_file=\"" << input_private_key_passwd_file << "\"\n";
        ofs << "tmp_dh_file=\""             << input_tmp_dh_file << "\"\n";
        ofs << "timeout_sec="               << input_handshake_timeout << "00000\n";

        ofs.close();

        vs->set_ssl_file_name(ssl_passwd_full_path);

        // MAX over case confirmation on a desk
        // MAX case
        // unit_test[140] get_ssl_parameter check(handshake_timeout MAX)
        bool return_ssl_config = vs->get_ssl_parameter();

        BOOST_CHECK_EQUAL( return_ssl_config, true );
        BOOST_CHECK_EQUAL( vs->get_handshake_timeout() , DEFAULT_SSL_HANDSHAKE_TIMEOUT );

        unlink( ssl_passwd_full_path.c_str() );

    }

    BOOST_MESSAGE( "-------??" );
    delete vs;

}

// set_ssl_config test
void    virtualservice_tcp_set_ssl_config(){
    counter = 1;

    debugg_flug_struct::getInstance().pmcontrol_err_flag() = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag() = false;
    debugg_flug_struct::getInstance().param_exist_flag()   = false;

    l7vs::l7vsd                     vsd;
    boost::asio::io_service         dispatcher;
    l7vs::replication               rep;
    l7vs::virtualservice_element    element;
    //set element value
    element.udpmode                 = false;
    element.tcp_accept_endpoint     = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    element.udp_recv_endpoint       = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    element.realserver_vector.clear();
    element.protocol_module_name    = "PMtest1";
    element.schedule_module_name    = "SMtest1";
    element.protocol_args.clear();
    element.protocol_args.push_back( "testarg" );
    element.protocol_args.push_back( "testarg2" );
    element.sorry_maxconnection     = 1234LL;
    element.sorry_endpoint          = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    element.sorry_flag              = false;
    element.qos_upstream            = 65535ULL;
    element.qos_downstream          = 32767ULL;

    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );

    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( element == vs->get_element() );

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config normal case01" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config normal case01
    {
        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = false;
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();
    
        //! set_ssl_config return data
        // unit_test[141] set_ssl_config normal case01
        BOOST_CHECK_EQUAL( return_ssl_config, true );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config normal case02 (no ca_file)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config normal case02 (no ca_file)
    {
    
        std::string input_ca_file = "";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2);
        bool        input_is_tmp_dh_use = false;
        std::string input_tmp_dh_dir = "";
        std::string input_tmp_dh_file = "";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();
    
        //! set_ssl_config return data
        // unit_test[142] set_ssl_config normal case02 (no ca_file)
        BOOST_CHECK_EQUAL( return_ssl_config, true );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (no ca_file,no_ca_dir)" << std::endl;
    std::cout << "  target : Set root CA path error" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (no ca_file,no_ca_dir)
    {
    
        std::string input_ca_file = "";
        std::string input_ca_dir = "";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2);
        bool        input_is_tmp_dh_use = false;
        std::string input_tmp_dh_dir = "";
        std::string input_tmp_dh_file = "";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();

        //! set_ssl_config return data
        // unit_test[143] set_ssl_config error (no ca_file,no ca_dir)
        BOOST_CHECK_EQUAL( return_ssl_config, false );
        // LOG:Set root CA path error

   }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (no ca_dir)" << std::endl;
    std::cout << "  target : Set root CA file error" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (no_ca_dir)
    {

        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2);
        bool        input_is_tmp_dh_use = false;
        std::string input_tmp_dh_dir = "";
        std::string input_tmp_dh_file = "";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();
    
        //! set_ssl_config return data
        // unit_test[144] set_ssl_config error (no_ca_dir)
        BOOST_CHECK_EQUAL( return_ssl_config, false );
        // LOG:Set root CA file error

   }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (chain file)" << std::endl;
    std::cout << "  target : Set certificate chain file error" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (chain file)
    {
        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
//        std::string input_cert_chain_file = "cert.pem";
        std::string input_cert_chain_file = "hoge.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = false;
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();
    
        //! set_ssl_config return data
        // unit_test[145] set_ssl_config error (chain file)
        BOOST_CHECK_EQUAL( return_ssl_config, false );
        // LOG:Set certificate chain file error

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (set_password_callback)" << std::endl;
    std::cout << "  target : Set password callback error" << std::endl;
    std::cout << "  check on desk" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (set_password_callback)
    {
        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
//        std::string input_private_key_passwd_file = "passwd.txt";
        std::string input_private_key_passwd_file = "hoge.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = false;
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();
    
        //! set_ssl_config return data
        // unit_test[146] set_ssl_config error (set_password_callback)
        BOOST_CHECK_EQUAL( return_ssl_config, false );
        // LOG:Set password callback error

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (private_key_file)" << std::endl;
    std::cout << "  target : Set private key file and filetype error" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (private_key_file)
    {
        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
//        std::string input_private_key_file = "server.key";
        std::string input_private_key_file = "hoge.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = false;
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();
    
        //! set_ssl_config return data
        // unit_test[147] set_ssl_config error (private_key_file)
        BOOST_CHECK_EQUAL( return_ssl_config, false );
        // LOG:Set private key file and filetype error

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (verify_options)" << std::endl;
    std::cout << "  target : Set verify option error" << std::endl;
    std::cout << "  check on desk" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (verify_options)
    {
        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
//        int         input_verify_options = SSL_VERIFY_PEER;
        int         input_verify_options = 0;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = false;
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();

        // verify_options all value success
        // verify_options error case confirmation on a desk
        // unit_test[148] set_ssl_config error (verify_options)
        BOOST_CHECK_EQUAL( return_ssl_config, true );
        // LOG:Set verify option error

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (ssl_options)" << std::endl;
    std::cout << "  target : Set SSL option error" << std::endl;
    std::cout << "  check on desk" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (ssl_options)
    {
        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
//        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_SINGLE_DH_USE);
        long int    input_ssl_options = 0;
        bool        input_is_tmp_dh_use = false;
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();

        // ssl_options all value success
        // ssl_options error case confirmation on a desk
        // unit_test[149] set_ssl_config error (ssl_options) dummy test
        BOOST_CHECK_EQUAL( return_ssl_config, true );

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (temporary Diffie-Hellman parameters file)" << std::endl;
    std::cout << "  target : Set tmp DH file error" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (temporary Diffie-Hellman parameters file)
    {
        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = true;
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
//        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_tmp_dh_file = "hoge.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();
    
        //! set_ssl_config return data
        // unit_test[150] set_ssl_config error (temporary Diffie-Hellman parameters file)
        BOOST_CHECK_EQUAL( return_ssl_config, false );
        // LOG:Set tmp DH file error

    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (cipher_list)" << std::endl;
    std::cout << "  target : Set cipher list error" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (cipher_list)
    {
    
        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = true;
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "xxxxxxxxxxxxxxx";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();
    
        //! set_ssl_config return data
        // unit_test[151] set_ssl_config error (cipher_list)
        BOOST_CHECK_EQUAL( return_ssl_config, false );
        // LOG:Set cipher list error

   }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "set_ssl_config error (session cache)" << std::endl;
    std::cout << "  target : Set session id context error" << std::endl;
    std::cout << "  check on desk" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_ssl_config error (session cache)
    {
        std::string input_ca_file = "CAfile.pem";
        std::string input_ca_dir = "/home/komata/KomataCA/";
        std::string input_cert_chain_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_cert_chain_file = "cert.pem";
        std::string input_private_key_dir = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_file = "server.key";
        boost::asio::ssl::context::file_format
                    input_private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        int         input_verify_options = SSL_VERIFY_PEER;

        std::string input_private_key_passwd_dir  = "/home/komata/KomataCA/SERVER/";
        std::string input_private_key_passwd_file = "passwd.txt";

        int         input_verify_cert_depth = 4;
        long int    input_ssl_options = (SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_SINGLE_DH_USE);
        bool        input_is_tmp_dh_use = true;
        std::string input_tmp_dh_dir = "/home/komata/KomataCA/";
        std::string input_tmp_dh_file = "dh512.pem";
        std::string input_cipher_list = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
        bool        input_is_session_cache_use = true;
        long        input_session_cache_mode = (SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_AUTO_CLEAR);
        long        input_session_cache_size = 20480;
        long        input_session_cache_timeout = 300;

        vs->set_ca_file(input_ca_file);
        vs->set_ca_dir(input_ca_dir);
        vs->set_cert_chain_dir(input_cert_chain_dir);
        vs->set_cert_chain_file(input_cert_chain_file);
        vs->set_private_key_dir(input_private_key_dir);
        vs->set_private_key_file(input_private_key_file);
        vs->set_private_key_filetype(input_private_key_filetype);
        vs->set_private_key_passwd_dir(input_private_key_passwd_dir);
        vs->set_private_key_passwd_file(input_private_key_passwd_file);
        vs->set_verify_options(input_verify_options);
        vs->set_verify_cert_depth(input_verify_cert_depth);
        vs->set_ssl_options(input_ssl_options);
        vs->set_is_tmp_dh_use(input_is_tmp_dh_use);
        vs->set_tmp_dh_dir(input_tmp_dh_dir);
        vs->set_tmp_dh_file(input_tmp_dh_file);
        vs->set_cipher_list(input_cipher_list);
        vs->set_is_session_cache_use(input_is_session_cache_use);
        vs->set_session_cache_mode(input_session_cache_mode);
        vs->set_session_cache_size(input_session_cache_size);
        vs->set_session_cache_timeout(input_session_cache_timeout);

        bool return_ssl_config = vs->set_ssl_config();

        // session id context error case confirmation on a desk
        // unit_test[152] set_ssl_config error (session cache) dummy test
        BOOST_CHECK_EQUAL( return_ssl_config, true );

    }



    BOOST_MESSAGE( "-------??" );
    delete vs;

}


//test case1  create,initialize,run,stop,finalize,destroy(ssl mode normal case)
void    virtualservice_tcp_initialize_to_finalize(){
    counter = 1;

    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().param_exist_flag() = false;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    //set element value
    element.udpmode                    = false;
    element.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    element.udp_recv_endpoint        = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    element.realserver_vector.clear();
    element.protocol_module_name    = "PMtest1";
    element.schedule_module_name    = "SMtest1";
    element.protocol_args.clear();
    element.protocol_args.push_back( "testarg" );
    element.protocol_args.push_back( "testarg2" );
    element.sorry_maxconnection        = 1234LL;
    element.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    element.sorry_flag                = false;
    element.qos_upstream            = 65535ULL;
    element.qos_downstream            = 32767ULL;
   
    element.ssl_file_name = "./sslfiles/ssl_conf.cf";
 
    // unit_test[153]  object create
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------1" );
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, element );

    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( element == vs->get_element() );
    l7vs::error_code    vs_err;
    // unit_test[154]  initialize method call
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------2" );
    BOOST_CHECK( NULL == vs->get_protocol_module() );
    BOOST_CHECK( NULL == vs->get_schedule_module() );

    result.flag = false;
    vs_err.setter(false,"");
    vs->initialize( vs_err );

    bool result_flg;

    if( vs_err == true ) {
        result_flg = true;
    } else if ( vs_err == false ) {
        result_flg = false;
    }

    BOOST_CHECK( result_flg == false );

    //protocol_module_controlのモジュールロードを呼んでいるか(モジュールロード正常終了)
    BOOST_CHECK( NULL != vs->get_protocol_module() );
    //schedule_module_controlのモジュールロードを呼んでいるか(モジュールロード正常終了)
    BOOST_CHECK( NULL != vs->get_schedule_module() );

    unsigned int pool_default_size = l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT;
    unsigned int pool_size = vs->get_pool_sessions().size();

    //session_poolを作成しているか(デフォルトサイズで作成)
    BOOST_CHECK( pool_default_size == pool_size );

    // set option
    bool& defer_accept_opt = vs->get_defer_accept_opt();
    defer_accept_opt = true;
    int& defer_accept_val = vs->get_defer_accept_val();
    defer_accept_val = 1;

    // unit_test[155]  run method test
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------3" );
    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );

    usleep( 1500000 );
    //1秒待って、ProtocolModule/ScheduleModuleのreplication_interruptが呼ばれることを確認
    //デフォルト設定は500msなので2回
    BOOST_MESSAGE( debugg_flug_struct::getInstance().get_pm_rep_count() );
    BOOST_MESSAGE( debugg_flug_struct::getInstance().get_sm_rep_count() );
    BOOST_CHECK( 2 == debugg_flug_struct::getInstance().get_pm_rep_count() );
    BOOST_CHECK( 2 == debugg_flug_struct::getInstance().get_sm_rep_count() );

    // unit_test[156]  set TCP_DEFER_ACCEPT test
    int val = 0;
    socklen_t len = sizeof(val);
    boost::system::error_code ec;
    int err = ::getsockopt(vs->get_acceptor().native(),IPPROTO_TCP,TCP_DEFER_ACCEPT,&val,&len);
    BOOST_CHECK(!err);
    BOOST_CHECK(val);
    
    // unit_test[157]  stop method test
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------4" );
    vs->stop();
    usleep( 1000 );

    // unit_test[158]  release_session method test(run直後はwaiting_sessionsに入っているため、release_sessionではプールに戻らない)
    std::cout << counter++ << std::endl;
//     BOOST_MESSAGE( "-------5" );
//     BOOST_CHECK( vs->get_pool_sessions().size() == (l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-1) );
//     BOOST_CHECK( vs->get_waiting_sessions().size() == 1 );
//     BOOST_CHECK( vs->get_active_sessions().size() == 0 );
//     vs->release_session( vs->get_waiting_sessions().begin()->second->get_upthread_id() );
//     BOOST_CHECK( vs->get_pool_sessions().size() == static_cast<size_t>( l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT) );
//     BOOST_CHECK( vs->get_active_sessions().size() == 0 );
//     BOOST_CHECK( vs->get_waiting_sessions().size() == 0 );

    // unit_test[159]  stop method test(call twice)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------6" );
    vs->stop();

    vs_main.join();

    // unit_test[160]  finalize method call
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------7" );
    vs_err.setter(false,"");
    vs->finalize( vs_err );
    BOOST_CHECK( vs_err == false );

    // unit_test[161]  finalizeを2回連続で呼んでみる
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------8" );
    vs_err.setter(false,"");
    vs->finalize( vs_err );
    BOOST_CHECK( vs_err == false );


    // unit_test[162]  object destroy
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-------9" );
    delete vs;
}

//send access_log_flag ON/OFF to TCP session
void    virtualservice_tcp_handle_accept_test(){

    //access_log_flag:true
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 1;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    // unit_test[163] send access_log_flag ON to TCP session
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "handle_accept_test (access_log_flag ON)" << std::endl;
    std::cout << "  target:set_virtual_service_message called : ACCESS_LOG_ON" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    boost::thread    cl_thread( &client );
    // session_access_log_output_mode_change STUB OUTPUT
    // set_virtual_service_message called : ACCESS_LOG_ON

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );
    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }
    //access_log_flag:false
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 0;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    // unit_test[164] send access_log_flag OFF to TCP session
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "handle_accept_test (access_log_flag OFF)" << std::endl;
    std::cout << "  target:set_virtual_service_message called : ACCESS_LOG_OFF" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    boost::thread    cl_thread( &client );
    // session_access_log_output_mode_change STUB OUTPUT
    // set_virtual_service_message called : ACCESS_LOG_OFF

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );
    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }
}

// l7vs::virtualservice_tcp::initialize test
void    virtualservice_tcp_initialize_test(){

    //flag=ON,file=not empty,key=""
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 1;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    // unit_test[165] initialize (flag=ON,file=not empty,key="")
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "initialize_test (flag=ON,file=not empty,key=\"\")" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), true );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    // this test is not use client
    //boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );


    sleep( 10 );
    delete vs;
    }

    usleep( 10000 );

    //flag=ON,file=not empty,key="key_info"
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 1;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "keyinfo";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    // unit_test[166] initialize (flag=ON,file=not empty,key="keyinfo")
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "initialize_test (flag=ON,file=not empty,key=\"keyinfo\")" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), true );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "keyinfo" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "keyinfo" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    //this test not use cleint 
    //boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    //usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    // this test not use lient.
    // cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    // vs is not delete outside.
    // delete vs;
    }

    //flag=OFF,file=not empty,key=""
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 0;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    // unit_test[167] initialize (flag=OFF,file=not empty,key="")
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "initialize_test (flag=OFF,file=not empty,key=\"\")" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), false );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    //boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    //usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    //cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    //delete vs;
    }

    //flag=OFF,file=not empty,key="keyinfo"
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 0;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "keyinfo";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    // unit_test[168] initialize (flag=OFF,file=not empty,key="keyinfo")
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "initialize_test (flag=OFF,file=not empty,key=\"keyinfo\")" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), false );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "keyinfo" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "keyinfo" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    //boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    //usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    //cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    //delete vs;
    }

    //flag=OFF,file=empty,key=""
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 0;
    elem1.access_log_file_name   = "";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    // unit_test[169] initialize (flag=OFF,file=empty,key="")
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "initialize_test (flag=OFF,file=empty,key=\"\")" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), false );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    //boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    //usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    //cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    //delete vs;
    }

    //flag=ON,file=empty,key=""
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 1;
    elem1.access_log_file_name   = "";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    // unit_test[170] initialize (flag=ON,file=empty,key="")
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "initialize_test (flag=ON,file=empty,key=\"\")" << std::endl;
    std::cout << "  target:access log file name not set error" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs_err, true );
    BOOST_CHECK_EQUAL( vs_err.get_message(), "access log file name not set error" );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    //boost::thread    cl_thread( &client );
    //2秒待ってsessionプールのサイズをチェック
    //usleep( 2000000 );
    //BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    //cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    //delete vs;
    }

    //flag=ON,file=not empty,access_log_instance NG
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 1;
    elem1.access_log_file_name   = "access_log.log";
//    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
//    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
//    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    // unit_test[171] initialize (flag=ON,file=not empty,access_log_instance NG)
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "initialize_test (flag=ON,file=not empty,access_log_instance NG)" << std::endl;
    std::cout << "  target:access logger Instance acquisition" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    vs->initialize( vs_err );

//    BOOST_CHECK_EQUAL( vs_err, true );
//    BOOST_CHECK_EQUAL( vs_err.get_message(), "access log class instance create failed" );

/*
    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );
    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();
*/
    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }

}

// l7vs::virtualservice_tcp::finalize test
void    virtualservice_tcp_finalize_test(){

    // erase_logger_implement_access OK
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 1;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), true );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );

// this check is not use
//    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();

    // unit_test[172] finalize (erase_logger_implement_access OK)
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "finalize_test (erase_logger_implement_access OK)" << std::endl;
    std::cout << "NOT OUTPUT LOG access logger instance erase err" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // NOT OUTPUT LOG access logger instance erase err
    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }

    // erase_logger_implement_access NG
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 1;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), true );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );

// this check is not use
//    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    //1秒待ってmainをSTOP
    usleep( 1000000 );
    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();

    // unit_test[173] finalize (erase_logger_implement_access NG)
    vs->set_access_log_file_name("hoge.log");
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "finalize_test (erase_logger_implement_access NG)" << std::endl;
    std::cout << "OUTPUT LOG access logger instance erase err" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // OUTPUT LOG access logger instance erase err
    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }

}


// edit_virtualservice test
void    virtualservice_tcp_edit_virtualservice_test(){

    // edit(flag:OFF,filename:not empty)->(flag:ON)
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 0;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), false );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );

// this check ins not use
//    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;

    usleep( 1000000 );

    // create edit element
    l7vs::virtualservice_element elem2;

    elem2.udpmode                = false;
    elem2.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem2.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem2.realserver_vector.clear();
    elem2.protocol_module_name   = "PMtest1";
    elem2.schedule_module_name   = "SMtest1";
    elem2.protocol_args.clear();
    elem2.sorry_maxconnection    = 1234LL;
    elem2.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem2.sorry_flag             = false;
    elem2.qos_upstream           = 65535ULL;
    elem2.qos_downstream         = 32767ULL;
    elem2.ssl_file_name          = "";
    elem2.access_log_flag        = 1;
//    elem2.access_log_file_name   = "access_log.log";
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
//    elem2.access_log_rotate_key_info = "";
//    elem2.access_log_rotate_verbose_info = "dummy";

    // unit_test[174] edit(flag:OFF,filename:not empty)->(flag:ON)
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "edit_virtualservice_test" << std::endl;
    std::cout << "(flag:OFF,filename:not empty)->(flag:ON)" << std::endl;
    std::cout << "  target:set_virtual_service_message called : ACCESS_LOG_ON" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    // set_virtual_service_message called : ACCESS_LOG_ON
    vs_err.setter(false,"");
    vs->edit_virtualservice( elem2, vs_err );

    BOOST_CHECK_EQUAL( vs_err, false );
    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), true );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }

    // edit(flag:ON,filename:not empty)->(flag:ON)
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 1;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), true );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );

// this check is not use
//    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;

    usleep( 1000000 );

    // create edit element
    l7vs::virtualservice_element elem2;

    elem2.udpmode                = false;
    elem2.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem2.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem2.realserver_vector.clear();
    elem2.protocol_module_name   = "PMtest1";
    elem2.schedule_module_name   = "SMtest1";
    elem2.protocol_args.clear();
    elem2.sorry_maxconnection    = 1234LL;
    elem2.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem2.sorry_flag             = false;
    elem2.qos_upstream           = 65535ULL;
    elem2.qos_downstream         = 32767ULL;
    elem2.ssl_file_name          = "";
    elem2.access_log_flag        = 1;
//    elem2.access_log_file_name   = "access_log.log";
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
//    elem2.access_log_rotate_key_info = "";
//    elem2.access_log_rotate_verbose_info = "dummy";

    // unit_test[175] edit(flag:ON,filename:not empty)->(flag:ON)
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "edit_virtualservice_test" << std::endl;
    std::cout << "(flag:ON,filename:not empty)->(flag:ON)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem2, vs_err );

    BOOST_CHECK_EQUAL( vs_err, false );
    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), true );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }

    // edit(flag:ON,filename:not empty)->(flag:OFF)
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 1;
    elem1.access_log_file_name   = "access_log.log";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), true );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );

// this check is not use
//    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;

    usleep( 1000000 );

    // create edit element
    l7vs::virtualservice_element elem2;

    elem2.udpmode                = false;
    elem2.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem2.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem2.realserver_vector.clear();
    elem2.protocol_module_name   = "PMtest1";
    elem2.schedule_module_name   = "SMtest1";
    elem2.protocol_args.clear();
    elem2.sorry_maxconnection    = 1234LL;
    elem2.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem2.sorry_flag             = false;
    elem2.qos_upstream           = 65535ULL;
    elem2.qos_downstream         = 32767ULL;
    elem2.ssl_file_name          = "";
    elem2.access_log_flag        = 0;
//    elem2.access_log_file_name   = "access_log.log";
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
//    elem2.access_log_rotate_key_info = "";
//    elem2.access_log_rotate_verbose_info = "dummy";

    // unit_test[176] edit(flag:ON,filename:not empty)->(flag:OFF)
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "edit_virtualservice_test" << std::endl;
    std::cout << "(flag:ON,filename:not empty)->(flag:OFF)" << std::endl;
    std::cout << "NOT OUTPUT LOG :set_virtual_service_message called : ACCESS_LOG_OFF" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem2, vs_err );

    BOOST_CHECK_EQUAL( vs_err, false );
    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), false );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }

    // edit(flag:OFF,filename:empty)->(flag:OFF)
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 0;
    elem1.access_log_file_name   = "";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), false );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );

// this check is not use
//    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;

    usleep( 1000000 );

    // create edit element
    l7vs::virtualservice_element elem2;

    elem2.udpmode                = false;
    elem2.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem2.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem2.realserver_vector.clear();
    elem2.protocol_module_name   = "PMtest1";
    elem2.schedule_module_name   = "SMtest1";
    elem2.protocol_args.clear();
    elem2.sorry_maxconnection    = 1234LL;
    elem2.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem2.sorry_flag             = false;
    elem2.qos_upstream           = 65535ULL;
    elem2.qos_downstream         = 32767ULL;
    elem2.ssl_file_name          = "";
    elem2.access_log_flag        = 0;
//    elem2.access_log_file_name   = "access_log.log";
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
//    elem2.access_log_rotate_key_info = "";
//    elem2.access_log_rotate_verbose_info = "dummy";

    // unit_test[177] edit(flag:ON,filename:empty)->(flag:OFF)
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "edit_virtualservice_test" << std::endl;
    std::cout << "(flag:ON,filename:empty)->(flag:OFF)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem2, vs_err );

    BOOST_CHECK_EQUAL( vs_err, false );
    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), false );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }

    // edit(flag:OFF,filename:empty)->(flag:ON) NG
    {
    debugg_flug_struct::getInstance().param_exist_flag() = false;
    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;

    l7vs::error_code            vs_err;
    boost::system::error_code   test_err;

    std::stringstream    tmp_tcp_ep;
    std::stringstream    tmp_udp_ep;
    std::stringstream    tmp_sorry_ep;

    l7vs::l7vsd                  vsd;
    boost::asio::io_service      dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element elem1;

    elem1.udpmode                = false;
    elem1.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name   = "PMtest1";
    elem1.schedule_module_name   = "SMtest1";
    elem1.protocol_args.clear();
    elem1.sorry_maxconnection    = 1234LL;
    elem1.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem1.sorry_flag             = false;
    elem1.qos_upstream           = 65535ULL;
    elem1.qos_downstream         = 32767ULL;
    elem1.ssl_file_name          = "";
    elem1.access_log_flag        = 0;
    elem1.access_log_file_name   = "";
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
    elem1.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
    elem1.access_log_rotate_key_info = "";
    elem1.access_log_rotate_verbose_info = "dummy";

    //vs作成
    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
    BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
    BOOST_CHECK( elem1 == vs->get_element() );

    vs_err.setter(false,"");
    vs->initialize( vs_err );

    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), false );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
    usleep( 2000000 );

    boost::thread    cl_thread( &client );

    //2秒待ってsessionプールのサイズをチェック
    usleep( 2000000 );

// this check is not use
//    BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;

    usleep( 1000000 );

    // create edit element
    l7vs::virtualservice_element elem2;

    elem2.udpmode                = false;
    elem2.tcp_accept_endpoint    = 
            tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem2.udp_recv_endpoint      = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem2.realserver_vector.clear();
    elem2.protocol_module_name   = "PMtest1";
    elem2.schedule_module_name   = "SMtest1";
    elem2.protocol_args.clear();
    elem2.sorry_maxconnection    = 1234LL;
    elem2.sorry_endpoint         = 
            tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
    elem2.sorry_flag             = false;
    elem2.qos_upstream           = 65535ULL;
    elem2.qos_downstream         = 32767ULL;
    elem2.ssl_file_name          = "";
    elem2.access_log_flag        = 1;
//    elem2.access_log_file_name   = "access_log.log";
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-type" , "size" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-backup-index" , "1" ) );
//    elem2.access_log_rotate_arguments.insert( std::make_pair( "--ac-rotate-max-filesize" , "10M" ) );
//    elem2.access_log_rotate_key_info = "";
//    elem2.access_log_rotate_verbose_info = "dummy";

    // unit_test[178] edit(flag:ON,filename:empty)->(flag:ON) NG
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "edit_virtualservice_test" << std::endl;
    std::cout << "(flag:ON,filename:empty)->(flag:ON) NG" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    vs_err.setter(false,"");
    vs->edit_virtualservice( elem2, vs_err );

    BOOST_CHECK_EQUAL( vs_err, true );
    BOOST_CHECK_EQUAL( vs_err.get_message(), "access log flag change err." );
    BOOST_CHECK_EQUAL( vs->get_access_log_flag(), false );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_key_info, "none" );
    BOOST_CHECK_EQUAL( vs->get_element().access_log_rotate_verbose_info, "" );
    BOOST_CHECK_EQUAL( vs->get_access_log_file_name(), vs->get_element().access_log_file_name );
    BOOST_CHECK( vs->get_access_log_rotate_arguments() == elem1.access_log_rotate_arguments );

    vs->stop();

    usleep( 1000 );
    cl_thread.join();
    vs_main.join();

    vs_err.setter(false,"");
    vs->finalize( vs_err );

    delete vs;
    }

}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    // create unit test suite
    test_suite* ts = BOOST_TEST_SUITE( "virtualservice_base_test" );

    // add test case to test suite
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test1 ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test2 ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test3 ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test4 ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test5 ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_set_socket_option ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_get_ssl_password ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_conv_verify_option ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_conv_ssl_option) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_get_ssl_parameter ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_set_ssl_config ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_initialize_to_finalize ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_handle_accept_test ) );
    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_initialize_test ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_finalize_test ) );
//    ts->add( BOOST_TEST_CASE( &virtualservice_tcp_edit_virtualservice_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}
