
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "stub.h"
#include "virtualservice.h"
#include "vs_tcp.h"

#include "protocol_module_test1.h"
#include "schedule_module_test1.h"

using namespace boost::unit_test;

typedef    boost::asio::ip::tcp::endpoint    tcp_ep_type;
typedef    boost::asio::ip::udp::endpoint    udp_ep_type;

class    client
{
protected:
        boost::asio::io_service            dispatcher;
        boost::asio::ip::tcp::socket    sock;
        boost::system::error_code        err;
        std::string                        name;
public:
        client(std::string in) : sock(dispatcher), name(in) {}
        ~client() {}
        void    connect() {
                sock.connect(tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000)), err);
                if (err)
                        std::cout << err.message() << std::endl;
                else
                        std::cout << "connect : " << name << std::endl;
        }
        void    disconnect() {
                sock.close();
                std::cout << "disconnect : " << name << std::endl;
        }
};

void    vs_tcp_test1()
{
        debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
        debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
        debugg_flug_struct::getInstance().param_exist_flag() = false;

        l7vs::l7vsd                    vsd;
        boost::asio::io_service        dispatcher;
        l7vs::replication            rep;
        l7vs::virtualservice_element    elem1;
        //set element value
        elem1.udpmode              = false;
        elem1.tcp_accept_endpoint  = tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000));
        elem1.udp_recv_endpoint    = udp_ep_type(boost::asio::ip::address::from_string("10.144.169.20"), (50000));
        elem1.realserver_vector.clear();
        elem1.protocol_module_name = "PMtest1";
        elem1.schedule_module_name = "SMtest1";
        elem1.protocol_args.clear();
        elem1.protocol_args.push_back("testarg");
        elem1.protocol_args.push_back("testarg2");
        elem1.sorry_maxconnection  = LLONG_MAX;
        elem1.sorry_endpoint       = tcp_ep_type(boost::asio::ip::address::from_string("10.144.169.87"), (8080));
        elem1.sorry_flag           = INT_MAX;
        elem1.sorry_fwdmode        = l7vs::virtualservice_element::FWD_MASQ;
        elem1.qos_upstream         = 65535ULL;
        elem1.qos_downstream       = 32767ULL;

        l7vs::vs_tcp    *vs = new l7vs::vs_tcp(vsd, rep, elem1);

        BOOST_CHECK_EQUAL(&vsd, &(vs->get_vsd()));
        BOOST_CHECK_EQUAL(&rep, &(vs->get_rep()));
        l7vs::error_code    vs_err;
        BOOST_CHECK(NULL == vs->get_protocol_module());
        BOOST_CHECK(NULL == vs->get_schedule_module());

        // unit_test[1]  sorry_flag = INT_MAXでVSを作成した場合、sorry_flagが0に設定されること
        // unit_test[2]  sorry_maxconnection = LLONG_MAXでVSを作成した場合、sorry_maxconnectionが0LLに設定されること
        BOOST_MESSAGE("-------1");
        BOOST_CHECK(vs->get_element().sorry_maxconnection == 0LL);
        BOOST_MESSAGE(vs->get_element().sorry_maxconnection);
        BOOST_MESSAGE("-------2");
        BOOST_CHECK(vs->get_element().sorry_flag == 0);
        BOOST_MESSAGE(vs->get_element().sorry_flag);
        BOOST_CHECK(vs->get_element().sorry_fwdmode == l7vs::virtualservice_element::FWD_MASQ);
        BOOST_MESSAGE(vs->get_element().sorry_fwdmode);

        vs->initialize(vs_err);
        BOOST_CHECK(vs_err == false);
        BOOST_MESSAGE(vs_err.get_message());

        // unit_test[3]  初期化(不正なIPアドレス指定でエラーが返ること)
        {
                BOOST_MESSAGE("-------3");
                l7vs::virtualservice_element    elem2;
                //set element value
                elem2.udpmode                    = false;
                elem2.tcp_accept_endpoint        = tcp_ep_type(boost::asio::ip::address::from_string("10.144.169.20"), (60000));
                elem2.realserver_vector.clear();
                elem2.protocol_module_name    = "PMtest1";
                elem2.schedule_module_name    = "SMtest1";
                elem2.protocol_args.clear();
                l7vs::vs_tcp    *vs2 = new l7vs::vs_tcp(vsd, rep, elem2);
                vs2->initialize(vs_err);
                BOOST_CHECK(vs_err == true);
                BOOST_MESSAGE(vs_err.get_message());

                vs2->finalize(vs_err);
                delete vs2;
        }
        BOOST_MESSAGE("-------");

        // unit_test[4]  初期化(重複IPアドレス指定でエラーが返ること)
        {
                BOOST_MESSAGE("-------4");
                l7vs::virtualservice_element    elem2;
                //set element value
                elem2.udpmode                    = false;
                elem1.tcp_accept_endpoint        = tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000));
                elem2.realserver_vector.clear();
                elem2.protocol_module_name    = "PMtest1";
                elem2.schedule_module_name    = "SMtest1";
                elem2.protocol_args.clear();
                l7vs::vs_tcp    *vs2 = new l7vs::vs_tcp(vsd, rep, elem2);
                vs->initialize(vs_err);
                BOOST_CHECK(vs_err == true);
                BOOST_MESSAGE(vs_err.get_message());

                vs2->finalize(vs_err);
//    テスト用途なので破棄不要
//    delete vs2;
        }

        vs->finalize(vs_err);

//    同上
//    delete vs;
        BOOST_MESSAGE("-------");

}

void    vs_tcp_test2()
{
        l7vs::error_code    vs_err;
        boost::system::error_code    test_err;

        debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
        debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
        debugg_flug_struct::getInstance().param_exist_flag() = false;
        //まず、stubのレプリケーションエリア作成をする
        debugg_flug_struct::getInstance().create_rep_area();

        std::stringstream    tmp_tcp_ep;
        std::stringstream    tmp_udp_ep;
        std::stringstream    tmp_sorry_ep;

        l7vs::l7vsd                    vsd;
        boost::asio::io_service        dispatcher;
        l7vs::replication            rep;
        l7vs::virtualservice_element    elem1;
        //set element value
        elem1.udpmode             = false;
        elem1.tcp_accept_endpoint = tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000));
        elem1.udp_recv_endpoint   = udp_ep_type(boost::asio::ip::address::from_string("10.144.169.20"), (50000));
        elem1.realserver_vector.clear();
        elem1.protocol_module_name = "PMtest1";
        elem1.schedule_module_name = "SMtest1";
        elem1.protocol_args.clear();
        elem1.protocol_args.push_back("testarg");
        elem1.protocol_args.push_back("testarg2");
        elem1.sorry_maxconnection = 1234LL;
        elem1.sorry_endpoint      = tcp_ep_type(boost::asio::ip::address::from_string("10.144.169.87"), (8080));
        elem1.sorry_flag          = true;
        elem1.sorry_fwdmode       = l7vs::virtualservice_element::FWD_MASQ;
        elem1.qos_upstream        = 65535ULL;
        elem1.qos_downstream      = 32767ULL;

        l7vs::vs_tcp    *vs = new l7vs::vs_tcp(vsd, rep, elem1);

        vs->initialize(vs_err);

//    threadの開始は不要
//    usleep( 1000 );

//    boost::thread    vs_main( &l7vs::vs_tcp::run, vs );
//    usleep( 1000 );

        boost::system::error_code    b_err;

        // unit_test[5]  sorry_flagがINT_MAXの場合、接続時にsorry通知を行わないことを確認する
        BOOST_MESSAGE("-------5");
        vs->get_element().sorry_flag = INT_MAX;

        /*    boost::asio::io_service    client_dispatcher;
            boost::asio::ip::tcp::socket    sock( client_dispatcher );
            std::cout << "connect" << std::endl;
            sock.connect( tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) ) );
            usleep( 1000 );
        */
        BOOST_CHECK(vs->get_element().sorry_flag == INT_MAX);


        // unit_test[6]  replicationデータ作成のテスト
        BOOST_MESSAGE("-------6");
        vs->get_element().sorry_flag = true;
        vs->get_element().sorry_fwdmode = l7vs::virtualservice_element::FWD_TPROXY;
        vs->call_handle_replication_interrupt(test_err);
        l7vs::virtualservice_base::replication_header    *rep_head =
                reinterpret_cast<l7vs::virtualservice_base::replication_header *>(debugg_flug_struct::getInstance().get_rep_area());
        //data_numが0になってることを確認
        BOOST_CHECK(rep_head->data_num == 1);
        l7vs::virtualservice_base::replication_data    *rep_data =
                reinterpret_cast<l7vs::virtualservice_base::replication_data *>(++rep_head);
        //udpmode
        BOOST_CHECK(rep_data->udpmode == false);
        //tcp_endpoint
        tmp_tcp_ep << elem1.tcp_accept_endpoint;
        BOOST_CHECK(0 == strncmp(rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47));
        //udp_endpoint
        tmp_udp_ep << elem1.udp_recv_endpoint;
        BOOST_CHECK(0 == strncmp(rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47));
        //sorry_maxconnection
        BOOST_CHECK(rep_data->sorry_maxconnection == 1234LL);
        //sorry_endpoint
        tmp_sorry_ep << elem1.sorry_endpoint;
        BOOST_CHECK(0 == strncmp(rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47));
        //sorry_flag
        BOOST_CHECK(rep_data->sorry_flag == true);
        //sorry_fwdmode
        BOOST_CHECK(rep_data->sorry_fwdmode == 2);
        //qos_up
        BOOST_CHECK(rep_data->qos_up == elem1.qos_upstream);
        //qos_down
        BOOST_CHECK(rep_data->qos_down == elem1.qos_downstream);
        BOOST_MESSAGE("-------");

        // unit_test[7]  sorry_flagのクリア
        // unit_test[8]  sorry_maxconnectionのクリア
        l7vs::virtualservice_element    elem2;
        //set element value
        elem2.udpmode                    = false;
        elem2.tcp_accept_endpoint        = tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000));
        elem2.udp_recv_endpoint            = udp_ep_type(boost::asio::ip::address::from_string("10.144.169.20"), (50000));
        elem2.realserver_vector.clear();
        elem2.protocol_module_name        = "PMtest1";
        elem2.schedule_module_name        = "SMtest1";
        elem2.protocol_args.clear();
        elem2.protocol_args.push_back("testarg");
        elem2.protocol_args.push_back("testarg2");
        elem2.sorry_maxconnection        = LLONG_MAX;
        elem2.sorry_endpoint            = tcp_ep_type();
        elem2.sorry_flag                = INT_MAX;
        elem2.qos_upstream                = 0ULL;
        elem2.qos_downstream            = 0ULL;

        BOOST_MESSAGE("-------7");
        vs->edit_virtualservice(elem2, vs_err);
        BOOST_CHECK(vs_err == false);
        BOOST_MESSAGE(vs_err.get_message());

        BOOST_CHECK(vs->get_element().sorry_maxconnection == 0LL);
        BOOST_MESSAGE(vs->get_element().sorry_maxconnection);
        BOOST_MESSAGE("-------8");
        BOOST_CHECK(vs->get_element().sorry_flag == 0);
        BOOST_MESSAGE(vs->get_element().sorry_flag);
        BOOST_MESSAGE("-------");

        //設定をもとにもどす
        vs->edit_virtualservice(elem1, vs_err);

        // unit_test[9]  sorry_serverのクリア
        elem2.sorry_maxconnection        = 1234LL;
        elem2.sorry_endpoint            = tcp_ep_type(boost::asio::ip::address::from_string("255.255.255.255"), (0));
        elem2.sorry_flag                = true;
        BOOST_MESSAGE("-------9");
        vs->edit_virtualservice(elem2, vs_err);
        BOOST_CHECK(vs->get_element().sorry_endpoint == tcp_ep_type(boost::asio::ip::address::from_string("0.0.0.0"), (0)));
        BOOST_MESSAGE(vs->get_element().sorry_endpoint);
        BOOST_CHECK(vs->get_element().sorry_maxconnection == 0LL);
        BOOST_MESSAGE(vs->get_element().sorry_maxconnection);
        BOOST_CHECK(vs->get_element().sorry_flag == 0);
        BOOST_MESSAGE(vs->get_element().sorry_flag);
        BOOST_MESSAGE("-------");

        //設定をもとにもどす
        vs->edit_virtualservice(elem1, vs_err);

        // unit_test[10]  sorry_endpointを変更しない
        // unit_test[11]  sorry_flagを変更しない
        // unit_test[12]  sorry_maxconnectionを変更しない
        elem2.sorry_maxconnection        = 0LL;
        elem2.sorry_endpoint            = tcp_ep_type();
        elem2.sorry_flag                = 0;
        BOOST_MESSAGE("-------10");
        vs->edit_virtualservice(elem2, vs_err);
        BOOST_CHECK(vs->get_element().sorry_endpoint == elem1.sorry_endpoint);
        BOOST_MESSAGE(vs->get_element().sorry_endpoint);
        BOOST_MESSAGE("-------11");
        BOOST_CHECK(vs->get_element().sorry_maxconnection == elem1.sorry_maxconnection);
        BOOST_MESSAGE(vs->get_element().sorry_maxconnection);
        BOOST_MESSAGE("-------12");
        BOOST_CHECK(vs->get_element().sorry_flag == elem1.sorry_flag);
        BOOST_MESSAGE(vs->get_element().sorry_flag);
        BOOST_MESSAGE("-------");

        //設定をもとにもどす
        vs->edit_virtualservice(elem1, vs_err);

        // unit_test[13]  普通に値が変わるか
        elem2.sorry_maxconnection        = 2468LL;
        elem2.sorry_endpoint            = tcp_ep_type(boost::asio::ip::address::from_string("192.168.0.101"), (43210));
        elem2.qos_upstream                = 32767ULL;
        elem2.qos_downstream            = 65535ULL;
        BOOST_MESSAGE("-------13");
        vs->edit_virtualservice(elem2, vs_err);
        BOOST_CHECK(vs->get_element().sorry_endpoint == elem2.sorry_endpoint);
        BOOST_MESSAGE(vs->get_element().sorry_endpoint);
        BOOST_CHECK(vs->get_element().sorry_maxconnection == elem2.sorry_maxconnection);
        BOOST_MESSAGE(vs->get_element().sorry_maxconnection);
        BOOST_CHECK(vs->get_element().qos_upstream == elem2.qos_upstream);
        BOOST_MESSAGE(vs->get_element().qos_upstream);
        BOOST_CHECK(vs->get_element().qos_downstream == elem2.qos_downstream);
        BOOST_MESSAGE(vs->get_element().qos_downstream);
        BOOST_MESSAGE("-------");


//     BOOST_MESSAGE( "-------14" );
//    sock.close();

        vs->stop();
        usleep(1000);

        vs->finalize(vs_err);

        delete vs;

        //テストが終わったらStubのレプリケーションエリアを削除
        debugg_flug_struct::getInstance().create_rep_area();
}


/*
void    vs_tcp_test3(){
    l7vs::error_code    vs_err;
    boost::system::error_code    test_err;

    debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
    debugg_flug_struct::getInstance().param_exist_flag() = false;

    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;
    l7vs::virtualservice_element    elem1;
    //set element value
    elem1.udpmode                    = false;
    elem1.tcp_accept_endpoint        = tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
    elem1.udp_recv_endpoint            = udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
    elem1.realserver_vector.clear();
    elem1.protocol_module_name        = "PMtest1";
    elem1.schedule_module_name        = "SMtest1";
    elem1.protocol_args.clear();
    elem1.protocol_args.push_back( "testarg" );
    elem1.protocol_args.push_back( "testarg2" );
    elem1.sorry_maxconnection        = 2LL;
    elem1.sorry_endpoint            = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
    elem1.sorry_flag                = false;
    elem1.qos_upstream                = 0ULL;
    elem1.qos_downstream            = 0ULL;

    l7vs::vs_tcp*    vs = new l7vs::vs_tcp( vsd, rep, elem1 );
    vs->initialize( vs_err );
    usleep( 1000 );
    boost::thread    thd1( boost::bind( vs_tcp_test3_mainfunc, vs ) );
    usleep( 1000 );

    // unit_test[]  RSに繋がるSession数がsorry_maxconnectionを越えたときに、Sorry状態ONでSessionをスタートすることを確認する
    BOOST_MESSAGE( "-------" );
    debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
    debugg_flug_struct::getInstance().session_loop_flag() = false;
    client    cl1("1"),cl2("2"),cl3("3"),cl4("4"),cl5("5");
    cl1.connect();
    cl2.connect();
    cl3.connect();
    cl4.connect();
    cl5.connect();

    usleep( 200 );
    cl1.disconnect();
    cl2.disconnect();
    cl3.disconnect();
    cl4.disconnect();
    cl5.disconnect();
    usleep( 1000 );
    BOOST_MESSAGE( "-------" );

    // unit_test[]  RSに繋がるSession数がsorry_maxconenctionを下回ったとき(Sorryへ接続しているSession数はカウントに入れない)、Sorry状態OFFでSessionをスタートすることを確認する

    // unit_test[]  session_pool_sizeを上回った接続数でSessionが増えても、切断されれば元の数に戻ることを確認する


    vs->finalize( vs_err );
    delete vs;
}
*/

test_suite    *init_unit_test_suite(int argc, char *argv[])
{

        // create unit test suite
        test_suite *ts = BOOST_TEST_SUITE("virtualservice_tcp_test(UT for IT-bug)");

        // add test case to test suite
        ts->add(BOOST_TEST_CASE(&vs_tcp_test1));
        ts->add(BOOST_TEST_CASE(&vs_tcp_test2));
//    ts->add( BOOST_TEST_CASE( &vs_tcp_test3 ) );

        framework::master_test_suite().add(ts);

        return 0;
}
