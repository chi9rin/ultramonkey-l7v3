
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/thread.hpp>

#include "stub.h"
#include "virtualservice.h"
#include "vs_tcp.h"

#include "protocol_module_test1.h"
#include "schedule_module_test1.h"

using namespace boost::unit_test;

typedef    boost::asio::ip::tcp::endpoint    tcp_ep_type;
typedef    boost::asio::ip::udp::endpoint    udp_ep_type;

//Acceptテスト用Client
void    client()
{
        boost::system::error_code    b_err;

        boost::asio::io_service    dispatcher;
        boost::asio::ip::tcp::socket    sock(dispatcher);
        sock.connect(tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000)), b_err);
        std::cout << "connect" << std::endl;

        usleep(10000);

        sock.close(b_err);
}

class    vs_access
{
protected:
        boost::asio::io_service        dispatcher;

        l7vs::vs_tcp                *vs;
        l7vs::l7vsd                    vsd;
        l7vs::replication            *rep;
        l7vs::error_code            vs_err;

        boost::condition            cond;
public:
        vs_access() : vs(NULL), rep(NULL) {}
        ~vs_access() {}

        void    initialize(l7vs::virtualservice_element &in_elem) {
                rep    = new l7vs::replication();
                vs    = new l7vs::vs_tcp(vsd, *rep, in_elem);
        }

        void    finalize() {
                vs->finalize(vs_err);
                usleep(10);
                if (NULL != vs) {
                        delete vs;
                        vs = NULL;
                }
                if (NULL != rep) {
                        delete rep;
                        rep = NULL;
                }
        }

        void    start() {
                cond.notify_all();
        }

        void    step() {
                cond.notify_one();
        }

        l7vs::vs_tcp    *get_vs() {
                return vs;
        }

        void    client() {
                boost::system::error_code    b_err;

                boost::asio::io_service    client_io;
                boost::asio::ip::tcp::socket    sock(client_io);
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                sock.connect(tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000)), b_err);
                std::cout << "connect" << std::endl;

                usleep(10000);

                sock.close(b_err);
        }

        void    rs_list_lock() {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->rs_list_lock();
//        std::cout << "" << std::endl;
        }
        void        rs_list_unlock() {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->rs_list_unlock();

        }

        void        get_element(l7vs::virtualservice_element *elem) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *(elem) = vs->get_element();

        }

        void        get_qos_upstream(unsigned long long *val) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *val = vs->get_qos_upstream();

        }
        void        get_qos_downstream(unsigned long long *val) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *val = vs->get_qos_downstream();

        }
        void        get_throughput_upstream(unsigned long long *val) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *val = vs->get_throughput_upstream();

        }
        void        get_throughput_downstream(unsigned long long *val) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *val = vs->get_throughput_downstream();

        }
        void        get_up_recv_size(unsigned long long *val) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *val = vs->get_up_recv_size();

        }
        void        get_up_send_size(unsigned long long *val) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *val = vs->get_up_send_size();

        }
        void        get_down_recv_size(unsigned long long *val) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *val = vs->get_down_recv_size();

        }
        void        get_down_send_size(unsigned long long *val) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *val = vs->get_down_send_size();

        }

        void        update_up_recv_size(unsigned long long size) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->update_up_recv_size(size);

        }
        void        update_up_send_size(unsigned long long size) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->update_up_send_size(size);

        }
        void        update_down_recv_size(unsigned long long size) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->update_down_recv_size(size);

        }
        void        update_down_send_size(unsigned long long size) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->update_down_send_size(size);

        }

        void        get_protocol_module(l7vs::protocol_module_base *in_pm) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                in_pm = vs->get_protocol_module();

        }
        void        get_schedule_module(l7vs::schedule_module_base *in_sm) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                in_sm = vs->get_schedule_module();

        }

        void        vs_initialize() {
                l7vs::error_code    vs_err;
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->initialize(vs_err);

        }
        void        vs_finalize() {
                l7vs::error_code    vs_err;
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->finalize(vs_err);

                BOOST_MESSAGE("finalize." << boost::this_thread::get_id());
        }

        void        vs_eq(const l7vs::virtualservice_base *in_vs, bool *ret) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *ret = vs->operator==(*in_vs);

        }
        void        vs_ne(const l7vs::virtualservice_base *in_vs, bool *ret) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                *ret = vs->operator!=(*in_vs);

        }

        void        set_virtualservice(const l7vs::virtualservice_element &elem) {
                l7vs::error_code    vs_err;
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->set_virtualservice(elem, vs_err);

        }
        void        edit_virtualservice(const l7vs::virtualservice_element &elem) {
                l7vs::error_code    vs_err;
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->edit_virtualservice(elem, vs_err);

        }

        void        add_realserver(const l7vs::virtualservice_element &elem) {
                l7vs::error_code    vs_err;
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->add_realserver(elem, vs_err);

        }
        void        edit_realserver(const l7vs::virtualservice_element &elem) {
                l7vs::error_code    vs_err;
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->edit_realserver(elem, vs_err);

        }
        void        del_realserver(const l7vs::virtualservice_element &elem) {
                l7vs::error_code    vs_err;
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->del_realserver(elem, vs_err);

        }

        void        run() {
                vs->run();
        }
        void        stop() {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->stop();

        }

        void        connection_active(const tcp_ep_type &ep) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->connection_active(ep);

        }
        void        connection_inactive(const tcp_ep_type &ep) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->connection_inactive(ep);

        }
        void        release_session(const l7vs::tcp_session *session_ptr) {
                boost::mutex    mtx;
                boost::mutex::scoped_lock    lk(mtx);
                cond.wait(lk);

                vs->release_session(session_ptr);
        }
};

void    virtualservice_tcp_test2_1()
{
        //replicationエリアを作成しておく
        debugg_flug_struct::getInstance().create_rep_area();

        boost::asio::io_service        dispatcher;

        l7vs::l7vsd                    vsd;
        l7vs::replication            rep();

        debugg_flug_struct::getInstance().pmcontrol_err_flag()    = false;
        debugg_flug_struct::getInstance().smcontrol_err_flag()    = false;
        debugg_flug_struct::getInstance().param_exist_flag() = false;

        l7vs::error_code            vs_err;
        boost::system::error_code    test_err;

        std::stringstream    tmp_tcp_ep;
        std::stringstream    tmp_udp_ep;
        std::stringstream    tmp_sorry_ep;

        l7vs::virtualservice_element    elem1;

        //set element value
        elem1.udpmode                    = false;
        elem1.tcp_accept_endpoint        =
                tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000));
        elem1.udp_recv_endpoint            = udp_ep_type(boost::asio::ip::address::from_string("10.144.169.20"), (50000));
        elem1.realserver_vector.clear();
        elem1.protocol_module_name        = "PMtest1";
        elem1.schedule_module_name        = "SMtest1";
        elem1.protocol_args.clear();
        elem1.sorry_maxconnection        = 1234LL;
        elem1.sorry_endpoint            = tcp_ep_type();
        elem1.sorry_flag                = false;
        elem1.sorry_fwdmode             = l7vs::virtualservice_element::FWD_MASQ;
        elem1.qos_upstream                = 65535ULL;
        elem1.qos_downstream            = 32767ULL;

        //vs作成
        vs_access    vst;
        vst.initialize(elem1);

        //rs追加
        // unit_test[27]  add_rsとrs_list_lock
        std::cout << "1" << std::endl;
        //RS追加用のelement準備
        l7vs::virtualservice_element    elem2;
        //set element value
        elem2.udpmode                    = false;
        elem2.tcp_accept_endpoint        =
                tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000));
        elem2.udp_recv_endpoint            = udp_ep_type(boost::asio::ip::address::from_string("10.144.169.20"), (50000));
        elem2.realserver_vector.clear();
        elem2.protocol_module_name        = "PMtest1";
        elem2.schedule_module_name        = "SMtest1";
        elem2.protocol_args.clear();
        elem2.sorry_maxconnection        = 1234LL;
        elem2.sorry_endpoint            = tcp_ep_type();
        elem2.sorry_flag                = false;
        elem2.sorry_fwdmode             = l7vs::virtualservice_element::FWD_MASQ;
        elem2.qos_upstream                = 65535ULL;
        elem2.qos_downstream            = 32767ULL;
        for (size_t i = 0; i < 2; ++i) {
                l7vs::realserver_element    rs_elem;
                rs_elem.tcp_endpoint = tcp_ep_type(boost::asio::ip::address::from_string("192.168.10.10"), (i + 8080));
                rs_elem.weight    = 10 + i;
                elem2.realserver_vector.push_back(rs_elem);
        }
        {
                boost::thread    thread1(&vs_access::rs_list_lock, &vst);
                boost::thread    thread2(&vs_access::add_realserver, &vst, elem2);

                usleep(100000);
                vst.start();
                usleep(1000);
                BOOST_CHECK(vst.get_vs()->get_ref_count() == 1);
                //ref_countが1のままだとadd_realserverができないのでunlockする
                vst.get_vs()->rs_list_unlock();
                usleep(100000);
                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 2);
        BOOST_CHECK(vst.get_vs()->get_ref_count() == 0);

        //一旦削除
        vst.get_vs()->del_realserver(elem2, vs_err);

        // unit_test[28]  add_rsとrs_list_unlock(ref_countが1でスタートした場合)
        std::cout << "2" << std::endl;
        {
                //あらかじめlockしておいてref_countを1にしておく
                vst.get_vs()->rs_list_lock();
                BOOST_CHECK(vst.get_vs()->get_ref_count() == 1);

                boost::thread    thread1(&vs_access::add_realserver, &vst, elem2);
                boost::thread    thread2(&vs_access::rs_list_unlock, &vst);

                usleep(100000);
                vst.start();
                usleep(1000);
                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 2);
        BOOST_CHECK(vst.get_vs()->get_ref_count() == 0);

        // unit_test[29]  add_rsとconnection_active
        std::cout << "3" << std::endl;
        //RS追加用のelement準備
        l7vs::virtualservice_element    elem3;
        //set element value
        elem3.udpmode                    = false;
        elem3.tcp_accept_endpoint        =
                tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000));
        elem3.udp_recv_endpoint            = udp_ep_type(boost::asio::ip::address::from_string("10.144.169.20"), (50000));
        elem3.realserver_vector.clear();
        elem3.protocol_module_name        = "PMtest1";
        elem3.schedule_module_name        = "SMtest1";
        elem3.protocol_args.clear();
        elem3.sorry_maxconnection        = 1234LL;
        elem3.sorry_endpoint            = tcp_ep_type();
        elem3.sorry_flag                = false;
        elem3.sorry_fwdmode             = l7vs::virtualservice_element::FWD_MASQ;
        elem3.qos_upstream                = 65535ULL;
        elem3.qos_downstream            = 32767ULL;
        for (size_t i = 0; i < 2; ++i) {
                l7vs::realserver_element    rs_elem;
                rs_elem.tcp_endpoint = tcp_ep_type(boost::asio::ip::address::from_string("192.168.10.10"), (i + 8090));
                rs_elem.weight    = 5 + i;
                elem3.realserver_vector.push_back(rs_elem);
        }
        {
                boost::thread    thread1(&vs_access::add_realserver, &vst, elem3);
                boost::thread    thread2(&vs_access::connection_active, &vst, elem2.realserver_vector[0].tcp_endpoint);

                usleep(100000);
                vst.start();
                usleep(1000);

                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 4);
        for (std::list<l7vs::realserver>::iterator itr = vst.get_vs()->get_rs_list().begin();
             itr != vst.get_vs()->get_rs_list().end(); ++itr) {
                if (itr->tcp_endpoint == elem2.realserver_vector[0].tcp_endpoint) {
                        std::cout << "active_conn check" << std::endl;
                        BOOST_CHECK(itr->get_active() == 1);
                        BOOST_CHECK(itr->get_inact() == 0);
                        break;
                }
        }

        //一旦削除
        vst.get_vs()->del_realserver(elem3, vs_err);

        // unit_test[30]  add_rsとconnection_inactive
        std::cout << "4" << std::endl;
        {
                boost::thread    thread1(&vs_access::add_realserver, &vst, elem3);
                boost::thread    thread2(&vs_access::connection_inactive, &vst, elem2.realserver_vector[0].tcp_endpoint);

                usleep(100000);
                vst.start();
                usleep(1000);

                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 4);
        for (std::list<l7vs::realserver>::iterator itr = vst.get_vs()->get_rs_list().begin();
             itr != vst.get_vs()->get_rs_list().end(); ++itr) {
                if (itr->tcp_endpoint == elem2.realserver_vector[0].tcp_endpoint) {
                        std::cout << "inactive_conn check" << std::endl;
                        BOOST_CHECK(itr->get_active() == 0);
                        BOOST_CHECK(itr->get_inact() == 1);
                        break;
                }
        }

        //rs変更
        // unit_test[31]  edit_rsとrs_list_lock
        std::cout << "5" << std::endl;
        //RS編集用のelement準備
        l7vs::virtualservice_element    elem4;
        //set element value
        elem4.udpmode                    = false;
        elem4.tcp_accept_endpoint        =
                tcp_ep_type(boost::asio::ip::address_v4::loopback(), (60000));
        elem4.udp_recv_endpoint            = udp_ep_type(boost::asio::ip::address::from_string("10.144.169.20"), (50000));
        elem4.realserver_vector.clear();
        elem4.protocol_module_name        = "PMtest1";
        elem4.schedule_module_name        = "SMtest1";
        elem4.protocol_args.clear();
        elem4.sorry_maxconnection        = 1234LL;
        elem4.sorry_endpoint            = tcp_ep_type();
        elem4.sorry_flag                = false;
        elem4.sorry_fwdmode             = l7vs::virtualservice_element::FWD_MASQ;
        elem4.qos_upstream                = 65535ULL;
        elem4.qos_downstream            = 32767ULL;
        for (size_t i = 0; i < 2; ++i) {
                l7vs::realserver_element    rs_elem;
                rs_elem.tcp_endpoint = tcp_ep_type(boost::asio::ip::address::from_string("192.168.10.10"), (i + 8080));
                rs_elem.weight    = 5 + i;
                elem4.realserver_vector.push_back(rs_elem);
        }
        {
                boost::thread    thread1(&vs_access::edit_realserver, &vst, elem4);
                boost::thread    thread2(&vs_access::rs_list_lock, &vst);

                usleep(100000);
                vst.start();
                usleep(1000);
                BOOST_CHECK(vst.get_vs()->get_ref_count() == 1);
                //ref_countが1のままだとedit_realserverができないのでunlockする
                vst.get_vs()->rs_list_unlock();
                BOOST_CHECK(vst.get_vs()->get_ref_count() == 0);
                usleep(100000);
                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 4);

        // unit_test[32]  edit_rsとrs_list_unlock
        std::cout << "6" << std::endl;
        {
                //あらかじめlockしておいてref_countを1にしておく
                vst.get_vs()->rs_list_lock();
                BOOST_CHECK(vst.get_vs()->get_ref_count() == 1);

                boost::thread    thread1(&vs_access::edit_realserver, &vst, elem2);
                boost::thread    thread2(&vs_access::rs_list_unlock, &vst);
                usleep(100000);
                vst.start();
                usleep(1000);
                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 4);
        BOOST_CHECK(vst.get_vs()->get_ref_count() == 0);

        // unit_test[33]  edit_rsとconnection_active
        std::cout << "7" << std::endl;
        elem4.realserver_vector.clear();
        for (size_t i = 0; i < 2; ++i) {
                l7vs::realserver_element    rs_elem;
                rs_elem.tcp_endpoint = tcp_ep_type(boost::asio::ip::address::from_string("192.168.10.10"), (i + 8080));
                rs_elem.weight    = 100 + i;
                elem4.realserver_vector.push_back(rs_elem);
        }
        {
                boost::thread    thread1(&vs_access::edit_realserver, &vst, elem4);
                boost::thread    thread2(&vs_access::connection_active, &vst, elem2.realserver_vector[0].tcp_endpoint);

                usleep(100000);
                vst.start();
                usleep(1000);

                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 4);
        for (std::list<l7vs::realserver>::iterator itr = vst.get_vs()->get_rs_list().begin();
             itr != vst.get_vs()->get_rs_list().end(); ++itr) {
                if (itr->tcp_endpoint == elem2.realserver_vector[0].tcp_endpoint) {
                        std::cout << "active_conn check" << std::endl;
                        BOOST_CHECK(itr->get_active() == 1);
                        BOOST_CHECK(itr->get_inact() == 1);
                        break;
                }
        }
        // unit_test[34]  edit_rsとconnection_inactive
        std::cout << "8" << std::endl;
        {
                boost::thread    thread1(&vs_access::edit_realserver, &vst, elem4);
                boost::thread    thread2(&vs_access::connection_inactive, &vst, elem2.realserver_vector[0].tcp_endpoint);

                usleep(100000);
                vst.start();
                usleep(1000);

                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 4);
        for (std::list<l7vs::realserver>::iterator itr = vst.get_vs()->get_rs_list().begin();
             itr != vst.get_vs()->get_rs_list().end(); ++itr) {
                if (itr->tcp_endpoint == elem2.realserver_vector[0].tcp_endpoint) {
                        std::cout << "inactive_conn check" << std::endl;
                        BOOST_CHECK(itr->get_active() == 0);
                        BOOST_CHECK(itr->get_inact() == 2);
                        break;
                }
        }

        //rs削除
        // unit_test[35]  del_rsとrs_list_lock
        std::cout << "9" << std::endl;
        {
                boost::thread    thread1(&vs_access::del_realserver, &vst, elem4);
                boost::thread    thread2(&vs_access::rs_list_lock, &vst);

                usleep(100000);
                vst.start();
                usleep(1000);
                BOOST_CHECK(vst.get_vs()->get_ref_count() == 1);
                //ref_countが1のままだとedit_realserverができないのでunlockする
                vst.get_vs()->rs_list_unlock();
                BOOST_CHECK(vst.get_vs()->get_ref_count() == 0);
                usleep(100000);
                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 2);

        //再度追加
        vst.get_vs()->add_realserver(elem4, vs_err);

        // unit_test[36]  del_rsとrs_list_unlock
        std::cout << "10" << std::endl;
        {
                //あらかじめlockしておいてref_countを1にしておく
                vst.get_vs()->rs_list_lock();
                BOOST_CHECK(vst.get_vs()->get_ref_count() == 1);

                boost::thread    thread1(&vs_access::del_realserver, &vst, elem4);
                boost::thread    thread2(&vs_access::rs_list_unlock, &vst);
                usleep(100000);
                vst.start();
                usleep(1000);
                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 2);
        BOOST_CHECK(vst.get_vs()->get_ref_count() == 0);

        //再度追加
        vst.get_vs()->add_realserver(elem4, vs_err);

        //del_rsとconnection_active
        {
                boost::thread    thread1(&vs_access::del_realserver, &vst, elem4);
                boost::thread    thread2(&vs_access::connection_active, &vst, elem3.realserver_vector[0].tcp_endpoint);

                usleep(100000);
                vst.start();
                usleep(1000);

                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 2);
        for (std::list<l7vs::realserver>::iterator itr = vst.get_vs()->get_rs_list().begin();
             itr != vst.get_vs()->get_rs_list().end(); ++itr) {
                if (itr->tcp_endpoint == elem2.realserver_vector[0].tcp_endpoint) {
                        std::cout << "active_conn check" << std::endl;
                        BOOST_CHECK(itr->get_active() == 1);
                        BOOST_CHECK(itr->get_inact() == 0);
                        break;
                }
        }

        //再度追加
        vst.get_vs()->add_realserver(elem4, vs_err);
        //del_rsとconnection_inactive
        {
                boost::thread    thread1(&vs_access::del_realserver, &vst, elem4);
                boost::thread    thread2(&vs_access::connection_inactive, &vst, elem3.realserver_vector[0].tcp_endpoint);

                usleep(100000);
                vst.start();
                usleep(1000);

                thread1.join();
                thread2.join();
        }
        BOOST_CHECK(vst.get_vs()->get_rs_list().size() == 2);
        for (std::list<l7vs::realserver>::iterator itr = vst.get_vs()->get_rs_list().begin();
             itr != vst.get_vs()->get_rs_list().end(); ++itr) {
                if (itr->tcp_endpoint == elem2.realserver_vector[0].tcp_endpoint) {
                        std::cout << "inactive_conn check" << std::endl;
                        BOOST_CHECK(itr->get_active() == 0);
                        BOOST_CHECK(itr->get_inact() == 1);
                        break;
                }
        }

        vst.finalize();
        //replicationエリアを開放する
        debugg_flug_struct::getInstance().destroy_rep_area();
}


test_suite    *init_unit_test_suite(int argc, char *argv[])
{

        // create unit test suite
        test_suite *ts = BOOST_TEST_SUITE("virtualservice_base_test");

        // add test case to test suite
        ts->add(BOOST_TEST_CASE(&virtualservice_tcp_test2_1));

        framework::master_test_suite().add(ts);

        return 0;
}
