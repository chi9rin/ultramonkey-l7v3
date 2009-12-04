#define    TEST_CASE

#include <dlfcn.h>

#include <boost/test/included/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "error_code.h"
#include "logger_wrapper.h"

//#include "logger_enum.h"
//#include "logger_stub.h"
#include "logger.h"

//#include "parameter_enum.h"
//#include "parameter_stub.h"
#include "parameter.h"

#include "command_receiver_stub.h"
#include "virtualservice_stub.h"
//#include "virtualservice_element_stub.h"
#include "realserver_stub.h"
//#include "realserver_element_stub.h"
#include "replication_stub.h"
#include "snmpbridge_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"

#include "../../src/l7vsd.cpp"

using namespace boost::unit_test_framework;


// test class
class    l7vsd_test    :public    l7vs::l7vsd {
public:
    vslist_type&    get_vslist(){ return vslist; }
    boost::thread_group&    get_tg(){ return vs_threads; }

    bool&    get_help() { return help; }
    bool&    get_debug() { return debug; }

    boost::posix_time::ptime&    get_starttime() { return starttime; }

    vslist_type::iterator    search_vslist( l7vs::virtualservice_element& in_elem ){
        return l7vsd::search_vslist( in_elem );
    }
    void    set_replication( boost::shared_ptr< l7vs::replication > inrep ){
        rep = inrep;
    }
    void    set_snmpbridge( boost::shared_ptr< l7vs::snmpbridge > inbridge ){
        bridge = inbridge;
    }
    sig_atomic_t    get_exit_requested(){
        return l7vsd::exit_requested;
    }
    sig_atomic_t    get_received_sig(){
        return l7vsd::received_sig;
    }
    void    set_exit_requested(sig_atomic_t in){
        l7vsd::exit_requested = in;
    }
    void    set_received_sig(sig_atomic_t in){
        l7vsd::received_sig = in;
    }
    void    sig_exit_handler_wp(){
        l7vsd::sig_exit_handler();
    }

};

// stub glibc functions
static int        (*org_daemon)(int nochdir, int noclose) = NULL;
int                daemon_ret = 0;

int    daemon( int nochdir, int noclose ){
    std::cout << "daemon" << std::endl;
    return daemon_ret;
}

// handler_wrapper
void*    handler_wrapper( void* );

// flags
int            call_count_test_handler = 0;    //test_handlerの呼出回数
int            arg_sig_test_handler = 0;        //test_handlerの引数(sig)


//util
template< typename InternetProtocol >
boost::asio::ip::basic_endpoint< InternetProtocol > string_to_endpoint( std::string str ){
    std::string::size_type pos = str.find( ":" );
    std::string    hostname = str.substr( 0, pos );
    std::string    portname = str.substr( pos+1, str.length() );
    boost::asio::io_service        io_service;
    typename InternetProtocol::resolver                resolver(io_service);
    typename InternetProtocol::resolver::query        query( hostname, portname );
    typename InternetProtocol::resolver::iterator    end;
    typename InternetProtocol::resolver::iterator    itr = resolver.resolve( query );
    if( itr == end ) return typename InternetProtocol::endpoint();
    return *itr;
}

//--tests--

void    list_virtual_service_test(){
    BOOST_MESSAGE( "----- list_virtual_service_test start -----" );

    l7vsd_test            vsd_test;

    boost::asio::io_service    io;
    l7vs::replication    rep;
    l7vs::virtualservice_element    e;

    boost::shared_ptr< l7vs::virtual_service > vs1( new l7vs::virtual_service( vsd_test, rep, e ) );
    boost::shared_ptr< l7vs::virtual_service > vs2( new l7vs::virtual_service( vsd_test, rep, e ) );
    vs1->element.protocol_module_name = "cinsert";
    vs2->element.protocol_module_name = "url";

    vsd_test.get_vslist().push_back(vs1);
    vsd_test.get_vslist().push_back(vs2);

    //virtualservice_element elem1;
    //virtualservice_element elem2;

    //normal case
    {
        l7vs::error_code    err;
        l7vs::l7vsd::vselist_type    vse_list;
    
        vsd_test.list_virtual_service( &vse_list, err );
        // unit_test[1] list_virtual_service normal case error_code check
        BOOST_CHECK( !err );
        // unit_test[2] list_virtual_service normal case vse_list num check
        BOOST_CHECK_EQUAL( vse_list.size(), 2U );
        // unit_test[3] list_virtual_service normal case vse_list content check
        l7vs::l7vsd::vselist_type::iterator itr = vse_list.begin();
        BOOST_CHECK_EQUAL( itr->protocol_module_name, "cinsert" );
        itr++;
        BOOST_CHECK_EQUAL( itr->protocol_module_name, "url" );
    }

    vsd_test.get_vslist().clear();

    //normal case 2
    {
        l7vs::error_code    err;
        l7vs::l7vsd::vselist_type    vse_list;

        vsd_test.list_virtual_service( &vse_list, err );
        // unit_test[4] list_virtual_service normal case 2(vslist empty) error_code check
        BOOST_CHECK( !err );
        // unit_test[5] list_virtual_service normal case 2(vslist empty) vse_list num check
        BOOST_CHECK_EQUAL( vse_list.size(), 0U );
    }

    BOOST_MESSAGE( "----- list_virtual_service_test end -----" );

}

void    list_virtual_service_verbose_test(){
    BOOST_MESSAGE( "----- list_virtual_service_verbose_test start -----" );

    l7vsd_test            vsd_test;

    boost::asio::io_service    io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );
    //l7vs::replication    rep(io);
    boost::shared_ptr< l7vs::snmpbridge >
                                    bridge( new l7vs::snmpbridge( vsd_test, io ) );
    vsd_test.set_snmpbridge( bridge );

    l7vs::virtualservice_element    e;

    boost::shared_ptr< l7vs::virtual_service > vs1( new l7vs::virtual_service( vsd_test, *rep, e ) );
    vs1->element.protocol_module_name = "cinsert";

    vsd_test.get_vslist().push_back(vs1);

    l7vs::Logger::setLogLevel( l7vs::LOG_CAT_L7VSD_NETWORK, l7vs::LOG_LV_FATAL );
    vsd_test.get_starttime() -= boost::posix_time::hours(1);

    //normal case
    {
        l7vs::error_code    err;
        l7vs::l7vsd_response    response;
    
        vsd_test.list_virtual_service_verbose( &response, err );
        // unit_test[6] list_virtual_service_verbose normal case error_code check
        BOOST_CHECK( !err );
        // unit_test[7] list_virtual_service_verbose normal case vse_list num check
        BOOST_CHECK_EQUAL( response.virtualservice_status_list.size(), 1U );
        // unit_test[8] list_virtual_service_verbose normal case vse_list content check
        l7vs::l7vsd::vselist_type::iterator itr = response.virtualservice_status_list.begin();
        BOOST_CHECK_EQUAL( itr->protocol_module_name, "cinsert" );
        // unit_test[9] list_virtual_service_verbose normal case replication_mode_status check
        BOOST_CHECK_EQUAL( response.replication_mode_status, l7vs::replication::REPLICATION_MASTER );
        // unit_test[10] list_virtual_service_verbose normal case log_status_list check
        l7vs::l7vsd::logstatus_list_type::iterator logitr;
        logitr = response.log_status_list.begin();
        BOOST_CHECK_EQUAL( logitr->second, l7vs::LOG_LV_FATAL );
        // unit_test[11] list_virtual_service_verbose normal case snmp_connection_status check
        BOOST_CHECK_EQUAL( response.snmp_connection_status, true );
        // unit_test[12] list_virtual_service_verbose normal case snmp_log_status_list check
        l7vs::l7vsd::logstatus_list_type::iterator snmplogitr;
        snmplogitr = response.snmp_log_status_list.begin();
        BOOST_CHECK_EQUAL( logitr->second, l7vs::LOG_LV_FATAL );
        // unit_test[13] list_virtual_service_verbose normal case total_client_recv_byte check
        BOOST_CHECK_EQUAL( response.total_client_recv_byte, 12345ULL );
        // unit_test[14] list_virtual_service_verbose normal case total_client_send_byte check
        BOOST_CHECK_EQUAL( response.total_client_send_byte, 45678ULL );
        // unit_test[15] list_virtual_service_verbose normal case total_realserver_recv_byte check
        BOOST_CHECK_EQUAL( response.total_realserver_recv_byte, 34567ULL );
        // unit_test[16] list_virtual_service_verbose normal case total_realserver_send_byte check
        BOOST_CHECK_EQUAL( response.total_realserver_send_byte, 23456ULL );
        // unit_test[17] list_virtual_service_verbose normal case total_bps check
        BOOST_CHECK( response.total_bps > 0ULL );

    }

    vsd_test.get_vslist().clear();

    //normal case 2
    {
        l7vs::error_code    err;
        l7vs::l7vsd_response    response;

        vsd_test.list_virtual_service_verbose( &response, err );
        // unit_test[18] list_virtual_service_verbose normal case 2(vslist empty) error_code check
        BOOST_CHECK( !err );
        // unit_test[19] list_virtual_service_verbose normal case 2(vslist empty) vse_list num check
        BOOST_CHECK_EQUAL( response.virtualservice_status_list.size(), 0U );
        // unit_test[20] list_virtual_service_verbose normal case 2(vslist empty) replication_mode_status check
        BOOST_CHECK_EQUAL( response.replication_mode_status, l7vs::replication::REPLICATION_MASTER );
        // unit_test[21] list_virtual_service_verbose normal case 2(vslist empty) log_status_list check
        l7vs::l7vsd::logstatus_list_type::iterator logitr;
        logitr = response.log_status_list.begin();
        BOOST_CHECK_EQUAL( logitr->second, l7vs::LOG_LV_FATAL );
        // unit_test[22] list_virtual_service_verbose normal case 2(vslist empty) snmp_connection_status check
        BOOST_CHECK_EQUAL( response.snmp_connection_status, true );
        // unit_test[23] list_virtual_service_verbose normal case 2(vslist empty) snmp_log_status_list check
        l7vs::l7vsd::logstatus_list_type::iterator snmplogitr;
        snmplogitr = response.snmp_log_status_list.begin();
        BOOST_CHECK_EQUAL( logitr->second, l7vs::LOG_LV_FATAL );
        // unit_test[24] list_virtual_service_verbose normal 2(vslist empty) case total_client_recv_byte check
        BOOST_CHECK_EQUAL( response.total_client_recv_byte, 0ULL );
        // unit_test[25] list_virtual_service_verbose normal 2(vslist empty) case total_client_send_byte check
        BOOST_CHECK_EQUAL( response.total_client_send_byte, 0ULL );
        // unit_test[26] list_virtual_service_verbose normal 2(vslist empty) case total_realserver_recv_byte check
        BOOST_CHECK_EQUAL( response.total_realserver_recv_byte, 0ULL );
        // unit_test[27] list_virtual_service_verbose normal 2(vslist empty) case total_realserver_send_byte check
        BOOST_CHECK_EQUAL( response.total_realserver_send_byte, 0ULL );
        // unit_test[28] list_virtual_service_verbose normal case 2(vslist empty) total_bps check
        BOOST_CHECK( response.total_bps == 0ULL );
    }

    BOOST_MESSAGE( "----- list_virtual_service_verbose_test end -----" );

}

void    add_virtual_service_test(){
    BOOST_MESSAGE( "----- add_virtual_service_test start -----" );

    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

// 正常系
    {
        l7vs::virtualservice_element    elem;
        elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem, err );
        // unit_test[29] add_virtual_service 正常系 エラーコード確認
        BOOST_CHECK( !err );
        // unit_test[30] add_virtual_service 正常系 vslist数確認
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 1U );
        // unit_test[31] add_virtual_service 正常系 vslist内容確認
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
        BOOST_CHECK( (*itr)->get_element() == elem );
        // unit_test[32] add_virtual_service 正常系 initialize確認
        BOOST_CHECK_EQUAL( (*itr)->initialize_called, true );
        // unit_test[33] add_virtual_service 正常系 set_virtualservice確認
        BOOST_CHECK_EQUAL( (*itr)->set_virtualservice_called, true );
        // unit_test[34] add_virtual_service 正常系 run確認
        vsd_test.get_tg().join_all();
        BOOST_CHECK_EQUAL( (*itr)->run_called, true );
        // unit_test[35] add_virtual_service 正常系 replication switch_to_master確認
        BOOST_CHECK_EQUAL( rep->switch_to_master_called, true );
    }

    rep->switch_to_master_called = false;

// 正常系２
    {
        l7vs::virtualservice_element    elem2;
        elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem2, err );
        // unit_test[36] add_virtual_service 正常系２(vs2個め) エラーコード確認
        BOOST_CHECK( !err );
        // unit_test[37] add_virtual_service 正常系２(vs2個め) vslist数確認
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 2U );
        // unit_test[38] add_virtual_service 正常系２(vs2個め) vslist内容確認
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
        ++itr;
        BOOST_CHECK( (*itr)->get_element() == elem2 );
        // unit_test[39] add_virtual_service 正常系２(vs2個め) initialize確認
        BOOST_CHECK_EQUAL( (*itr)->initialize_called, true );
        // unit_test[40] add_virtual_service 正常系２(vs2個め) set_virtualservice確認
        BOOST_CHECK_EQUAL( (*itr)->set_virtualservice_called, true );
        // unit_test[41] add_virtual_service 正常系２(vs2個め) run確認
        vsd_test.get_tg().join_all();
        BOOST_CHECK_EQUAL( (*itr)->run_called, true );
        // unit_test[42] add_virtual_service 正常系２(vs2個め) replication switch_to_master確認
        BOOST_CHECK_EQUAL( rep->switch_to_master_called, false );
    }

//異常系
    // unit_test[43] add_virtual_service 異常系(既に同じvsがある場合) エラーコード確認
    {
        l7vs::virtualservice_element    elem3;
        elem3.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem3, err );
        BOOST_CHECK( err );
    }

    // unit_test[44] add_virtual_service 異常系２(replicaitonがnullの場合) 戻り値確認
    {
        l7vsd_test            vsd_test2;
        l7vs::virtualservice_element    elem;
        elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

        l7vs::error_code err;
        vsd_test2.add_virtual_service( &elem, err );
        BOOST_CHECK( err );
    }

    // unit_test[45] add_virtual_service 異常系３(initialize失敗) 戻り値確認
    {
        l7vs::virtualservice_element    elem4;
        elem4.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "30.30.30.30:7777" );
        l7vs::virtual_service::initialize_fail = true;

        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem4, err );
        BOOST_CHECK( err );

        l7vs::virtual_service::initialize_fail = false;
    }

    // unit_test[46] add_virtual_service 異常系４(set_virtualservice失敗) 戻り値確認
    {
        l7vs::virtualservice_element    elem5;
        elem5.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "40.40.40.40:6666" );
        l7vs::virtual_service::set_virtualservice_fail = true;

        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem5, err );
        BOOST_CHECK( err );

        l7vs::virtual_service::set_virtualservice_fail = false;
    }

    BOOST_MESSAGE( "----- add_virtual_service_test end -----" );
}

void    del_virtual_service_test(){
    BOOST_MESSAGE( "----- del_virtual_service_test start -----" );

    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    l7vs::virtualservice_element    elem;
    elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );
    l7vs::virtualservice_element    elem2;
    elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

// prepare vslist
    {
        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem, err );
        vsd_test.add_virtual_service( &elem2, err );
    }

// normal case
    {
        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;

        l7vs::error_code err;
        vsd_test.del_virtual_service( &elem, err );
        // unit_test[47] del_virtual_service normal case error_code check
        BOOST_CHECK( !err );
        // unit_test[48] del_virtual_service normal case vslist num check
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 1U );
        // unit_test[49] del_virtual_service normal case stop call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, true );
        // unit_test[50] del_virtual_service normal case finalize call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, true );
        // unit_test[51] del_virtual_service normal case replication switch_to_slave call check
        BOOST_CHECK_EQUAL( rep->switch_to_slave_called, false );

        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;

        vsd_test.del_virtual_service( &elem2, err );
        // unit_test[52] del_virtual_service normal case 2(delete last vs) error_code check
        BOOST_CHECK( !err );
        // unit_test[53] del_virtual_service normal case 2(delete last vs) vslist num check
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );
        // unit_test[54] del_virtual_service normal case 2(delete last vs) stop call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, true );
        // unit_test[55] del_virtual_service normal case 2(delete last vs) finalize call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, true );
        // unit_test[56] del_virtual_service normal case 2(delete last vs) replication switch_to_slave call check
        BOOST_CHECK_EQUAL( rep->switch_to_slave_called, true );

        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;
    }

// error case 1
    {
        l7vs::error_code err;
        vsd_test.del_virtual_service( &elem, err );

        // unit_test[57] del_virtual_service error_case 1(vslist empty) error_code check
        BOOST_CHECK( err );
        // unit_test[58] del_virtual_service error_case 1(vslist empty) vslist num check
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );

        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;
    }

// vslist prepare
    {
        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem, err );
    }

// error case 2
    {
        l7vs::virtualservice_element    elem3;
        elem3.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "30.30.30.30:7777" );

        l7vs::error_code err;
        vsd_test.del_virtual_service( &elem3, err );
        // unit_test[59] del_virtual_service error case 2(no target vs) error code check
        BOOST_CHECK( err );
        // unit_test[60] del_virtual_service error case 2(no target vs) vslist num check
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 1U );

        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;
    }

// error case 3
    {
        l7vs::virtual_service::finalize_fail = true;

        l7vs::error_code err;
        vsd_test.del_virtual_service( &elem, err );
        // unit_test[61] del_virtual_service error case 3(error in finalize) error_code check
        BOOST_CHECK( err );
        // unit_test[62] del_virtual_service error case 3(error in finalize) vslist num check
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );

        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;
    }

    BOOST_MESSAGE( "----- del_virtual_service_test end -----" );

}

void    edit_virtual_service_test(){
    BOOST_MESSAGE( "----- edit_virtual_service_test start -----" );
    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    l7vs::virtualservice_element    elem;
    elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

// prepare vslist
    {
        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem, err );
    }

// normal case
    {
        l7vs::error_code err;
        vsd_test.edit_virtual_service( &elem, err );
        // unit_test[63] edit_virtual_service normal_case error_code check
        BOOST_CHECK( !err );
        // unit_test[64] edit_virtual_service normal_case edit_virtual_service call check
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
        BOOST_CHECK_EQUAL( (*itr)->edit_virtualservice_called, true );
    }
// error case
    {
        l7vs::virtual_service::edit_virtualservice_fail = true;

        l7vs::error_code err;
        vsd_test.edit_virtual_service( &elem, err );
        // unit_test[65] edit_virtual_service error_case(vs::edit_virtualservice fail) error_code check
        BOOST_CHECK( err );
        l7vs::virtual_service::edit_virtualservice_fail = false;
    }

    {
        l7vs::virtualservice_element    elem2;
        elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

        l7vs::error_code err;
        vsd_test.edit_virtual_service( &elem2, err );
        // unit_test[66] edit_virtual_service error_case(no target vs found) error_code check
        BOOST_CHECK( err );
    }

    BOOST_MESSAGE( "----- edit_virtual_service_test end -----" );
}

void    add_real_server_test(){
    BOOST_MESSAGE( "----- add_real_server_test start -----" );
    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    l7vs::virtualservice_element    elem;
    elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

// prepare vslist
    {
        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem, err );
    }

// normal case
    {
        l7vs::error_code err;
        vsd_test.add_real_server( &elem, err );
        // unit_test[67] add_real_server normal_case error_code check
        BOOST_CHECK( !err );
        // unit_test[68] add_real_server normal_case add_realserver call check
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
        BOOST_CHECK_EQUAL( (*itr)->add_realserver_called, true );
    }
// error case
    {
        l7vs::virtual_service::add_realserver_fail = true;

        l7vs::error_code err;
        vsd_test.add_real_server( &elem, err );
        // unit_test[69] add_real_server error_case(vs::add_realserver fail) error_code check
        BOOST_CHECK( err );
        l7vs::virtual_service::add_realserver_fail = false;
    }

    {
        l7vs::virtualservice_element    elem2;
        elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

        l7vs::error_code err;
        vsd_test.add_real_server( &elem2, err );
        // unit_test[70] add_real_server error_case(no target vs found) error_code check
        BOOST_CHECK( err );
    }

    BOOST_MESSAGE( "----- add_real_server_test end -----" );
}

void    del_real_server_test(){
    BOOST_MESSAGE( "----- del_real_server_test start -----" );
    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    l7vs::virtualservice_element    elem;
    elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

// prepare vslist
    {
        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem, err );
    }

// normal case
    {
        l7vs::error_code err;
        vsd_test.del_real_server( &elem, err );
        // unit_test[71] del_real_server normal_case error_code check
        BOOST_CHECK( !err );
        // unit_test[72] del_real_server normal_case del_realserver call check
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
        BOOST_CHECK_EQUAL( (*itr)->del_realserver_called, true );
    }
// error case
    {
        l7vs::virtual_service::del_realserver_fail = true;

        l7vs::error_code err;
        vsd_test.del_real_server( &elem, err );
        // unit_test[73] del_real_server error_case(vs::del_realserver fail) error_code check
        BOOST_CHECK( err );
        l7vs::virtual_service::del_realserver_fail = false;
    }

    {
        l7vs::virtualservice_element    elem2;
        elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

        l7vs::error_code err;
        vsd_test.del_real_server( &elem2, err );
        // unit_test[74] del_real_server error_case(no target vs found) error_code check
        BOOST_CHECK( err );
    }

    BOOST_MESSAGE( "----- del_real_server_test end -----" );
}

void    edit_real_server_test(){
    BOOST_MESSAGE( "----- edit_real_server_test start -----" );
    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    l7vs::virtualservice_element    elem;
    elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );

// prepare vslist
    {
        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem, err );
    }

// normal case
    {
        l7vs::error_code err;
        vsd_test.edit_real_server( &elem, err );
        // unit_test[75] edit_real_server normal_case error_code check
        BOOST_CHECK( !err );
        // unit_test[76] edit_real_server normal_case edit_realserver call check
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.get_vslist().begin();
        BOOST_CHECK_EQUAL( (*itr)->edit_realserver_called, true );
    }
// error case
    {
        l7vs::virtual_service::edit_realserver_fail = true;

        l7vs::error_code err;
        vsd_test.edit_real_server( &elem, err );
        // unit_test[77] edit_real_server error_case(vs::edit_realserver fail) error_code check
        BOOST_CHECK( err );
        l7vs::virtual_service::edit_realserver_fail = false;
    }

    {
        l7vs::virtualservice_element    elem2;
        elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

        l7vs::error_code err;
        vsd_test.edit_real_server( &elem2, err );
        // unit_test[78] edit_real_server error_case(no target vs found) error_code check
        BOOST_CHECK( err );
    }

    BOOST_MESSAGE( "----- edit_real_server_test end -----" );
}

void    flush_virtual_service_test(){
    BOOST_MESSAGE( "----- flush_virtual_service_test start -----" );

    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    l7vs::virtualservice_element    elem;
    elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );
    l7vs::virtualservice_element    elem2;
    elem2.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

// prepare vslist
    {
        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem, err );
        vsd_test.add_virtual_service( &elem2, err );
    }

// normal case
    {
        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;

        l7vs::error_code err;
        vsd_test.flush_virtual_service( err );
        // unit_test[79] flush_virtual_service normal case error_code check
        BOOST_CHECK( !err );
        // unit_test[80] flush_virtual_service normal case vslist num check
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );
        // unit_test[81] flush_virtual_service normal case stop call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, true );
        // unit_test[82] flush_virtual_service normal case finalize call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, true );
        // unit_test[83] flush_virtual_service normal case replication switch_to_slave call check
        BOOST_CHECK_EQUAL( rep->switch_to_slave_called, true );

        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;
        rep->switch_to_slave_called = false;

        vsd_test.flush_virtual_service( err );
        // unit_test[84] flush_virtual_service normal case 2(vslist empty) error_code check
        BOOST_CHECK( !err );
        // unit_test[85] flush_virtual_service normal case 2(vslist empty) vslist num check
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );
        // unit_test[86] flush_virtual_service normal case 2(vslist empty) stop call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, false );
        // unit_test[87] flush_virtual_service normal case 2(vslist empty) finalize call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, false );
        // unit_test[88] flush_virtual_service normal case 2(vslist empty) replication switch_to_slave call check
        BOOST_CHECK_EQUAL( rep->switch_to_slave_called, true );

        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;

        rep->switch_to_slave_called = false;
    }

// prepare vslist
    {
        l7vs::error_code err;
        vsd_test.add_virtual_service( &elem, err );
    }

// error case 1
    {
        l7vs::virtual_service::finalize_fail = true;

        l7vs::error_code err;
        vsd_test.flush_virtual_service( err );

        // unit_test[89] flush_virtual_service error case 1(error in finalize) error_code check
        BOOST_CHECK( err );
        // unit_test[90] flush_virtual_service error case 1(error in finalize) vslist num check
        BOOST_CHECK_EQUAL( vsd_test.get_vslist().size(), 0U );
        // unit_test[91] flush_virtual_service error case 1(error in finalize) stop call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::stop_called, true );
        // unit_test[92] flush_virtual_service error case 1(error in finalize) finalize call check
        BOOST_CHECK_EQUAL( l7vs::virtual_service::finalize_called, true );
        // unit_test[93] flush_virtual_service error case 1(error in finalize) replication switch_to_slave call check
        BOOST_CHECK_EQUAL( rep->switch_to_slave_called, true );

        l7vs::virtual_service::stop_called = false;
        l7vs::virtual_service::finalize_called = false;
        l7vs::virtual_service::finalize_fail = false;
    }

    BOOST_MESSAGE( "----- flush_virtual_service_test end -----" );

}

void    replication_command_test(){
    BOOST_MESSAGE( "----- replication_command_test start -----" );
    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    // REP_START
    {
        l7vs::replication::start_called = false;
        l7vs::error_code err;
        l7vs::l7vsadm_request::REPLICATION_COMMAND_TAG cmd = l7vs::l7vsadm_request::REP_START;
        vsd_test.replication_command( &cmd, err );
        // unit_test[94] replication_command normal case 1(REP_START) error_code check
        BOOST_CHECK( !err );
        // unit_test[95] replication_command normal case 1(REP_START) start call check
        BOOST_CHECK_EQUAL( l7vs::replication::start_called, true );
    }

    // REP_STOP
    {
        l7vs::replication::stop_called = false;
        l7vs::error_code err;
        l7vs::l7vsadm_request::REPLICATION_COMMAND_TAG cmd = l7vs::l7vsadm_request::REP_STOP;
        vsd_test.replication_command( &cmd, err );
        // unit_test[96] replication_command normal case 2(REP_STOP) error_code check
        BOOST_CHECK( !err );
        // unit_test[97] replication_command normal case 2(REP_STOP) stop call check
        BOOST_CHECK_EQUAL( l7vs::replication::stop_called, true );
    }

    // REP_FORCE
    {
        l7vs::replication::force_replicate_called = false;
        l7vs::error_code err;
        l7vs::l7vsadm_request::REPLICATION_COMMAND_TAG cmd = l7vs::l7vsadm_request::REP_FORCE;
        vsd_test.replication_command( &cmd, err );
        // unit_test[98] replication_command normal case 3(REP_FORCE) error_code check
        BOOST_CHECK( !err );
        // unit_test[99] replication_command normal case 3(REP_FORCE) stop call check
        BOOST_CHECK_EQUAL( l7vs::replication::force_replicate_called, true );
    }

    // REP_DUMP
    {
        l7vs::replication::dump_memory_called = false;
        l7vs::error_code err;
        l7vs::l7vsadm_request::REPLICATION_COMMAND_TAG cmd = l7vs::l7vsadm_request::REP_DUMP;
        vsd_test.replication_command( &cmd, err );
        // unit_test[100] replication_command normal case 4(REP_DUMP) error_code check
        BOOST_CHECK( !err );
        // unit_test[101] replication_command normal case 4(REP_DUMP) stop call check
        BOOST_CHECK_EQUAL( l7vs::replication::dump_memory_called, true );
    }

    // error case
    {
        l7vs::error_code err;
        l7vs::l7vsadm_request::REPLICATION_COMMAND_TAG cmd = l7vs::l7vsadm_request::REP_NONE;
        vsd_test.replication_command( &cmd, err );
        // unit_test[102] replication_command error case (REP_NONE) error_code check
        BOOST_CHECK( err );
    }


    BOOST_MESSAGE( "----- replication_command_test end -----" );

}

void    set_loglevel_test(){
    BOOST_MESSAGE( "----- set_loglevel_test start -----" );
    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    std::vector< l7vs::LOG_CATEGORY_TAG > categories;
    categories.push_back( l7vs::LOG_CAT_L7VSD_NETWORK );
    categories.push_back( l7vs::LOG_CAT_L7VSD_NETWORK_QOS );
    categories.push_back( l7vs::LOG_CAT_L7VSD_NETWORK_BANDWIDTH );
    categories.push_back( l7vs::LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION );
    categories.push_back( l7vs::LOG_CAT_L7VSD_NETWORK_ACCESS );
    categories.push_back( l7vs::LOG_CAT_L7VSD_MAINTHREAD );
    categories.push_back( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE );
    categories.push_back( l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD );
    categories.push_back( l7vs::LOG_CAT_L7VSD_SESSION );
    categories.push_back( l7vs::LOG_CAT_L7VSD_SESSION_THREAD );
    categories.push_back( l7vs::LOG_CAT_L7VSD_REALSERVER );
    categories.push_back( l7vs::LOG_CAT_L7VSD_SORRYSERVER );
    categories.push_back( l7vs::LOG_CAT_L7VSD_MODULE );
    categories.push_back( l7vs::LOG_CAT_L7VSD_REPLICATION );
    categories.push_back( l7vs::LOG_CAT_L7VSD_REPLICATION_SENDTHREAD );
    categories.push_back( l7vs::LOG_CAT_L7VSD_PARAMETER );
    categories.push_back( l7vs::LOG_CAT_L7VSD_LOGGER );
    categories.push_back( l7vs::LOG_CAT_L7VSD_COMMAND );
    categories.push_back( l7vs::LOG_CAT_L7VSD_START_STOP );
    categories.push_back( l7vs::LOG_CAT_L7VSD_SYSTEM );
    categories.push_back( l7vs::LOG_CAT_L7VSD_SYSTEM_MEMORY );
    categories.push_back( l7vs::LOG_CAT_L7VSD_SYSTEM_ENDPOINT );
    categories.push_back( l7vs::LOG_CAT_L7VSD_SYSTEM_SIGNAL );
    categories.push_back( l7vs::LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT );

    // unit_test[103] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK) error_code check
    // unit_test[104] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK) loglevel check

    // unit_test[105] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK_QOS) error_code check
    // unit_test[106] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK_QOS) loglevel check

    // unit_test[107] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK_BANDWIDTH) error_code check
    // unit_test[108] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK_BANDWIDTH) loglevel check

    // unit_test[109] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION) error_code check
    // unit_test[110] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION) loglevel check

    // unit_test[111] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK_ACCESS) error_code check
    // unit_test[112] set_loglevel normal case (LOG_CAT_L7VSD_NETWORK_ACCESS) loglevel check

    // unit_test[113] set_loglevel normal case (LOG_CAT_L7VSD_MAINTHREAD) error_code check
    // unit_test[114] set_loglevel normal case (LOG_CAT_L7VSD_MAINTHREAD) loglevel check

    // unit_test[115] set_loglevel normal case (LOG_CAT_L7VSD_VIRTUALSERVICE) error_code check
    // unit_test[116] set_loglevel normal case (LOG_CAT_L7VSD_VIRTUALSERVICE) loglevel check

    // unit_test[117] set_loglevel normal case (LOG_CAT_L7VSD_VIRTUALSERVICE_THRAD) error_code check
    // unit_test[118] set_loglevel normal case (LOG_CAT_L7VSD_VIRTUALSERVICE_THRAD) loglevel check

    // unit_test[119] set_loglevel normal case (LOG_CAT_L7VSD_SESSION) error_code check
    // unit_test[120] set_loglevel normal case (LOG_CAT_L7VSD_SESSION) loglevel check

    // unit_test[121] set_loglevel normal case (LOG_CAT_L7VSD_SESSION_THREAD) error_code check
    // unit_test[122] set_loglevel normal case (LOG_CAT_L7VSD_SESSION_THREAD) loglevel check

    // unit_test[123] set_loglevel normal case (LOG_CAT_L7VSD_REALSERVER) error_code check
    // unit_test[124] set_loglevel normal case (LOG_CAT_L7VSD_REALSERVER) loglevel check

    // unit_test[125] set_loglevel normal case (LOG_CAT_L7VSD_SORRYSERVER) error_code check
    // unit_test[126] set_loglevel normal case (LOG_CAT_L7VSD_SORRYSERVER) loglevel check

    // unit_test[127] set_loglevel normal case (LOG_CAT_L7VSD_MODULE) error_code check
    // unit_test[128] set_loglevel normal case (LOG_CAT_L7VSD_MODULE) loglevel check

    // unit_test[129] set_loglevel normal case (LOG_CAT_L7VSD_REPLICATION) error_code check
    // unit_test[130] set_loglevel normal case (LOG_CAT_L7VSD_REPLICATION) loglevel check

    // unit_test[131] set_loglevel normal case (LOG_CAT_L7VSD_REPLICATION_SENDTHREAD) error_code check
    // unit_test[132] set_loglevel normal case (LOG_CAT_L7VSD_REPLICATION_SENDTHREAD) loglevel check

    // unit_test[133] set_loglevel normal case (LOG_CAT_L7VSD_PARAMETER) error_code check
    // unit_test[134] set_loglevel normal case (LOG_CAT_L7VSD_PARAMETER) loglevel check

    // unit_test[135] set_loglevel normal case (LOG_CAT_L7VSD_LOGGER) error_code check
    // unit_test[136] set_loglevel normal case (LOG_CAT_L7VSD_LOGGER) loglevel check

    // unit_test[137] set_loglevel normal case (LOG_CAT_L7VSD_COMMAND) error_code check
    // unit_test[138] set_loglevel normal case (LOG_CAT_L7VSD_COMMAND) loglevel check

    // unit_test[139] set_loglevel normal case (LOG_CAT_L7VSD_START_STOP) error_code check
    // unit_test[140] set_loglevel normal case (LOG_CAT_L7VSD_START_STOP) loglevel check

    // unit_test[141] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM) error_code check
    // unit_test[142] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM) loglevel check

    // unit_test[143] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM_MEMORY) error_code check
    // unit_test[144] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM_MEMORY) loglevel check

    // unit_test[145] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM_ENDPOINT) error_code check
    // unit_test[146] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM_ENDPOINT) loglevel check

    // unit_test[147] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM_SIGNAL) error_code check
    // unit_test[148] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM_SIGNAL) loglevel check

    // unit_test[149] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT) error_code check
    // unit_test[150] set_loglevel normal case (LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT) loglevel check

    for( unsigned int i = 0; i < categories.size(); ++i ){
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_DEBUG;
        vsd_test.set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_DEBUG );

        level = l7vs::LOG_LV_INFO;
        vsd_test.set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_INFO );

        level = l7vs::LOG_LV_WARN;
        vsd_test.set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_WARN );

        level = l7vs::LOG_LV_ERROR;
        vsd_test.set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_ERROR );

        level = l7vs::LOG_LV_FATAL;
        vsd_test.set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_FATAL );
    }

    // unit_test[151] set_loglevel normal case (LOG_CAT_END(all)) error_code check
    // unit_test[152] set_loglevel normal case (LOG_CAT_END(all)) loglevel check

    // set loglevel all(LOG_LV_DEBUG)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_DEBUG;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        for( unsigned int i = 0; i < categories.size(); ++i )
            BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_DEBUG );
    }

    // set loglevel all(LOG_LV_INFO)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_INFO;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        for( unsigned int i = 0; i < sizeof(categories); ++i )
            BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_INFO );
    }

    // set loglevel all(LOG_LV_WARN)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_WARN;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        for( unsigned int i = 0; i < sizeof(categories); ++i )
            BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_WARN );
    }

    // set loglevel all(LOG_LV_ERROR)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_ERROR;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        for( unsigned int i = 0; i < sizeof(categories); ++i )
            BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_ERROR );
    }

    // set loglevel all(LOG_LV_FATAL)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_FATAL;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        for( unsigned int i = 0; i < sizeof(categories); ++i )
            BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( categories[i] ), l7vs::LOG_LV_FATAL );
    }

    BOOST_MESSAGE( "----- set_loglevel_test end -----" );
}

void    set_snmp_loglevel_test(){
    BOOST_MESSAGE( "----- set_snmp_loglevel_test start -----" );
    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    boost::shared_ptr< l7vs::snmpbridge >
                                    bridge( new l7vs::snmpbridge( vsd_test, io ) );
    vsd_test.set_snmpbridge( bridge );

    std::vector< l7vs::LOG_CATEGORY_TAG > categories;
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_START_STOP );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_MANAGER_SEND );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_L7VSD_SEND );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_LOGGER );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_PARAMETER );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_SYSTEM );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_SYSTEM_MEMORY );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL );
    categories.push_back( l7vs::LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT );

    // unit_test[153] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_START_STOP) error_code check
    // unit_test[154] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_START_STOP) loglevel check

    // unit_test[155] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_MANAGER_RECEIVE) error_code check
    // unit_test[156] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_MANAGER_RECEIVE) loglevel check

    // unit_test[157] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_MANAGER_SEND) error_code check
    // unit_test[158] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_MANAGER_SEND) loglevel check

    // unit_test[159] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_L7VSD_RECEIVE) error_code check
    // unit_test[160] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_L7VSD_RECEIVE) loglevel check

    // unit_test[161] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_L7VSD_SEND) error_code check
    // unit_test[162] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_L7VSD_SEND) loglevel check

    // unit_test[163] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_LOGGER) error_code check
    // unit_test[164] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_LOGGER) loglevel check

    // unit_test[165] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_PARAMETER) error_code check
    // unit_test[166] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_PARAMETER) loglevel check

    // unit_test[167] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM) error_code check
    // unit_test[168] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM) loglevel check

    // unit_test[169] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM_MEMORY) error_code check
    // unit_test[170] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM_MEMORY) loglevel check

    // unit_test[171] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT) error_code check
    // unit_test[172] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT) loglevel check

    // unit_test[173] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL) error_code check
    // unit_test[174] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL) loglevel check

    // unit_test[175] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT) error_code check
    // unit_test[176] set_snmp_loglevel normal case (LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT) loglevel check

    for( unsigned int i = 0; i < categories.size(); ++i ){
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_DEBUG;
        vsd_test.snmp_set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_category, categories[i] );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_level, l7vs::LOG_LV_DEBUG );

        level = l7vs::LOG_LV_INFO;
        vsd_test.snmp_set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_category, categories[i] );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_level, l7vs::LOG_LV_INFO );

        level = l7vs::LOG_LV_WARN;
        vsd_test.snmp_set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_category, categories[i] );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_level, l7vs::LOG_LV_WARN );

        level = l7vs::LOG_LV_ERROR;
        vsd_test.snmp_set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_category, categories[i] );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_level, l7vs::LOG_LV_ERROR );

        level = l7vs::LOG_LV_FATAL;
        vsd_test.snmp_set_loglevel( &categories[i], &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_category, categories[i] );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_level, l7vs::LOG_LV_FATAL );
    }

    // unit_test[177] set_snmp_loglevel normal case (LOG_CAT_END(all)) error_code check
    // unit_test[178] set_snmp_loglevel normal case (LOG_CAT_END(all)) loglevel check

    // set loglevel all(LOG_LV_DEBUG)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_DEBUG;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.snmp_set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_all_level, l7vs::LOG_LV_DEBUG );
    }

    // set loglevel all(LOG_LV_INFO)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_INFO;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.snmp_set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_all_level, l7vs::LOG_LV_INFO );
    }

    // set loglevel all(LOG_LV_WARN)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_WARN;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.snmp_set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_all_level, l7vs::LOG_LV_WARN );
    }

    // set loglevel all(LOG_LV_ERROR)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_ERROR;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.snmp_set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_all_level, l7vs::LOG_LV_ERROR );
    }

    // set loglevel all(LOG_LV_FATAL)
    {
        l7vs::error_code err;
        l7vs::LOG_LEVEL_TAG    level = l7vs::LOG_LV_FATAL;
        l7vs::LOG_CATEGORY_TAG    category = l7vs::LOG_CAT_END;
        vsd_test.snmp_set_loglevel( &category, &level, err );
        BOOST_CHECK( !err );
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::change_loglevel_all_level, l7vs::LOG_LV_FATAL );
    }

    BOOST_MESSAGE( "----- set_snmp_loglevel_test end -----" );
}

void    reload_parameter_test(){
    BOOST_MESSAGE( "----- reload_parameter_test start -----" );
    l7vsd_test            vsd_test;

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );
    boost::shared_ptr< l7vs::snmpbridge >
                                    bridge( new l7vs::snmpbridge( vsd_test, io ) );
    vsd_test.set_snmpbridge( bridge );

    // replication reload
    {
        l7vs::replication::reset_called = false;
        l7vs::PARAMETER_COMPONENT_TAG comp = l7vs::PARAM_COMP_REPLICATION;
        l7vs::error_code err;
        vsd_test.reload_parameter( &comp, err );
        // unit_test[179] reload_parameter normal case (replication reload) error_code check
        BOOST_CHECK( !err );
        // unit_test[180] reload_parameter normal case (replication reload) reset call check
        BOOST_CHECK_EQUAL( l7vs::replication::reset_called, true );
    }

    // snmpbridge reload
    {
        l7vs::snmpbridge::reload_config_called = false;
        l7vs::PARAMETER_COMPONENT_TAG comp = l7vs::PARAM_COMP_SNMPAGENT;
        l7vs::error_code err;
        vsd_test.reload_parameter( &comp, err );
        // unit_test[181] reload_parameter normal case (snmpbridge reload) error_code check
        BOOST_CHECK( !err );
        // unit_test[182] reload_parameter normal case (snmpbridge reload) reload_conf call check
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::reload_config_called, true );
    }

    // logger reload
    {
        l7vs::Logger::setLogLevel( l7vs::LOG_CAT_L7VSD_NETWORK, l7vs::LOG_LV_FATAL );
        l7vs::PARAMETER_COMPONENT_TAG comp = l7vs::PARAM_COMP_LOGGER;
        l7vs::error_code err;
        vsd_test.reload_parameter( &comp, err );
        // unit_test[183] reload_parameter normal case (logger reload) error_code check
        BOOST_CHECK( !err );
        // unit_test[184] reload_parameter normal case (logger reload) log level default check
        BOOST_CHECK_EQUAL( l7vs::Logger::getLogLevel( l7vs::LOG_CAT_L7VSD_NETWORK ), l7vs::LOG_LV_INFO );
    }

    // error case
    {
        l7vs::PARAMETER_COMPONENT_TAG comp = l7vs::PARAM_COMP_NOCAT;
        l7vs::error_code err;
        vsd_test.reload_parameter( &comp, err );
        // unit_test[185] reload_parameter error case error_code check
        BOOST_CHECK( err );
    }

    BOOST_MESSAGE( "----- reload_parameter_test end -----" );
}

void    search_vslist_test(){
    BOOST_MESSAGE( "----- search_vslist_test start -----" );
    l7vsd_test            vsd_test;

    boost::asio::io_service    io;
    l7vs::replication    rep;
    l7vs::virtualservice_element    e;

    boost::shared_ptr< l7vs::virtual_service > vs1( new l7vs::virtual_service( vsd_test, rep, e ) );
    boost::shared_ptr< l7vs::virtual_service > vs2( new l7vs::virtual_service( vsd_test, rep, e ) );

    vs1->element.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:9999" );
    vs2->element.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );

    vsd_test.get_vslist().push_back(vs1);
    vsd_test.get_vslist().push_back(vs2);

    // unit_test[186] search_vslist normal case return value check
    {
        l7vs::virtualservice_element    elem;
        elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
        BOOST_CHECK( (*itr)->get_element() == elem );
    }

    // unit_test[187] search_vslist normal case 2(vs not found) return value check
    {
        l7vs::virtualservice_element    elem;
        elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "30.30.30.30:7777" );
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
        BOOST_CHECK( itr == vsd_test.get_vslist().end() );
    }

    // unit_test[188] search_vslist normal case 3(vslist empty) return value check
    vsd_test.get_vslist().clear();
    {
        l7vs::virtualservice_element    elem;
        elem.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "20.20.20.20:8888" );
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
        BOOST_CHECK( itr == vsd_test.get_vslist().end() );
    }

    //udpmode
    boost::shared_ptr< l7vs::virtual_service > vs3( new l7vs::virtual_service( vsd_test, rep, e ) );
    boost::shared_ptr< l7vs::virtual_service > vs4( new l7vs::virtual_service( vsd_test, rep, e ) );

    vs3->element.udpmode = true;
    vs4->element.udpmode = true;
    vs3->element.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "40.40.40.40:6666" );
    vs4->element.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "50.50.50.50:5555" );

    vsd_test.get_vslist().push_back(vs3);
    vsd_test.get_vslist().push_back(vs4);
    // unit_test[189] search_vslist normal case 4(udpmode) return value check
    {
        l7vs::virtualservice_element    elem;
        elem.udpmode = true;
        elem.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "40.40.40.40:6666" );
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
        BOOST_CHECK( (*itr)->get_element() == elem );
    }

    // unit_test[190] search_vslist normal case 5(vs not found in udpmode) return value check
    {
        l7vs::virtualservice_element    elem;
        elem.udpmode = true;
        elem.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "60.60.60.60:4444" );
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
        BOOST_CHECK( itr == vsd_test.get_vslist().end() );
    }

    // unit_test[191] search_vslist normal case 6(vslist empty in udpmode) return value check
    vsd_test.get_vslist().clear();
    {
        l7vs::virtualservice_element    elem;
        elem.udpmode = true;
        elem.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>( "40.40.40.40:6666" );
        l7vs::l7vsd::vslist_type::iterator itr = vsd_test.search_vslist( elem );
        BOOST_CHECK( itr == vsd_test.get_vslist().end() );
    }

    BOOST_MESSAGE( "----- search_vslist_test end -----" );

}



void    run_test(){
    BOOST_MESSAGE( "----- run test_start -----" );
    l7vsd_test            vsd_test;

// normal case
    {
        l7vs::protocol_module_control::initialize_called = false;
        l7vs::protocol_module_control::finalize_called = false;
        l7vs::schedule_module_control::initialize_called = false;
        l7vs::schedule_module_control::finalize_called = false;
        l7vs::replication::initialize_called = false;
        l7vs::replication::initialize_fail = false;
        l7vs::replication::finalize_called = false;
        l7vs::snmpbridge::initialize_called = false;
        l7vs::snmpbridge::initialize_fail = false;
        l7vs::snmpbridge::finalize_called = false;

        daemon_ret = 0;
    
        int        argc    = 1;
        char*    argv[]    = { "l7vsd_test" };
    
        vsd_test.set_exit_requested( 1 );
        int ret = vsd_test.run( argc, argv );
        //boost::thread    thd( boost::bind( &l7vsd_test::run, &vsd_test, argc, argv ) );
        vsd_test.set_exit_requested( 0 );
    
        // unit_test[192] l7vsd::run normal case return value check
        BOOST_CHECK_EQUAL( ret, 0 );
        // unit_test[193] l7vsd::run normal case help mode check
        BOOST_CHECK_EQUAL( vsd_test.get_help(), false );
        // unit_test[194] l7vsd::run normal case debug mode check
        BOOST_CHECK_EQUAL( vsd_test.get_help(), false );
        vsd_test.get_help() = false;
        vsd_test.get_help() = false;

        // unit_test[195] l7vsd::run normal case protocol_module_control initialize call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_control::initialize_called, true );
        // unit_test[196] l7vsd::run normal case protocol_module_control finalize call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_control::finalize_called, true );
        // unit_test[197] l7vsd::run normal case schedule_module_control initilize call check
        BOOST_CHECK_EQUAL( l7vs::schedule_module_control::initialize_called, true );
        // unit_test[198] l7vsd::run normal case schedule_module_control finalize call check
        BOOST_CHECK_EQUAL( l7vs::schedule_module_control::finalize_called, true );
        // unit_test[199] l7vsd::run normal case replication initilalize call check
        BOOST_CHECK_EQUAL( l7vs::replication::initialize_called, true );
        // unit_test[200] l7vsd::run normal case replication finalize call check
        BOOST_CHECK_EQUAL( l7vs::replication::finalize_called, true );
        // unit_test[201] l7vsd::run normal case snmpbridge initilalize call check
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::initialize_called, true );
        // unit_test[202] l7vsd::run normal case snmpbridge finalize call check
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::finalize_called, true );

        l7vs::protocol_module_control::initialize_called = false;
        l7vs::protocol_module_control::finalize_called = false;
        l7vs::schedule_module_control::initialize_called = false;
        l7vs::schedule_module_control::finalize_called = false;
        l7vs::replication::initialize_called = false;
        l7vs::replication::initialize_fail = false;
        l7vs::replication::finalize_called = false;
        l7vs::snmpbridge::initialize_called = false;
        l7vs::snmpbridge::initialize_fail = false;
        l7vs::snmpbridge::finalize_called = false;
    }

// normal case 2(help mode )
    {
        int        argc    = 2;
        char*    argv[]    = { "l7vsd_test", "-h" };
    
        vsd_test.set_exit_requested( 1 );
        int ret = vsd_test.run( argc, argv );
        vsd_test.set_exit_requested( 0 );
    
        // unit_test[203] l7vsd::run normal case 2(help mode) return value check
        BOOST_CHECK_EQUAL( ret, 0 );
        // unit_test[204] l7vsd::run normal case 2(help mode) help mode check
        BOOST_CHECK_EQUAL( vsd_test.get_help(), true );
        // unit_test[205] l7vsd::run normal case 2(help mode) debug mode check
        BOOST_CHECK_EQUAL( vsd_test.get_debug(), false );
        vsd_test.get_help() = false;
        vsd_test.get_debug() = false;

        // unit_test[206] l7vsd::run normal case 2(help mode) protocol_module_control initialize call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_control::initialize_called, false );
        // unit_test[207] l7vsd::run normal case 2(help mode) protocol_module_control finalize call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_control::finalize_called, false );
        // unit_test[208] l7vsd::run normal case 2(help mode) schedule_module_control initilize call check
        BOOST_CHECK_EQUAL( l7vs::schedule_module_control::initialize_called, false );
        // unit_test[209] l7vsd::run normal case 2(help mode) schedule_module_control finalize call check
        BOOST_CHECK_EQUAL( l7vs::schedule_module_control::finalize_called, false );
        // unit_test[210] l7vsd::run normal case 2(help mode) replication initilalize call check
        BOOST_CHECK_EQUAL( l7vs::replication::initialize_called, false );
        // unit_test[211] l7vsd::run normal case 2(help mode) replication finalize call check
        BOOST_CHECK_EQUAL( l7vs::replication::finalize_called, false );
        // unit_test[212] l7vsd::run normal case 2(help mode) snmpbridge initilalize call check
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::initialize_called, false );
        // unit_test[213] l7vsd::run normal case 2(help mode) snmpbridge finalize call check
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::finalize_called, false );

        l7vs::protocol_module_control::initialize_called = false;
        l7vs::protocol_module_control::finalize_called = false;
        l7vs::schedule_module_control::initialize_called = false;
        l7vs::schedule_module_control::finalize_called = false;
        l7vs::replication::initialize_called = false;
        l7vs::replication::initialize_fail = false;
        l7vs::replication::finalize_called = false;
        l7vs::snmpbridge::initialize_called = false;
        l7vs::snmpbridge::initialize_fail = false;
        l7vs::snmpbridge::finalize_called = false;
    }

// normal case 3(debug mode)
    {
        int        argc    = 2;
        char*    argv[]    = { "l7vsd_test", "-d" };
    
        vsd_test.set_exit_requested( 1 );
        int ret = vsd_test.run( argc, argv );
        vsd_test.set_exit_requested( 0 );
    
        // unit_test[214] l7vsd::run normal case 3(debug mode) return value check
        BOOST_CHECK_EQUAL( ret, 0 );
        // unit_test[215] l7vsd::run normal case 3(debug mode) delp mode check
        BOOST_CHECK_EQUAL( vsd_test.get_help(), false );
        // unit_test[216] l7vsd::run normal case 3(debug mode) debug mode check
        BOOST_CHECK_EQUAL( vsd_test.get_debug(), true );
        vsd_test.get_help() = false;
        vsd_test.get_debug() = false;

        // unit_test[217] l7vsd::run normal case 3(debug mode) protocol_module_control initialize call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_control::initialize_called, true );
        // unit_test[218] l7vsd::run normal case 3(debug mode) protocol_module_control finalize call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_control::finalize_called, true );
        // unit_test[219] l7vsd::run normal case 3(debug mode) schedule_module_control initilize call check
        BOOST_CHECK_EQUAL( l7vs::schedule_module_control::initialize_called, true );
        // unit_test[220] l7vsd::run normal case 3(debug mode) schedule_module_control finalize call check
        BOOST_CHECK_EQUAL( l7vs::schedule_module_control::finalize_called, true );
        // unit_test[221] l7vsd::run normal case 3(debug mode) replication initilalize call check
        BOOST_CHECK_EQUAL( l7vs::replication::initialize_called, true );
        // unit_test[222] l7vsd::run normal case 3(debug mode) replication finalize call check
        BOOST_CHECK_EQUAL( l7vs::replication::finalize_called, true );
        // unit_test[223] l7vsd::run normal case 3(debug mode) snmpbridge initilalize call check
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::initialize_called, true );
        // unit_test[224] l7vsd::run normal case 3(debug mode) snmpbridge finalize call check
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::finalize_called, true );

        l7vs::protocol_module_control::initialize_called = false;
        l7vs::protocol_module_control::finalize_called = false;
        l7vs::schedule_module_control::initialize_called = false;
        l7vs::schedule_module_control::finalize_called = false;
        l7vs::replication::initialize_called = false;
        l7vs::replication::initialize_fail = false;
        l7vs::replication::finalize_called = false;
        l7vs::snmpbridge::initialize_called = false;
        l7vs::snmpbridge::initialize_fail = false;
        l7vs::snmpbridge::finalize_called = false;
    }

// normal case 4(help and debug mode)
    {
        int        argc    = 3;
        char*    argv[]    = { "l7vsd_test", "-d", "-h" };
    
        vsd_test.set_exit_requested( 1 );
        int ret = vsd_test.run( argc, argv );
        vsd_test.set_exit_requested( 0 );
    
        // unit_test[225] l7vsd::run normal case 4(help and debug mode) return value check
        BOOST_CHECK_EQUAL( ret, 0 );
        // unit_test[226] l7vsd::run normal case 4(help and debug mode) delp mode check
        BOOST_CHECK_EQUAL( vsd_test.get_help(), true );
        // unit_test[227] l7vsd::run normal case 4(help and debug mode) debug mode check
        BOOST_CHECK_EQUAL( vsd_test.get_debug(), true );
        vsd_test.get_help() = false;
        vsd_test.get_debug() = false;

        // unit_test[228] l7vsd::run normal case 4(help and debug mode) protocol_module_control initialize call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_control::initialize_called, false );
        // unit_test[229] l7vsd::run normal case 4(help and debug mode) protocol_module_control finalize call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_control::finalize_called, false );
        // unit_test[230] l7vsd::run normal case 4(help and debug mode) schedule_module_control initilize call check
        BOOST_CHECK_EQUAL( l7vs::schedule_module_control::initialize_called, false );
        // unit_test[231] l7vsd::run normal case 4(help and debug mode) schedule_module_control finalize call check
        BOOST_CHECK_EQUAL( l7vs::schedule_module_control::finalize_called, false );
        // unit_test[232] l7vsd::run normal case 4(help and debug mode) replication initilalize call check
        BOOST_CHECK_EQUAL( l7vs::replication::initialize_called, false );
        // unit_test[233] l7vsd::run normal case 4(help and debug mode) replication finalize call check
        BOOST_CHECK_EQUAL( l7vs::replication::finalize_called, false );
        // unit_test[234] l7vsd::run normal case 4(help and debug mode) snmpbridge initilalize call check
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::initialize_called, false );
        // unit_test[235] l7vsd::run normal case 4(help and debug mode) snmpbridge finalize call check
        BOOST_CHECK_EQUAL( l7vs::snmpbridge::finalize_called, false );

        l7vs::protocol_module_control::initialize_called = false;
        l7vs::protocol_module_control::finalize_called = false;
        l7vs::schedule_module_control::initialize_called = false;
        l7vs::schedule_module_control::finalize_called = false;
        l7vs::replication::initialize_called = false;
        l7vs::replication::initialize_fail = false;
        l7vs::replication::finalize_called = false;
        l7vs::snmpbridge::initialize_called = false;
        l7vs::snmpbridge::initialize_fail = false;
        l7vs::snmpbridge::finalize_called = false;
    }

    BOOST_MESSAGE( "----- run_test end -----" );
}

void    sig_exit_handler_test(){
    BOOST_MESSAGE( "----- sig_exit_handler test start -----" );

    int ret;
    sigset_t    newmask;
    sigset_t    oldmask;
    sigfillset( &newmask );
    ret = pthread_sigmask( SIG_BLOCK, &newmask, &oldmask );

    pthread_t    th;
    l7vsd_test            vsd_test;

    // sig_exit_handler SIGHUP
    vsd_test.set_exit_requested( 0 );
    vsd_test.set_received_sig( 0 );
    pthread_create( &th, NULL, handler_wrapper, &vsd_test );
    usleep( 10 );
    pthread_kill( th, SIGHUP );
    pthread_join( th, NULL );
    // unit_test[236] sig_exit_handler SIGHUP exit_requested check
    BOOST_CHECK_EQUAL( vsd_test.get_exit_requested(), 1 );
    // unit_test[237] sig_exit_handler SIGHUP received_sig check
    BOOST_CHECK_EQUAL( vsd_test.get_received_sig(), 1 );

    // sig_exit_handler SIGINT
    vsd_test.set_exit_requested( 0 );
    vsd_test.set_received_sig( 0 );
    pthread_create( &th, NULL, handler_wrapper, &vsd_test );
    usleep( 10 );
    pthread_kill( th, SIGINT );
    pthread_join( th, NULL );
    // unit_test[238] sig_exit_handler SIGINT exit_requested check
    BOOST_CHECK_EQUAL( vsd_test.get_exit_requested(), 1 );
    // unit_test[239] sig_exit_handler SIGINT received_sig check
    BOOST_CHECK_EQUAL( vsd_test.get_received_sig(), 2 );

    // sig_exit_handler SIGQUIT
    vsd_test.set_exit_requested( 0 );
    vsd_test.set_received_sig( 0 );
    pthread_create( &th, NULL, handler_wrapper, &vsd_test );
    usleep( 10 );
    pthread_kill( th, SIGQUIT );
    pthread_join( th, NULL );
    // unit_test[240] sig_exit_handler SIGQUIT exit_requested check
    BOOST_CHECK_EQUAL( vsd_test.get_exit_requested(), 1 );
    // unit_test[241] sig_exit_handler SIGQUIT received_sig check
    BOOST_CHECK_EQUAL( vsd_test.get_received_sig(), 3 );

    // sig_exit_handler SIGTERM
    vsd_test.set_exit_requested( 0 );
    vsd_test.set_received_sig( 0 );
    pthread_create( &th, NULL, handler_wrapper, &vsd_test );
    usleep( 10 );
    pthread_kill( th, SIGTERM );
    pthread_join( th, NULL );
    // unit_test[242] sig_exit_handler SIGQUIT exit_requested check
    BOOST_CHECK_EQUAL( vsd_test.get_exit_requested(), 1 );
    // unit_test[243] sig_exit_handler SIGQUIT received_sig check
    BOOST_CHECK_EQUAL( vsd_test.get_received_sig(), 15 );

    // sig_exit_handler SIGUSR1
    vsd_test.set_exit_requested( 0 );
    vsd_test.set_received_sig( 0 );
    pthread_create( &th, NULL, handler_wrapper, &vsd_test );
    usleep( 10 );
    pthread_kill( th, SIGUSR1 );
    usleep( 10 );
    // unit_test[244] sig_exit_handler SIGUSR1 exit_requested check
    BOOST_CHECK_EQUAL( vsd_test.get_exit_requested(), 0 );
    // unit_test[245] sig_exit_handler SIGUSR1 received_sig check
    BOOST_CHECK_EQUAL( vsd_test.get_received_sig(), 0 );

    pthread_kill( th, SIGINT );
    pthread_join( th, NULL );
    vsd_test.set_exit_requested( 0 );
    vsd_test.set_received_sig( 0 );

    ret = pthread_sigmask( SIG_SETMASK, &oldmask, NULL );

    BOOST_MESSAGE( "----- sig_exit_handler test end -----" );
}

void*    handler_wrapper( void* args ){
    l7vsd_test* vsd_test = (l7vsd_test*)args;
    vsd_test->sig_exit_handler_wp();
    return NULL;
}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){
    //glibc function alias
    org_daemon = (int (*)(int, int)) dlsym(RTLD_NEXT, "daemon");

    test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

    l7vs::Logger    logger_instance;
    l7vs::Parameter    parameter_instance;
    logger_instance.loadConf();

    ts->add( BOOST_TEST_CASE( &list_virtual_service_test ) );
    ts->add( BOOST_TEST_CASE( &list_virtual_service_verbose_test ) );
    ts->add( BOOST_TEST_CASE( &search_vslist_test ) );
    ts->add( BOOST_TEST_CASE( &add_virtual_service_test ) );
    ts->add( BOOST_TEST_CASE( &del_virtual_service_test ) );
    ts->add( BOOST_TEST_CASE( &edit_virtual_service_test ) );
    ts->add( BOOST_TEST_CASE( &add_real_server_test ) );
    ts->add( BOOST_TEST_CASE( &del_real_server_test ) );
    ts->add( BOOST_TEST_CASE( &edit_real_server_test ) );
    ts->add( BOOST_TEST_CASE( &flush_virtual_service_test ) );

    ts->add( BOOST_TEST_CASE( &replication_command_test ) );
    ts->add( BOOST_TEST_CASE( &set_loglevel_test ) );
    ts->add( BOOST_TEST_CASE( &set_snmp_loglevel_test) );
    ts->add( BOOST_TEST_CASE( &reload_parameter_test ) );

    ts->add( BOOST_TEST_CASE( &run_test ) );

    ts->add( BOOST_TEST_CASE( &sig_exit_handler_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}
