
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "stub.h"
#include "virtualservice.h"

#include "protocol_module_test1.h"
#include "schedule_module_test1.h"

using namespace boost::unit_test;


static bool new_flag = false;
void * operator new ( size_t size ){
    if( new_flag ){
        throw std::bad_alloc();
    }
    else{
        void * p = malloc( size );
        return p;
    }
}
void operator delete ( void * p){
    free( p );
}

void virtual_service_test(void){
    l7vs::l7vsd                    vsd;
    boost::asio::io_service        dispatcher;
    l7vs::replication            rep;

    // common
    int basecount = virtual_service_test_struct::getInstance().basefunctioncallcount;
    int udpcount = virtual_service_test_struct::getInstance().udpfunctioncallcount;
    int    tcpcount = virtual_service_test_struct::getInstance().tcpfunctioncallcount;
    unsigned long long datasize;

    // UDP
    l7vs::virtualservice_element    elementudp;
    elementudp.udpmode = true;
    l7vs::virtual_service vsudp( vsd, rep, elementudp );
    l7vs::error_code errudp;
    boost::asio::ip::udp::endpoint udpendpoint;
    boost::thread::id thread_idudp;

    // TCP
    l7vs::virtualservice_element    elementtcp;
    elementtcp.udpmode = false;
    l7vs::virtual_service vstcp( vsd, rep, elementtcp );
    l7vs::error_code errtcp;
    boost::asio::ip::tcp::endpoint tcpendpoint;
    boost::thread::id thread_idtcp;

/// UDP TEST ////////////////////////////////////////////////////////////////////////////
    // unit_test[1] コンストラクタの呼び出し先のチェック(UDP)（正常系）
    {
    l7vs::virtualservice_element    elementudp;
    elementudp.udpmode = true;
    l7vs::virtual_service vsudp( vsd, rep, elementudp );
    }
    // unit_test[2] コンストラクタの呼び出し先のチェック(UDP)（異常系、newに失敗）
    {
    bool badallocflag = false;
    new_flag = true;
    l7vs::virtualservice_element    elementudp;
    elementudp.udpmode = true;
    try{
        l7vs::virtual_service vsudp( vsd, rep, elementudp );
    }
    catch(std::bad_alloc&){
        badallocflag = true;
    }
    new_flag = false;
    BOOST_CHECK_EQUAL( badallocflag, true );
    }

    // unit_test[3] initializeの呼び出し先のチェック(UDP)
    vsudp.initialize( errudp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[4] finalizeの呼び出し先のチェック(UDP)
    vsudp.finalize( errudp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[5] operator==の呼び出し先のチェック(UDP)
    l7vs::virtualservice_udp* vsudp1 = new l7vs::virtualservice_udp( vsd, rep, elementudp );
    l7vs::virtualservice_udp* vsudp2 = new l7vs::virtualservice_udp( vsd, rep, elementudp );
    BOOST_CHECK_EQUAL( ( *vsudp1 == *vsudp2 ), true );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[6] operator!=の呼び出し先のチェック(UDP)
    BOOST_CHECK_EQUAL( ( *vsudp1 != *vsudp2 ), true );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    delete vsudp1;
    delete vsudp2;

    // unit_test[7] set_virtualserviceの呼び出し先のチェック(UDP)
    vsudp.set_virtualservice( elementudp, errudp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[8] edit_virtualserviceの呼び出し先のチェック(UDP)
    vsudp.edit_virtualservice( elementudp, errudp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[9] add_realserverの呼び出し先のチェック(UDP)
    vsudp.add_realserver( elementudp, errudp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[10] edit_realserverの呼び出し先のチェック(UDP)
    vsudp.edit_realserver( elementudp, errudp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[11] del_realserverの呼び出し先のチェック(UDP)
    vsudp.del_realserver( elementudp, errudp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[12] get_elementの呼び出し先のチェック(UDP)
    vsudp.get_element();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[13] runの呼び出し先のチェック(UDP)
    vsudp.run();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[14] stopの呼び出し先のチェック(UDP)
    vsudp.stop();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[15] release_sessionの呼び出し先のチェック(UDP)
    vsudp.release_session( thread_idudp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, ++udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[16] get_qos_upstreamの呼び出し先のチェック(UDP)
    vsudp.get_qos_upstream();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[17] get_qos_downstreamの呼び出し先のチェック(UDP)
    vsudp.get_qos_downstream();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[18] get_throughput_upstreamの呼び出し先のチェック(UDP)
    vsudp.get_throughput_upstream();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[19] get_throughput_downstreamの呼び出し先のチェック(UDP)
    vsudp.get_throughput_downstream();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[20] get_up_recv_sizeの呼び出し先のチェック(UDP)
    vsudp.get_up_recv_size();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[21] get_up_send_sizeの呼び出し先のチェック(UDP)
    vsudp.get_up_send_size();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[22] get_down_recv_sizeの呼び出し先のチェック(UDP)
    vsudp.get_down_recv_size();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[23] get_down_send_sizeの呼び出し先のチェック(UDP)
    vsudp.get_down_send_size();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[24] update_up_recv_sizeの呼び出し先のチェック(UDP)
    vsudp.update_up_recv_size( datasize );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[25] update_up_send_sizeの呼び出し先のチェック(UDP)
    vsudp.update_up_send_size( datasize );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[26] update_down_recv_sizeの呼び出し先のチェック(UDP)
    vsudp.update_down_recv_size( datasize );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[27] update_down_send_sizeの呼び出し先のチェック(UDP)
    vsudp.update_down_send_size( datasize );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[28] get_protocol_moduleの呼び出し先のチェック(UDP)
    vsudp.get_protocol_module();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[29] get_schedule_moduleの呼び出し先のチェック(UDP)
    vsudp.get_schedule_module();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );


/// TCP TEST ////////////////////////////////////////////////////////////////////////////
    // unit_test[30] コンストラクタの呼び出し先のチェック(TCP)（正常系）
    {
    l7vs::virtualservice_element    elementtcp;
    elementtcp.udpmode = false;
    l7vs::virtual_service vstcp( vsd, rep, elementtcp );
    }
    // unit_test[31] コンストラクタの呼び出し先のチェック(TCP)（異常系、newに失敗）
    {
    bool badallocflag = false;
    new_flag = true;
    l7vs::virtualservice_element    elementtcp;
    elementtcp.udpmode = false;
    try{
        l7vs::virtual_service vstcp( vsd, rep, elementtcp );
    }
    catch(std::bad_alloc&){
        badallocflag = true;
    }
    new_flag = false;
    BOOST_CHECK_EQUAL( badallocflag, true );
    }

    // unit_test[32] initializeの呼び出し先のチェック(TCP)
    vstcp.initialize( errtcp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[33] finalizeの呼び出し先のチェック(TCP)
    vstcp.finalize( errtcp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[34] operator==の呼び出し先のチェック(TCP)
    l7vs::virtualservice_tcp* vstcp1 = new l7vs::virtualservice_tcp( vsd, rep, elementtcp );
    l7vs::virtualservice_tcp* vstcp2 = new l7vs::virtualservice_tcp( vsd, rep, elementtcp );
    BOOST_CHECK_EQUAL( ( *vstcp1 == *vstcp2 ), true );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[35] operator!=の呼び出し先のチェック(TCP)
    BOOST_CHECK_EQUAL( ( *vstcp1 != *vstcp2 ), true );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    delete vstcp1;
    delete vstcp2;

    // unit_test[36] set_virtualserviceの呼び出し先のチェック(TCP)
    vstcp.set_virtualservice( elementtcp, errtcp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[37] edit_virtualserviceの呼び出し先のチェック(TCP)
    vstcp.edit_virtualservice( elementtcp, errtcp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[38] add_realserverの呼び出し先のチェック(TCP)
    vstcp.add_realserver( elementtcp, errtcp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[39] edit_realserverの呼び出し先のチェック(TCP)
    vstcp.edit_realserver( elementtcp, errtcp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[40] del_realserverの呼び出し先のチェック(TCP)
    vstcp.del_realserver( elementtcp, errtcp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[41] get_elementの呼び出し先のチェック(TCP)
    vstcp.get_element();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[42] runの呼び出し先のチェック(TCP)
    vstcp.run();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[43] stopの呼び出し先のチェック(TCP)
    vstcp.stop();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[44] connection_activeの呼び出し先のチェック(TCP)
    vstcp.connection_active( tcpendpoint );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[45] connection_inactiveの呼び出し先のチェック(TCP)
    vstcp.connection_inactive( tcpendpoint );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[46] release_sessionの呼び出し先のチェック(TCP)
    vstcp.release_session( thread_idtcp );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, ++tcpcount );

    // unit_test[47] get_qos_upstreamの呼び出し先のチェック(TCP)
    vstcp.get_qos_upstream();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[48] get_qos_downstreamの呼び出し先のチェック(TCP)
    vstcp.get_qos_downstream();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[49] get_throughput_upstreamの呼び出し先のチェック(TCP)
    vstcp.get_throughput_upstream();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[50] get_throughput_downstreamの呼び出し先のチェック(TCP)
    vstcp.get_throughput_downstream();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[51] get_up_recv_sizeの呼び出し先のチェック(TCP)
    vstcp.get_up_recv_size();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[52] get_up_send_sizeの呼び出し先のチェック(TCP)
    vstcp.get_up_send_size();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[53] get_down_recv_sizeの呼び出し先のチェック(TCP)
    vstcp.get_down_recv_size();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[54] get_down_send_sizeの呼び出し先のチェック(TCP)
    vstcp.get_down_send_size();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[55] update_up_recv_sizeの呼び出し先のチェック(TCP)
    vstcp.update_up_recv_size( datasize );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[56] update_up_send_sizeの呼び出し先のチェック(TCP)
    vstcp.update_up_send_size( datasize );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[57] update_down_recv_sizeの呼び出し先のチェック(TCP)
    vstcp.update_down_recv_size( datasize );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[58] update_down_send_sizeの呼び出し先のチェック(TCP)
    vstcp.update_down_send_size( datasize );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[59] get_protocol_moduleの呼び出し先のチェック(TCP)
    vstcp.get_protocol_module();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );

    // unit_test[60] get_schedule_moduleの呼び出し先のチェック(TCP)
    vstcp.get_schedule_module();
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().basefunctioncallcount, ++basecount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().udpfunctioncallcount, udpcount );
    BOOST_CHECK_EQUAL( virtual_service_test_struct::getInstance().tcpfunctioncallcount, tcpcount );


}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    // create unit test suite
    test_suite* ts = BOOST_TEST_SUITE( "virtual_service_test" );

    // add test case to test suite
    ts->add( BOOST_TEST_CASE( &virtual_service_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}
