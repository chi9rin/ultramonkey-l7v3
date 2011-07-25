
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "session_thread_control.h"
#include "l7vsd.h"
#include "tcp_socket_option.h"

#include "stub.h"


using namespace boost::unit_test;
using namespace l7vs;

typedef boost::shared_ptr<l7vs::tcp_session>                session_type;
typedef boost::shared_ptr<l7vs::session_thread_control>        stc_type;

int    counter;

class    test_thread{
    boost::function<void()>        access_func;
    boost::mutex                func_mutex;
    boost::thread                acc_thread;
    bool                        stop_flag;
    boost::mutex                flag_mutex;
    unsigned int                call_cnt;
    void    run(){
        for( unsigned int i = 0; i < call_cnt; ++i ){
            {
                boost::mutex::scoped_lock( flag_mutex );
                if( stop_flag )break;
            }
            boost::mutex::scoped_lock( func_mutex );
            access_func();
        }
    }
public:
    test_thread(unsigned int set_call_cnt = UINT_MAX){ call_cnt = set_call_cnt; }
    ~test_thread(){ stop(); acc_thread.join(); }
    boost::thread::id    get_id(){ return acc_thread.get_id(); }
    void    start( boost::function<void()>    in_func ){
        {
            boost::mutex::scoped_lock( flag_mutex );
            stop_flag = false;
        }
        boost::mutex::scoped_lock( func_mutex );
        access_func = in_func;
        acc_thread = boost::thread( &test_thread::run, this );
    }
    void    stop(){
        boost::mutex::scoped_lock( flag_mutex );
        stop_flag = true;
    }
};


//test case1. 全メソッドの正常動作確認
void    stc_method_test1(){
    counter = 0;

// unit_test[1]  session_thread_controlオブジェクトの作成
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----1" );
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    l7vs::virtualservice_tcp    tcpservice( vsd, rep, element );
//     session_type    session( new l7vs::tcp_session( tcpservice, dispatcher ) );

    l7vs::tcp_socket_option_info                set_sock_opt;
    boost::asio::ip::tcp::endpoint        tcp_accept_endpoint;
    bool                           ssl_virtualservice_mode_flag = false;
    boost::asio::ssl::context            sslcontext(dispatcher, DEFAULT_SSL_METHOD);
    bool                        is_session_cache_use = false;
    int                        handshake_timeout = 300;

    l7vs::tcp_session*    sess    = new l7vs::tcp_session(tcpservice,
                                      dispatcher,
                                      set_sock_opt,
                                      element.tcp_accept_endpoint,
                                      ssl_virtualservice_mode_flag,
                                      sslcontext,
                                      is_session_cache_use,
                                      handshake_timeout,
                                      NULL);


    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
    cpu_set_t    vsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( address );
    cpu_set_t    rsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( "eth0" );
    stc_type        stc( new l7vs::session_thread_control( sess,
                         vsnic_cpumask, rsnic_cpumask, 0 ) );

//
// unit_test[2]  get_sessionメソッドのテスト
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----2" );
//     BOOST_CHECK_EQUAL( session, stc->get_session() );

//スレッドID取得のテスト
// unit_test[3]  上りスレッドID取得
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----3" );
    std::cout << "upthread id : " << stc->get_upthread_id() << std::endl;
// unit_test[4]  下りスレッドID取得
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----4" );
    std::cout << "downthread id : " << stc->get_downthread_id() << std::endl;

//上りスレッド

//パターン１
// unit_test[5]  上りスレッドスタート
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----5" );
    stc->startupstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 4000000 );
// unit_test[6]  上りスレッドストップ
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----6" );
    stc->stopupstream();
//下りスレッド

// unit_test[7]  下りスレッドスタート
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----7" );
    stc->startdownstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 4000000 );
// unit_test[8]  下りスレッドストップ
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----8" );
    stc->stopdownstream();


// unit_test[9]  sessionの上りスレッドが外から停止された場合
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----9" );
    stc->startupstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 1000 );
//    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
    stc->session_sorry_enable();
    usleep( 1000 );
    stc->stopupstream();
//    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );
    stc->session_sorry_disable();

// unit_test[10]  sessionの下りスレッドが外から停止された場合
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "----10" );
    stc->startdownstream();
    usleep( 1000 );
//    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
    stc->session_pause_on();
    usleep( 1000 );
    stc->stopdownstream();
//    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );
    stc->session_pause_off();

// unit_test[12]  停止スレッドの待ち合わせ
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "----12" );
    stc->join();
    usleep( 1 );
}

//test case2. スレッドの停止と再開
void    stc_method_test2(){
//session_thread_controlオブジェクトの作成
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    l7vs::virtualservice_tcp    tcpservice( vsd, rep, element );
//     session_type    session( new l7vs::tcp_session( tcpservice, dispatcher ) );

    l7vs::tcp_socket_option_info                set_sock_opt;
    boost::asio::ip::tcp::endpoint        tcp_accept_endpoint;
    bool                           ssl_virtualservice_mode_flag = false;
    boost::asio::ssl::context            sslcontext(dispatcher, DEFAULT_SSL_METHOD);
    bool                        is_session_cache_use = false;
    int                        handshake_timeout = 300;

    l7vs::tcp_session*    sess    = new l7vs::tcp_session(tcpservice,
                                      dispatcher,
                                      set_sock_opt,
                                      element.tcp_accept_endpoint,
                                      ssl_virtualservice_mode_flag,
                                      sslcontext,
                                      is_session_cache_use,
                                      handshake_timeout,
                                      NULL);


    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
    cpu_set_t    vsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( address );
    cpu_set_t    rsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( "eth0" );
    stc_type        stc( new l7vs::session_thread_control( sess,
                         vsnic_cpumask, rsnic_cpumask, 0 ) );

// unit_test[12]  上りスレッドと下りスレッドを開始し、順番に停止する(sessionループは自然に終了)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "----12" );
    stc->startupstream();
    stc->startdownstream();
    usleep( 5000000 );

// unit_test[13]  上りスレッドと下りスレッドを開始し、下りスレッド・上りスレッドの順に停止する(sessionループは外から停止)
    std::cout << counter++ << std::endl;
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

// unit_test[14]  上りスレッドを外部から停止後に上りスレッドを再開
    std::cout << counter++ << std::endl;
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

// unit_test[15]  下りスレッドストップ後に再開
    std::cout << counter++ << std::endl;
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

// unit_test[16]  上りスレッドを連続で開始指示した場合
    std::cout << counter++ << std::endl;
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

// unit_test[17]  下りスレッドを連続で開始指示した場合
    std::cout << counter++ << std::endl;
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

// unit_test[18]  上りスレッドを連続で開始・停止指示した場合(ループカウント0で出力)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "----18" );
    stc->startupstream();
    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_ENABLE );
    usleep( 100 );
    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SORRY_STATE_DISABLE );

// unit_test[19]  下りスレッドを連続で開始・停止指示した場合(ループカウント0で出力)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "----19" );
    stc->startdownstream();
    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
    usleep( 100 );
    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );

std::cout << "join 1" << std::endl;

//停止スレッドの待ち合わせ
    stc->join();
//join後のスレッドID取得のテスト
// unit_test[20]  上りスレッドID取得
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----20" );
    std::cout << "upthread id : " << stc->get_upthread_id() << std::endl;
// unit_test[21]  下りスレッドID取得
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----21" );
    std::cout << "downthread id : " << stc->get_downthread_id() << std::endl;
}


//test case3
void    stc_method_test3(){
// unit_test[23]  joinを呼ばずにオブジェクト廃棄する
    std::cout << counter++ << std::endl;
/*    BOOST_MESSAGE( "-----23" );
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    l7vs::virtualservice_tcp    tcpservice( vsd, rep, element );
    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
    cpu_set_t    vsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( address );
    cpu_set_t    rsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( "eth0" );
    stc_type        stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ),
                         vsnic_cpumask, rsnic_cpumask, 0 ) );
*/
}

//test case4
void    stc_method_test4(){
// unit_test[24]  スレッドを動かしたまま廃棄する
    std::cout << counter++ << std::endl;
/*    BOOST_MESSAGE( "-----24" );
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    l7vs::virtualservice_tcp    tcpservice( vsd, rep, element );
    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
    cpu_set_t    vsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( address );
    cpu_set_t    rsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( "eth0" );
    stc_type        stc( new l7vs::session_thread_control( new l7vs::tcp_session( tcpservice, dispatcher ),
                         vsnic_cpumask, rsnic_cpumask, 0 ) );

    //start thread
    stc->startupstream();
    stc->startdownstream();
    usleep( 1 );*/
}

//test case5. 外部スレッドからのメソッドアクセス
void    stc_method_test5(){
//session_thread_controlオブジェクトの作成
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    l7vs::virtualservice_tcp    tcpservice( vsd, rep, element );
//     session_type    session( new l7vs::tcp_session( tcpservice, dispatcher ) );



    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
    cpu_set_t    vsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( address );
    cpu_set_t    rsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( "eth0" );

    l7vs::tcp_socket_option_info                set_sock_opt;
    boost::asio::ip::tcp::endpoint        tcp_accept_endpoint;
    bool                           ssl_virtualservice_mode_flag = false;
    boost::asio::ssl::context            sslcontext(dispatcher, DEFAULT_SSL_METHOD);
    bool                        is_session_cache_use = false;
    int                        handshake_timeout = 300;

    l7vs::tcp_session*    sess    = new l7vs::tcp_session(tcpservice,
                                      dispatcher,
                                      set_sock_opt,
                                      element.tcp_accept_endpoint,
                                      ssl_virtualservice_mode_flag,
                                      sslcontext,
                                      is_session_cache_use,
                                      handshake_timeout,
                                      NULL);

    stc_type        stc( new l7vs::session_thread_control( sess,
                         vsnic_cpumask, rsnic_cpumask, 0 ) );

    test_thread    thread1;

// unit_test[25]  スレッドを開始して、別スレッドからスレッド停止メソッドを呼ぶ(上りスレッド)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----25" );
    std::cout << "thread id : " << thread1.get_id() << std::endl; 
    stc->startupstream();
    usleep( 2000000 );
    thread1.start( boost::bind( &l7vs::session_thread_control::stopupstream, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 2000000 );
    std::cout << "thread id : " << thread1.get_id() << std::endl;
    thread1.stop();

// unit_test[26]  スレッドを開始して、別スレッドからスレッド停止メソッドを呼ぶ(下りスレッド)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----26" );
    std::cout << "thread id : " << thread1.get_id() << std::endl; 
    stc->startdownstream();
    usleep( 2000000 );
    thread1.start( boost::bind( &l7vs::session_thread_control::stopdownstream, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 2000000 );
    std::cout << "thread id : " << thread1.get_id() << std::endl;
    thread1.stop();

// unit_test[27]  スレッドが停止すると同時に別スレッドから開始メソッドが呼ばれる(上りスレッド)
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----27" );
    std::cout << "thread id : " << thread1.get_id() << std::endl; 
    stc->startupstream();
    usleep( 2000000 );
    thread1.start( boost::bind( &l7vs::session_thread_control::startupstream, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 2000000 );
    std::cout << "thread id : " << thread1.get_id() << std::endl;
    thread1.stop();

// unit_test[28]  スレッドが停止すると同時に別スレッドから開始メソッドが呼ばれる(下りスレッド)
    std::cout << counter++ << std::endl;
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

//test case6. スレッド停止と同時にjoinが呼ばれる・その１(上りスレッド)
void    stc_method_test6(){
//session_thread_controlオブジェクトの作成
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    l7vs::virtualservice_tcp    tcpservice( vsd, rep, element );
//     session_type    session( new l7vs::tcp_session( tcpservice, dispatcher ) );
    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
    cpu_set_t    vsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( address );
    l7vs::tcp_socket_option_info                set_sock_opt;
    boost::asio::ip::tcp::endpoint        tcp_accept_endpoint;
    bool                           ssl_virtualservice_mode_flag = false;
    boost::asio::ssl::context            sslcontext(dispatcher, DEFAULT_SSL_METHOD);
    bool                        is_session_cache_use = false;
    int                        handshake_timeout = 300;

    l7vs::tcp_session*    sess    = new l7vs::tcp_session(tcpservice,
                                      dispatcher,
                                      set_sock_opt,
                                      element.tcp_accept_endpoint,
                                      ssl_virtualservice_mode_flag,
                                      sslcontext,
                                      is_session_cache_use,
                                      handshake_timeout,
                                      NULL);
    cpu_set_t    rsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( "eth0" );



    stc_type        stc( new l7vs::session_thread_control( sess,
                         vsnic_cpumask, rsnic_cpumask, 0 ) );

    test_thread    thread1(1);

// unit_test[29]  上りスレッド停止と同時にjoinを呼ぶ
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----29" );
    stc->startupstream();
    usleep( 2000000 );
    thread1.start( boost::bind( &l7vs::session_thread_control::join, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 2000000 );
    thread1.stop();
}

//test case7. スレッド停止と同時にjoinが呼ばれる・その2(下りスレッド)
void    stc_method_test7(){
//session_thread_controlオブジェクトの作成
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    l7vs::virtualservice_tcp    tcpservice( vsd, rep, element );
//     session_type    session( new l7vs::tcp_session( tcpservice, dispatcher ) );
    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
    cpu_set_t    vsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( address );
    cpu_set_t    rsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( "eth0" );


    l7vs::tcp_socket_option_info                set_sock_opt;
    boost::asio::ip::tcp::endpoint        tcp_accept_endpoint;
    bool                           ssl_virtualservice_mode_flag = false;
    boost::asio::ssl::context            sslcontext(dispatcher, DEFAULT_SSL_METHOD);
    bool                        is_session_cache_use = false;
    int                        handshake_timeout = 300;

    l7vs::tcp_session*    sess    = new l7vs::tcp_session(tcpservice,
                                      dispatcher,
                                      set_sock_opt,
                                      element.tcp_accept_endpoint,
                                      ssl_virtualservice_mode_flag,
                                      sslcontext,
                                      is_session_cache_use,
                                      handshake_timeout,
                                      NULL);


    stc_type        stc( new l7vs::session_thread_control( sess,
                         vsnic_cpumask, rsnic_cpumask, 0 ) );

    test_thread    thread1(1);

// unit_test[30]  下りスレッド停止と同時にjoinを呼ぶ
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----30" );
    stc->startdownstream();
    usleep( 2000000 );
    thread1.start( boost::bind( &l7vs::session_thread_control::join, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 2000000 );
    thread1.stop();
}

//test case8. スレッド停止と同時にjoinが呼ばれる・その3(上下スレッド)
void    stc_method_test8(){
//session_thread_controlオブジェクトの作成
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    l7vs::virtualservice_tcp    tcpservice( vsd, rep, element );
//     session_type    session( new l7vs::tcp_session( tcpservice, dispatcher ) );
    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
    cpu_set_t    vsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( address );
    cpu_set_t    rsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( "eth0" );

    l7vs::tcp_socket_option_info                set_sock_opt;
    boost::asio::ip::tcp::endpoint        tcp_accept_endpoint;
    bool                           ssl_virtualservice_mode_flag = false;
    boost::asio::ssl::context            sslcontext(dispatcher, DEFAULT_SSL_METHOD);
    bool                        is_session_cache_use = false;
    int                        handshake_timeout = 300;

    l7vs::tcp_session*    sess    = new l7vs::tcp_session(tcpservice,
                                      dispatcher,
                                      set_sock_opt,
                                      element.tcp_accept_endpoint,
                                      ssl_virtualservice_mode_flag,
                                      sslcontext,
                                      is_session_cache_use,
                                      handshake_timeout,
                                      NULL);

    stc_type        stc( new l7vs::session_thread_control( sess,
                         vsnic_cpumask, rsnic_cpumask, 0 ) );

    test_thread    thread1(1);

// unit_test[31]  上下スレッド停止と同時にjoinを呼ぶ
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----31" );
    stc->startupstream();
    stc->startdownstream();
    usleep( 2000000 );
    thread1.start( boost::bind( &l7vs::session_thread_control::join, stc ) );
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 2000000 );
    thread1.stop();
}

//test case9. sorryモード変更関数及びアクセスログ出力設定/解除関数の正常動作確認
void    stc_method_test9(){

// unit_test[33]  session_thread_controlオブジェクトの作成
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----33" );
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    element;
    l7vs::virtualservice_tcp    tcpservice( vsd, rep, element );
//     session_type    session( new l7vs::tcp_session( tcpservice, dispatcher ) );

    l7vs::tcp_socket_option_info                set_sock_opt;
    boost::asio::ip::tcp::endpoint        tcp_accept_endpoint;
    bool                           ssl_virtualservice_mode_flag = false;
    boost::asio::ssl::context            sslcontext(dispatcher, DEFAULT_SSL_METHOD);
    bool                        is_session_cache_use = false;
    int                        handshake_timeout = 300;

    l7vs::tcp_session*    sess    = new l7vs::tcp_session(tcpservice,
                                      dispatcher,
                                      set_sock_opt,
                                      element.tcp_accept_endpoint,
                                      ssl_virtualservice_mode_flag,
                                      sslcontext,
                                      is_session_cache_use,
                                      handshake_timeout,
                                      NULL);


    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
    cpu_set_t    vsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( address );
    cpu_set_t    rsnic_cpumask    = debugg_flug_struct::getInstance().get_cpu_mask( "eth0" );
    stc_type        stc( new l7vs::session_thread_control( sess,
                         vsnic_cpumask, rsnic_cpumask, 0 ) );

//
// unit_test[34]  get_sessionメソッドのテスト
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----34" );
//     BOOST_CHECK_EQUAL( session, stc->get_session() );

//スレッドID取得のテスト
// unit_test[35]  上りスレッドID取得
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----35" );
    std::cout << "upthread id : " << stc->get_upthread_id() << std::endl;
// unit_test[36]  下りスレッドID取得
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----36" );
    std::cout << "downthread id : " << stc->get_downthread_id() << std::endl;

//上りスレッド

//パターン１
// unit_test[37]  上りスレッドスタート
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----37" );
    stc->startupstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 4000000 );
// unit_test[38]  上りスレッドストップ
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----38" );
    stc->stopupstream();
//下りスレッド

// unit_test[39]  下りスレッドスタート
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----39" );
    stc->startdownstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 4000000 );
// unit_test[40]  下りスレッドストップ
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----40" );
    stc->stopdownstream();

// unit_test[41]  sessionの上りスレッドが外から停止された場合 パターン２
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----41" );
    stc->startupstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 1000 );
    stc->session_sorry_mode_change(1);
    usleep( 1000 );
    stc->stopupstream();
    stc->session_sorry_mode_change(INT_MAX);

// unit_test[42]  sessionの上りスレッドが外から停止された場合
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----42" );
    stc->startupstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 1000 );
    stc->session_accesslog_output_mode_on();
    usleep( 1000 );
    stc->stopupstream();
    stc->session_accesslog_output_mode_off();

// unit_test[43]  sessionの上りスレッドが外から停止された場合
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "-----43" );
    stc->startupstream();
//sleepを入れないとsessionのループに入る前にEXITしてしまう
    usleep( 1000 );
    stc->session_access_log_output_mode_change(true);
    usleep( 1000 );
    stc->stopupstream();
    stc->session_access_log_output_mode_change(false);

// unit_test[44]  sessionの下りスレッドが外から停止された場合
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "----44" );
    stc->startdownstream();
    usleep( 1000 );
//    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_ON );
    stc->session_pause_on();
    usleep( 1000 );
    stc->stopdownstream();
//    stc->get_session()->set_virtual_service_message( l7vs::tcp_session::SESSION_PAUSE_OFF );
    stc->session_pause_off();

// unit_test[45]  停止スレッドの待ち合わせ
    std::cout << counter++ << std::endl;
    BOOST_MESSAGE( "----45" );
    stc->join();
    usleep( 1 );
}




test_suite*    init_unit_test_suite( int argc, char* argv[] ){

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
    ts->add( BOOST_TEST_CASE( &stc_method_test9 ) );

    framework::master_test_suite().add( ts );

    return 0;
}

