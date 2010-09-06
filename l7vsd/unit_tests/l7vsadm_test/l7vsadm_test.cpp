#define    UNIT_TEST

#include <boost/test/included/unit_test.hpp>
#include <boost/algorithm/string.hpp>

//#include "error_code.h"

#include "logger.h"
#include "parameter.h"

#include "protocol_module_stub.h"
#include "schedule_module_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"
#include "logger_access_manager_stub.h"

#include "../../src/l7vsadm.cpp"

#ifndef    COMMAND_BUFFER_SIZE
    #define    COMMAND_BUFFER_SIZE (65535)
#endif
#ifndef    L7VS_CONFIG_SOCKNAME
    #define L7VS_CONFIG_SOCKNAME        "./l7vs"
#endif

using namespace boost::unit_test_framework;


// test class
class    l7vsadm_test    :public    l7vs::l7vsadm {
public:

    l7vs::l7vsadm_request&        get_request()    { return request; }
    l7vs::l7vsd_response&        get_response()    { return response; }

    l7vs::error_code&    get_error()    { return l7vsadm_err; }

    bool&    get_numeric()    { return numeric_flag; }

    bool    parse_list_func_wp( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] )
    { return parse_list_func( cmd, argc, argv ); }

    bool    parse_opt_vs_target_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_target_func( pos, argc, argv ); }

    bool    parse_opt_vs_module_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_module_func( pos, argc, argv ); }

    bool    parse_opt_vs_scheduler_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_scheduler_func( pos, argc, argv ); }

    bool    parse_opt_vs_upper_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_upper_func( pos, argc, argv ); }

    bool    parse_opt_vs_bypass_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_bypass_func( pos, argc, argv ); }

    bool    parse_opt_vs_flag_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_flag_func( pos, argc, argv ); }

    bool    parse_opt_vs_qosup_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_qosup_func( pos, argc, argv ); }

    bool    parse_opt_vs_qosdown_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_qosdown_func( pos, argc, argv ); }

    bool    parse_opt_vs_udp_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_udp_func( pos, argc, argv ); }

    bool    parse_opt_vs_ssl_file_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_ssl_file_func( pos, argc, argv ); }

    bool    parse_opt_vs_access_log_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_access_log_func( pos, argc, argv ); }

    bool    parse_opt_vs_access_log_logrotate_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_access_log_logrotate_func( pos, argc, argv ); }

    bool    parse_opt_vs_socket_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_socket_func( pos, argc, argv ); }

    bool    parse_vs_func_wp( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] )
    { return parse_vs_func( cmd, argc, argv ); }

    bool    parse_opt_rs_weight_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_rs_weight_func( pos, argc, argv ); }

    bool    parse_opt_rs_realserver_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_rs_realserver_func( pos, argc, argv ); }

    bool    parse_rs_func_wp( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] )
    { return parse_rs_func( cmd, argc, argv ); }

    bool    parse_opt_replication_switch_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_replication_switch_func( pos, argc, argv ); }

    bool    parse_opt_replication_force_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_replication_force_func( pos, argc, argv ); }

    bool    parse_opt_replication_dump_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_replication_dump_func( pos, argc, argv ); }

    bool    parse_replication_func_wp( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] )
    { return parse_replication_func( cmd, argc, argv ); }

    bool    parse_opt_log_category_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_log_category_func( pos, argc, argv ); }

    bool    parse_opt_log_level_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_log_level_func( pos, argc, argv ); }

    bool    parse_log_func_wp( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] )
    { return parse_log_func( cmd, argc, argv ); }

    bool    parse_opt_snmp_log_category_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_snmp_log_category_func( pos, argc, argv ); }

    bool    parse_opt_snmp_log_level_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_snmp_log_level_func( pos, argc, argv ); }

    bool    parse_snmp_func_wp( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] )
    { return parse_snmp_func( cmd, argc, argv ); }

    bool    parse_opt_parameter_reload_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_parameter_reload_func( pos, argc, argv ); }

    bool    parse_parameter_func_wp( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char* argv[] )
    { return parse_parameter_func( cmd, argc, argv ); }

    bool    execute_wp( int argc, char* argv[] )
    { return execute( argc, argv ); }

    bool    parse_opt_vs_fwdmode_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_vs_fwdmode_func( pos, argc, argv ); }

    bool    parse_opt_rs_fwdmode_func_wp( int& pos, int argc, char* argv[] )
    { return parse_opt_rs_fwdmode_func( pos, argc, argv ); }

    static void file_lock_class_test();

};

//variables
//boost::mutex        accept_mutex;
//boost::condition    accept_condition;
volatile    bool    accept_ready;

l7vs::l7vsadm_request        test_request;
l7vs::l7vsd_response        test_response;

//functions
void    server_thread(){
    using boost::asio::local::stream_protocol;

    boost::array< char,COMMAND_BUFFER_SIZE >    buf;
    buf.assign( 0x00 );

    // accept
    std::cout << "sock:" << L7VS_CONFIG_SOCKNAME << std::endl;
    boost::asio::io_service        server_io;
    stream_protocol::acceptor    acc(    server_io,
                                        stream_protocol::endpoint( L7VS_CONFIG_SOCKNAME ) );
    stream_protocol::socket    s( server_io );

    // ready to accept
    accept_ready = true;
    //accept_condition.notify_all();
    //std::cout << "accept_ready" << std::endl;

    acc.accept( s );
    //std::cout << "accepted" << std::endl;

    // recv request
    s.read_some( boost::asio::buffer( buf ) );
    //std::cout << "read_done" << std::endl;
    
    std::stringstream    recv_stream;
    recv_stream << &(buf[0]);
    boost::archive::text_iarchive    ia( recv_stream );
    ia >> test_request;

    std::stringstream    send_stream;
    boost::archive::text_oarchive    oa( send_stream );
    oa << (const l7vs::l7vsd_response&) test_response;

    // send response
    boost::asio::write( s, boost::asio::buffer( send_stream.str() ) );
    //std::cout << "write_done" << std::endl;

    s.close();

    unlink( L7VS_CONFIG_SOCKNAME );
    //std::cout << "unlink" << std::endl;
}

//util
template< typename InternetProtocol >
boost::asio::ip::basic_endpoint< InternetProtocol > string_to_endpoint( std::string str ){
    std::string::size_type pos = str.rfind( ":" );
    std::string    hostname = str.substr( 0, pos );
    std::string    portname = str.substr( pos+1, str.length() );
    //remove "[","]"
    boost::algorithm::erase_first( hostname, "[" );
    boost::algorithm::erase_last( hostname, "]" );

    boost::asio::io_service        io_service;
    typename InternetProtocol::resolver                resolver(io_service);
    typename InternetProtocol::resolver::query        query( hostname, portname );
    typename InternetProtocol::resolver::iterator    end;
    typename InternetProtocol::resolver::iterator    itr = resolver.resolve( query );
    if( itr == end ) return typename InternetProtocol::endpoint();
    return *itr;
}

//--tests--
void    parse_list_func_test(){
    BOOST_MESSAGE( "----- parse_list_func_test start -----" );

    // parse_list_func normal case 1 (no option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_LIST;
        int        argc    = 2;
        char*    argv[]    = { "l7vsadm_test", "-l" };
    
        bool ret = adm.parse_list_func_wp( cmd, argc, argv );
    
        // unit_test[1] parse_list_func normal case 1 (no option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[2] parse_list_func normal case 1 (no option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_LIST );
    }

    // parse_list_func normal case 2 (with num option short)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_LIST;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-l", "-n" };
    
        bool ret = adm.parse_list_func_wp( cmd, argc, argv );

        // unit_test[3] parse_list_func normal case 2 (with num option short) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[4] parse_list_func normal case 2 (with num option short) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_LIST );
        // unit_test[5] parse_list_func normal case 2 (with num option short) nummeric flag check
        BOOST_CHECK_EQUAL( adm.get_numeric(), true );
    }

    // parse_list_func normal case 3 (with num option long)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_LIST;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-l", "--numeric" };
    
        bool ret = adm.parse_list_func_wp( cmd, argc, argv );

        // unit_test[6] parse_list_func normal case 3 (with num option long) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[7] parse_list_func normal case 3 (with num option long) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_LIST );
        // unit_test[8] parse_list_func normal case 3 (with num option long) nummeric flag check
        BOOST_CHECK_EQUAL( adm.get_numeric(), true );
    }

    // parse_list_func error case 1 (invarid option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_LIST;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-l", "-z" };
    
        bool ret = adm.parse_list_func_wp( cmd, argc, argv );

        // unit_test[9] parse_list_func error case 1 (invarid option) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_list_func_test end -----" );

}

void    parse_opt_vs_target_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_target_func_test start -----" );

    // parse_opt_vs_target_func normal case 1 (tcp endpoint)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "10.144.169.87:22100" };
    
        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[10] parse_opt_vs_target_func normal case 1 (tcp endpoint) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[11] parse_opt_vs_target_func normal case 1 (tcp endpoint) endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, ep );
    }

    // parse_opt_vs_target_func normal case 2 (udp endpoint)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "10.144.169.87:22100" };
        adm.get_request().vs_element.udpmode = true;

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[12] parse_opt_vs_target_func normal case 2 (udp endpoint) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[13] parse_opt_vs_target_func normal case 2 (udp endpoint) endpoint check
        boost::asio::ip::udp::endpoint    ep = string_to_endpoint<boost::asio::ip::udp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.udp_recv_endpoint, ep );
    }

    // parse_opt_vs_target_func normal case 3 (tcp endpoint specified in name)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "127.0.0.1:pop3" };
    
        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[14] parse_opt_vs_target_func normal case 3 (tcp endpoint specified in name) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[15] parse_opt_vs_target_func normal case 3 (tcp endpoint specified in name) endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "127.0.0.1:110" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, ep );
    }

// ramiel_ipv6 add
    // parse_opt_vs_target_func normal case 4 (any address)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "0.0.0.0:8080" };

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_target_func normal case 4 (any address) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_target_func normal case 4 (any address) endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "0.0.0.0:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, ep );
    }

// ramiel_ipv6 add
    // parse_opt_vs_target_func normal case 5 (tcp endpoint(ipv6))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "[2001::11]:8080" };

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_target_func normal case 4 (tcp endpoint(ipv6)) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_target_func normal case 4 (tcp endpoint(ipv6)) endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "[2001::11]:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, ep );
    }

// ramiel_ipv6 add
    // parse_opt_vs_target_func normal case 6 (udp endpoint(ipv6))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "[2001::11]:8080" };
        adm.get_request().vs_element.udpmode = true;

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_target_func normal case 6 (udp endpoint(ipv6)) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_target_func normal case 6 (udp endpoint(ipv6)) endpoint check
        boost::asio::ip::udp::endpoint    ep = string_to_endpoint<boost::asio::ip::udp>( "[2001::11]:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.udp_recv_endpoint, ep );
    }

// ramiel_ipv6 add
    // parse_opt_vs_target_func normal case 5 (any address(ipv6))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "[::]:8080" };

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_target_func normal case 4 (any address(ipv6)) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_target_func normal case 4 (any address(ipv6)) endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "[::]:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, ep );
    }

    // parse_opt_vs_target_func error case 1 (invalid endpoint)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "10.144.169.257:22100" };

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[16] parse_opt_vs_target_func error case 1 (invalid endpoint) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_target_func error case 2 (no endpoint)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t" };

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[17] parse_opt_vs_target_func error case 2 (no endpoint) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

// ramiel_ipv6 delete
//    // parse_opt_vs_target_func error case 3 (0.0.0.0 address)
//    {
//        l7vsadm_test    adm;
//        int        pos        = 2;
//        int        argc    = 4;
//        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "0.0.0.0:8080" };
//
//        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );
//
//        // unit_test[18] parse_opt_vs_target_func error case 3 (0.0.0.0 address) return value check
//        BOOST_CHECK_EQUAL( ret, false );    
//    }

    // parse_opt_vs_target_func error case 4 (0 port)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "10.144.169.87:0" };

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[19] parse_opt_vs_target_func error case 4 (0 port) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_target_func error case 5 (invalid port (99999))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "10.144.169.87:99999" };

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[20] parse_opt_vs_target_func error case 5 (invalid port (99999)) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_target_func error case 6 (hostname omitted)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", ":22100" };

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[21] parse_opt_vs_target_func error case 6 (hostname omitted) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_target_func error case 7 (port omitted)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-t", "10.144.169.87:" };

        bool ret = adm.parse_opt_vs_target_func_wp( pos, argc, argv );

        // unit_test[22] parse_opt_vs_target_func error case 7 (port omitted) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_target_func_test end -----" );

}

void    parse_opt_vs_module_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_module_func_test start -----" );

    // parse_opt_vs_module_func normal case 1 (end with module option)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 5;
        char*    argv[]    = { "l7vsadm_test", "-A", "-m", "cinsert", "option" };
        l7vs::protocol_module_control::load_module_fail = false;
        l7vs::protocol_module_stub::check_parameter_called = false;
        l7vs::protocol_module_stub::check_parameter_fail = false;
    
        bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

        // unit_test[23] parse_opt_vs_module_func normal case 1 (end with module option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[24] parse_opt_vs_module_func normal case 1 (end with module option) check_parameter call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_stub::check_parameter_called, true );
        // unit_test[25] parse_opt_vs_module_func normal case 1 (end with module option) module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[26] parse_opt_vs_module_func normal case 1 (end with module option) module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "option" );
    }

    // parse_opt_vs_module_func normal case 2 (end with next vs option)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 6;
        char*    argv[]    = { "l7vsadm_test", "-A", "-m", "cinsert", "option", "-t" };
        l7vs::protocol_module_control::load_module_fail = false;
        l7vs::protocol_module_stub::check_parameter_called = false;
        l7vs::protocol_module_stub::check_parameter_fail = false;
    
        bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

        // unit_test[27] parse_opt_vs_module_func normal case 2 (end with next vs option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[28] parse_opt_vs_module_func normal case 2 (end with next vs option) check_parameter call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_stub::check_parameter_called, true );
        // unit_test[29] parse_opt_vs_module_func normal case 2 (end with next vs option) module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[30] parse_opt_vs_module_func normal case 2 (end with next vs option) module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "option" );

    }

    // parse_opt_vs_module_func normal case 3 (no protocol module arg)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 5;
        char*    argv[]    = { "l7vsadm_test", "-A", "-m", "cinsert", "-t" };
        l7vs::protocol_module_control::load_module_fail = false;
        l7vs::protocol_module_stub::check_parameter_called = false;
        l7vs::protocol_module_stub::check_parameter_fail = false;
    
        bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

        // unit_test[31] parse_opt_vs_module_func normal case 3 (no protocol module arg) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[32] parse_opt_vs_module_func normal case 3 (no protocol module arg) check_parameter call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_stub::check_parameter_called, true );
        // unit_test[33] parse_opt_vs_module_func normal case 3 (no protocol module arg) module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[34] parse_opt_vs_module_func normal case 3 (no protocol module arg) module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.size(), 0U );

    }

    // parse_opt_vs_module_func error case 1 (no protocol module name)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-m" };
        l7vs::protocol_module_control::load_module_fail = false;
        l7vs::protocol_module_stub::check_parameter_called = false;
        l7vs::protocol_module_stub::check_parameter_fail = false;
    
        bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

        // unit_test[35] parse_opt_vs_module_func error case 1 (no protocol module name) return value check
        BOOST_CHECK_EQUAL( ret, false );    

    }

    // parse_opt_vs_module_func error case 2 (protocol module load failed)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 5;
        char*    argv[]    = { "l7vsadm_test", "-A", "-m", "cinsert", "-t" };
        l7vs::protocol_module_control::load_module_fail = true;
        l7vs::protocol_module_stub::check_parameter_called = false;
        l7vs::protocol_module_stub::check_parameter_fail = false;
    
        bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

        // unit_test[36] parse_opt_vs_module_func error case 2 (protocol module load failed) return value check
        BOOST_CHECK_EQUAL( ret, false );    

    }

    // parse_opt_vs_module_func error case 3 (check parameter failed)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 5;
        char*    argv[]    = { "l7vsadm_test", "-A", "-m", "cinsert", "-t" };
        l7vs::protocol_module_control::load_module_fail = false;
        l7vs::protocol_module_stub::check_parameter_called = false;
        l7vs::protocol_module_stub::check_parameter_fail = true;
    
        bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

        // unit_test[37] parse_opt_vs_module_func error case 3 (check parameter failed) return value check
        BOOST_CHECK_EQUAL( ret, false );    
        // unit_test[38] parse_opt_vs_module_func error case 3 (check parameter failed) check_parameter call check
        BOOST_CHECK_EQUAL( l7vs::protocol_module_stub::check_parameter_called, true );

    }

    // parse_opt_vs_module_func error case 4 (protomod name too long)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 5;
        char*    argv[]    = { "l7vsadm_test", "-A", "-m", "protomodnameistoolong", "-t" };
        l7vs::protocol_module_control::load_module_fail = false;
        l7vs::protocol_module_stub::check_parameter_called = false;
        l7vs::protocol_module_stub::check_parameter_fail = false;
    
        bool ret = adm.parse_opt_vs_module_func_wp( pos, argc, argv );

        // unit_test[39] parse_opt_vs_module_func error case 4 (protomod name too long) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    l7vs::protocol_module_control::load_module_fail = false;
    l7vs::protocol_module_stub::check_parameter_called = false;
    l7vs::protocol_module_stub::check_parameter_fail = false;

    BOOST_MESSAGE( "----- parse_opt_vs_module_func_test end -----" );

}

void    parse_opt_vs_scheduler_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_scheduler_func_test start -----" );

    // parse_opt_vs_scheduler_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-s", "rr" };
        l7vs::schedule_module_control::load_module_fail = false;
    
        bool ret = adm.parse_opt_vs_scheduler_func_wp( pos, argc, argv );

        // unit_test[40] parse_opt_vs_scheduler_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[41] parse_opt_vs_scheduler_func normal case 1 module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "rr" );
    }

    // parse_opt_vs_scheduler_func error case 1 (no schedule module name)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-s" };
        l7vs::schedule_module_control::load_module_fail = false;
    
        bool ret = adm.parse_opt_vs_scheduler_func_wp( pos, argc, argv );

        // unit_test[42] parse_opt_vs_scheduler_func error case 1 (no schedule module name) return value check
        BOOST_CHECK_EQUAL( ret, false );    

    }

    // parse_opt_vs_scheduler_func error case 2 (schedule module load failed)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-s", "rr" };
        l7vs::schedule_module_control::load_module_fail = true;
    
        bool ret = adm.parse_opt_vs_scheduler_func_wp( pos, argc, argv );

        // unit_test[43] parse_opt_vs_scheduler_func error case 2 (schedule module load failed) return value check
        BOOST_CHECK_EQUAL( ret, false );    

    }

    // parse_opt_vs_scheduler_func error case 3 (schedule module name too long)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-s", "schedulernameistoolong" };
        l7vs::schedule_module_control::load_module_fail = false;
    
        bool ret = adm.parse_opt_vs_scheduler_func_wp( pos, argc, argv );

        // unit_test[44] parse_opt_vs_scheduler_func error case 3 (schedule module name too long) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    l7vs::schedule_module_control::load_module_fail = false;

    BOOST_MESSAGE( "----- parse_opt_vs_scheduler_func_test end -----" );

}

void    parse_opt_vs_upper_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_upper_func_test start -----" );

    // parse_opt_vs_upper_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-u", "128" };
    
        bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

        // unit_test[45] parse_opt_vs_upper_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[46] parse_opt_vs_upper_func normal case 1 sorry_maxconnection check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 128 );
    }

    // parse_opt_vs_upper_func normal case 2(0 clear)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-u", "0" };
    
        bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

        // unit_test[47] parse_opt_vs_upper_func normal case 2(0 clear) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[48] parse_opt_vs_upper_func normal case 2(0 clear) sorry_maxconnection check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, LLONG_MAX );
    }

    // parse_opt_vs_upper_func error case 1 (invalid maxconnection value (charactor))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-u", "a" };
    
        bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

        // unit_test[49] parse_opt_vs_upper_func error case 1 (invalid maxconnection value (charactor)) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_upper_func error case 2 (invalid maxconnection value(long long over))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << (1ULL) + LLONG_MAX;
        char*    argv[]    = { "l7vsadm_test", "-A", "-u", const_cast<char*>( ss.str().c_str() ) };
    
        bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

        // unit_test[50] parse_opt_vs_upper_func error case 2 (invalid maxconnection value(long long over)) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_upper_func error case 3 (no maxconnection value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-u" };
    
        bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

        // unit_test[51] parse_opt_vs_upper_func error case 3 (no maxconnection value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_upper_func error case 4 (under 0)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-u", "-1" };
    
        bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

        // unit_test[52] parse_opt_vs_upper_func error case 4 (under 0) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_upper_func error case 5 (over 100000)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-u", "100001" };
    
        bool ret = adm.parse_opt_vs_upper_func_wp( pos, argc, argv );

        // unit_test[53] parse_opt_vs_upper_func error case 5 (over 100000) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_upper_func_test end -----" );

}

void    parse_opt_vs_bypass_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_bypass_func_test start -----" );

    // parse_opt_vs_bypass_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "10.144.169.87:8080" };
    
        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[54] parse_opt_vs_bypass_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[55] parse_opt_vs_bypass_func normal case 1 sorry_endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, ep );
    }

    // parse_opt_vs_bypass_func normal case 2 (sorryserver_clear)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "0.0.0.0:0" };
    
        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[56] parse_opt_vs_bypass_func normal case 2 (sorryserver_clear) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[57] parse_opt_vs_bypass_func normal case 2 (sorryserver_clear) sorry_endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "255.255.255.255:0" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, ep );
    }

    // parse_opt_vs_bypass_func normal case 3 (sorryserver specified in name)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "127.0.0.1:http" };
    
        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[58] parse_opt_vs_bypass_func normal case 3 (sorryserver specified in name) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[59] parse_opt_vs_bypass_func normal case 3 (sorryserver specified in name) endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "127.0.0.1:80" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, ep );
    }

//ramiel_ipv6 add
    // parse_opt_vs_bypass_func normal case 4 (ipv6)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "[2001::10]:8080" };
    
        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_bypass_func normal case 4 (ipv6) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_bypass_func normal case 4 (ipv6) sorry_endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "[2001::10]:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, ep );
    }

    // parse_opt_vs_bypass_func error case 1 (invalid endpoint)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "10.144.169.257:8080" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[60] parse_opt_vs_bypass_func error case 1 (invalid endpoint) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_bypass_func error case 2 (no endpoint)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[61] parse_opt_vs_bypass_func error case 2 (no endpoint) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_bypass_func error case 3 (0.0.0.0 address)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "0.0.0.0:8080" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[62] parse_opt_vs_bypass_func error case 3 (0.0.0.0 address) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_bypass_func error case 4 (0 port)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "10.144.169.87:0" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[63] parse_opt_vs_bypass_func error case 4 (0 port) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_bypass_func error case 5 (invalid port (99999))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "10.144.169.87:99999" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[64] parse_opt_vs_bypass_func error case 5 (invalid port (99999)) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_bypass_func error case 6 (hostname omitted)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", ":22100" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[65] parse_opt_vs_bypass_func error case 6 (hostname omitted) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_bypass_func error case 7 (port omitted)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "10.144.169.87:" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[66] parse_opt_vs_bypass_func error case 7 (port omitted) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

//ramiel_ipv6 add
    // parse_opt_vs_bypass_func error case 8 (ipv6 any address)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-b", "[::]:8080" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_bypass_func error case 8 (ipv6 any address) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_bypass_func_test end -----" );

}

void    parse_opt_vs_flag_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_flag_func_test start -----" );

    // parse_opt_vs_flag_func normal case 1 (flag on)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-f", "1" };
    
        bool ret = adm.parse_opt_vs_flag_func_wp( pos, argc, argv );

        // unit_test[67] parse_opt_vs_flag_func normal case 1 (flag on) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[68] parse_opt_vs_flag_func normal case 1 (flag on) sorry_flag check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_flag, 1 );
    }

    // parse_opt_vs_flag_func normal case 2 (flag off)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-f", "0" };
    
        bool ret = adm.parse_opt_vs_flag_func_wp( pos, argc, argv );

        // unit_test[69] parse_opt_vs_flag_func normal case 2 (flag off) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[70] parse_opt_vs_flag_func normal case 2 (flag off) sorry_flag check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_flag, INT_MAX );
    }

    // parse_opt_vs_flag_func error case 1 (no flag)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-f" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[71] parse_opt_vs_flag_func error case 1 (no flag) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_flag_func error case 2 (1 nor 0)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-f", "2" };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[72] parse_opt_vs_flag_func error case 2 (1 nor 0) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_flag_func error case 3 (int over)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << (1ULL) + INT_MAX;
        char*    argv[]    = { "l7vsadm_test", "-A", "-f", const_cast<char*>( ss.str().c_str() ) };

        bool ret = adm.parse_opt_vs_bypass_func_wp( pos, argc, argv );

        // unit_test[73] parse_opt_vs_flag_func error case 3 (int over) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_flag_func_test end -----" );

}

void    parse_opt_vs_qosup_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_qosup_func_test start -----" );

    // parse_opt_vs_qosup_func normal case 1 (no unit postfix)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128" };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[74] parse_opt_vs_qosup_func normal case 1 (no unit postfix) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[75] parse_opt_vs_qosup_func normal case 1 (no unit postfix) qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 128ULL / 8 ) );
    }

    // parse_opt_vs_qosup_func normal case 2 (unit postfix 'G')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128G" };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[76] parse_opt_vs_qosup_func normal case 2 (unit postfix 'G') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[77] parse_opt_vs_qosup_func normal case 2 (unit postfix 'G') qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 128000000000ULL / 8 ) );
    }

    // parse_opt_vs_qosup_func normal case 3 (unit postfix 'g')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128g" };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[78] parse_opt_vs_qosup_func normal case 3 (unit postfix 'g') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[79] parse_opt_vs_qosup_func normal case 3 (unit postfix 'g') qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 128000000000ULL / 8 ) );
    }

    // parse_opt_vs_qosup_func normal case 4 (unit postfix 'M')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128M" };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[80] parse_opt_vs_qosup_func normal case 4 (unit postfix 'M') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[81] parse_opt_vs_qosup_func normal case 4 (unit postfix 'M') qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 128000000ULL / 8 ) );
    }

    // parse_opt_vs_qosup_func normal case 5 (unit postfix 'm')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128m" };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[82] parse_opt_vs_qosup_func normal case 5 (unit postfix 'm') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[83] parse_opt_vs_qosup_func normal case 5 (unit postfix 'm') qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 128000000ULL / 8 ) );
    }

    // parse_opt_vs_qosup_func normal case 6 (unit postfix 'K')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128K" };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[84] parse_opt_vs_qosup_func normal case 6 (unit postfix 'K') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[85] parse_opt_vs_qosup_func normal case 6 (unit postfix 'K') qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 128000ULL / 8 ) );
    }

    // parse_opt_vs_qosup_func normal case 7 (unit postfix 'k')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128k" };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[86] parse_opt_vs_qosup_func normal case 7 (unit postfix 'k') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[87] parse_opt_vs_qosup_func normal case 7 (unit postfix 'k') qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 128000ULL / 8 ) );
    }

    // parse_opt_vs_qosup_func error case 1 (invalid qos_upstream value (charactor))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "a" };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[88] parse_opt_vs_qosup_func error case 1 (invalid qos_upstream value (charactor)) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosup_func error case 2 (invalid qos_upstream value(unsigned long long over))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << "18446744073709551616";    // ULLONG_MAX is 18446744073709551615
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[89] parse_opt_vs_qosup_func error case 2 (invalid qos_upstream value(unsigned long long over)) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosup_func error case 3 (invalid qos_upstream value(unsigned long long over in postfix 'G'))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << "17179869184G";    // ULLONG_MAX / 1024 / 1024 / 1024
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[90] parse_opt_vs_qosup_func error case 3 (invalid qos_upstream value(unsigned long long over in postfix 'G')) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosup_func error case 4 (invalid qos_upstream value(unsigned long long over in postfix 'M'))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << "17592186044416M";    // ULLONG_MAX / 1024 / 1024
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[91] parse_opt_vs_qosup_func error case 4 (invalid qos_upstream value(unsigned long long over in postfix 'M')) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosup_func error case 5 (invalid qos_upstream value(unsigned long long over in postfix 'K'))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << "18014398509481984K";    // ULLONG_MAX / 1024
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[92] parse_opt_vs_qosup_func error case 5 (invalid qos_upstream value(unsigned long long over in postfix 'K')) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosup_func error case 6 (no qos_upstream value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q" };
    
        bool ret = adm.parse_opt_vs_qosup_func_wp( pos, argc, argv );

        // unit_test[93] parse_opt_vs_qosup_func error case 6 (no qos_upstream value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_qosup_func_test end -----" );

}

void    parse_opt_vs_qosdown_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_qosdown_func_test start -----" );

    // parse_opt_vs_qosdown_func normal case 1 (no unit postfix)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128" };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[94] parse_opt_vs_qosdown_func normal case 1 (no unit postfix) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[95] parse_opt_vs_qosdown_func normal case 1 (no unit postfix) qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 128ULL / 8 ) );
    }

    // parse_opt_vs_qosdown_func normal case 2 (unit postfix 'G')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128G" };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[96] parse_opt_vs_qosdown_func normal case 2 (unit postfix 'G') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[97] parse_opt_vs_qosdown_func normal case 2 (unit postfix 'G') qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 128000000000ULL / 8 ) );
    }

    // parse_opt_vs_qosdown_func normal case 3 (unit postfix 'g')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128g" };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[98] parse_opt_vs_qosdown_func normal case 3 (unit postfix 'g') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[99] parse_opt_vs_qosdown_func normal case 3 (unit postfix 'g') qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 128000000000ULL / 8 ) );
    }

    // parse_opt_vs_qosdown_func normal case 4 (unit postfix 'M')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128M" };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[100] parse_opt_vs_qosdown_func normal case 4 (unit postfix 'M') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[101] parse_opt_vs_qosdown_func normal case 4 (unit postfix 'M') qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 128000000ULL / 8 ) );
    }

    // parse_opt_vs_qosdown_func normal case 5 (unit postfix 'm')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128m" };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[102] parse_opt_vs_qosdown_func normal case 5 (unit postfix 'm') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[103] parse_opt_vs_qosdown_func normal case 5 (unit postfix 'm') qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 128000000ULL / 8 ) );
    }

    // parse_opt_vs_qosdown_func normal case 6 (unit postfix 'K')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128K" };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[104] parse_opt_vs_qosdown_func normal case 6 (unit postfix 'K') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[105] parse_opt_vs_qosdown_func normal case 6 (unit postfix 'K') qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 128000ULL / 8 ) );
    }

    // parse_opt_vs_qosdown_func normal case 7 (unit postfix 'k')
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "128k" };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[106] parse_opt_vs_qosdown_func normal case 7 (unit postfix 'k') return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[107] parse_opt_vs_qosdown_func normal case 7 (unit postfix 'k') qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 128000ULL / 8 ) );
    }

    // parse_opt_vs_qosdown_func error case 1 (invalid qos_downstream value (charactor))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", "a" };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[108] parse_opt_vs_qosdown_func error case 1 (invalid qos_upstream value (charactor)) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosdown_func error case 2 (invalid qos_downstream value(unsigned long long over))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << "18446744073709551616";    // ULLONG_MAX is 18446744073709551615
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[109] parse_opt_vs_qosdown_func error case 2 (invalid qos_downstream value(unsigned long long over)) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosdown_func error case 3 (invalid qos_downstream value(unsigned long long over in postfix 'G'))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << "17179869184G";    // ULLONG_MAX / 1024 / 1024 / 1024
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[110] parse_opt_vs_qosdown_func error case 3 (invalid qos_downstream value(unsigned long long over in postfix 'G')) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosdown_func error case 4 (invalid qos_downstream value(unsigned long long over in postfix 'M'))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << "17592186044416M";    // ULLONG_MAX / 1024 / 1024
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[111] parse_opt_vs_qosdown_func error case 4 (invalid qos_downstream value(unsigned long long over in postfix 'M')) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosdown_func error case 5 (invalid qos_downstream value(unsigned long long over in postfix 'K'))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << "18014398509481984K";    // ULLONG_MAX / 1024
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q", const_cast<char*>( ss.str().c_str() ) };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[112] parse_opt_vs_qosdown_func error case 5 (invalid qos_downstream value(unsigned long long over in postfix 'K')) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_vs_qosdown_func error case 6 (no qos_upstream value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-Q" };
    
        bool ret = adm.parse_opt_vs_qosdown_func_wp( pos, argc, argv );

        // unit_test[113] parse_opt_vs_qosdown_func error case 6 (no qos_downstream value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_qosdown_func_test end -----" );

}

void    parse_opt_vs_udp_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_udp_func_test start -----" );

    // parse_opt_vs_udp_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-p" };
    
        bool ret = adm.parse_opt_vs_udp_func_wp( pos, argc, argv );

        // unit_test[114] parse_opt_vs_udp_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[115] parse_opt_vs_udp_func normal case 1 udp_mode check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.udpmode, true );
    }

    // parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-p" };

        adm.get_request().vs_element.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );
        adm.get_request().vs_element.realserver_vector.front().tcp_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:80" );
    
        bool ret = adm.parse_opt_vs_udp_func_wp( pos, argc, argv );

        // unit_test[116] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[117] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) udp_mode check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.udpmode, true );

        // unit_test[118] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    zeropoint;
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, zeropoint );

        // unit_test[119] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) udp_recv_endpoint check
        boost::asio::ip::udp::endpoint    udp_recv_ep = string_to_endpoint<boost::asio::ip::udp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.udp_recv_endpoint, udp_recv_ep );

        // unit_test[120] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) realserver tcp_endpoint check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, zeropoint );

        // unit_test[121] parse_opt_vs_udp_func normal case 2 (already set tcp_endpoint) realserver udp_endpoint check
        boost::asio::ip::udp::endpoint    rs_udp_ep = string_to_endpoint<boost::asio::ip::udp>( "10.144.169.86:80" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().udp_endpoint, rs_udp_ep );

    }

    BOOST_MESSAGE( "----- parse_opt_vs_udp_func_test end -----" );

}

void    parse_opt_vs_ssl_file_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_ssl_file_func_test start -----" );

    // parse_opt_vs_ssl_file_func normal case 1
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-z", "./sslfile.cf" };
    
        bool ret = adm.parse_opt_vs_ssl_file_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_ssl_file_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_ssl_file_func normal case 1 filename check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.ssl_file_name, "./sslfile.cf" );
    }

    // pser_opt_vs_ssl_file_func error case 1 (filename not specified)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 3;
        char*   argv[]      = { "l7vsadm_test", "-A", "-z"};
    
        bool ret = adm.parse_opt_vs_ssl_file_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_ssl_file_func error case 1 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_ssl_file_func error case 2 (filename too long)
    {
        std::string filename;
        filename.resize( L7VS_FILENAME_LEN + 1, 'a' );

        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-z", const_cast< char* >( filename.c_str() )  };
    
        bool ret = adm.parse_opt_vs_ssl_file_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_ssl_file_func error case 2 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_ssl_file_func error case 3 (file cannnot open)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-z", "./test.dat"};
    
        bool ret = adm.parse_opt_vs_ssl_file_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_ssl_file_func error case 3 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_ssl_file_func_test end -----" );

}

void    parse_opt_vs_access_log_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_access_log_func_test start -----" );

    // parse_opt_vs_access_log_func normal case 1
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-L", "1" };
    
        bool ret = adm.parse_opt_vs_access_log_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_access_log_func normal case 1 filename check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.access_log_flag, 1 );
    }

    // pser_opt_vs_access_log_func error case 1 (access_log_flag not specified)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 3;
        char*   argv[]      = { "l7vsadm_test", "-A", "-L"};
    
        bool ret = adm.parse_opt_vs_access_log_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_func error case 1 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_access_log_func error case 2 (invalid range access log flag value)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-z", "2"};
    
        bool ret = adm.parse_opt_vs_access_log_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_func error case 2 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_access_log_func error case 3 (invalied charactor access log flag value)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-z", "a"};
    
        bool ret = adm.parse_opt_vs_access_log_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_func error case 3 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_access_log_func_test end -----" );

}

void    parse_opt_vs_access_log_logrotate_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_access_log_logrotate_func_test start -----" );

    // parse_opt_vs_access_log_logrotate_func normal case 1 (end with access log rotate option)
    {
        l7vsadm_test    adm;
        int        pos     = 2;
        int        argc    = 6;
        char*    argv[]    = { "l7vsadm_test", "-A", "-a", "/var/log/l7vs/access.log", "--option", "zzz" };
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_called = false;
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_fail = false;
    
        bool ret = adm.parse_opt_vs_access_log_logrotate_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 1 check_parameter call check
        BOOST_CHECK_EQUAL( l7vs::logger_access_manager::access_log_logrotate_parameter_check_called, true );
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 1 access log file name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.access_log_file_name, "/var/log/l7vs/access.log" );
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 1 rotate argument check
        std::map< std::string, std::string >::iterator it =  adm.get_request().vs_element.access_log_rotate_arguments.find("--option");
        BOOST_CHECK( it != adm.get_request().vs_element.access_log_rotate_arguments.end() );
        BOOST_CHECK( it->second == "zzz" );
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 1 rotate argument atring check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.access_log_rotate_key_info, "--option zzz" );
    }


    // parse_opt_vs_access_log_logrotate_func normal case 2 (end with next vs option)
    {
        l7vsadm_test    adm;
        int        pos     = 2;
        int        argc    = 7;
        char*    argv[]    = { "l7vsadm_test", "-A", "-a", "/var/log/l7vs/access.log", "--option", "zzz", "-t" };
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_called = false;
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_fail = false;
    
        bool ret = adm.parse_opt_vs_access_log_logrotate_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 2 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 2 check_parameter call check
        BOOST_CHECK_EQUAL( l7vs::logger_access_manager::access_log_logrotate_parameter_check_called, true );
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 2 access log file name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.access_log_file_name, "/var/log/l7vs/access.log" );
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 2 rotate argument check
        std::map< std::string, std::string >::iterator it =  adm.get_request().vs_element.access_log_rotate_arguments.find("--option");
        BOOST_CHECK( it != adm.get_request().vs_element.access_log_rotate_arguments.end() );
        BOOST_CHECK( it->second == "zzz" );
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 2 rotate argument atring check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.access_log_rotate_key_info, "--option zzz" );
    }

    // parse_opt_vs_access_log_logrotate_func normal case 3 (no rotate arg)
    {
        l7vsadm_test    adm;
        int        pos     = 2;
        int        argc    = 5;
        char*    argv[]    = { "l7vsadm_test", "-A", "-a", "/var/log/l7vs/access.log", "-t" };
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_called = false;
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_fail = false;
    
        bool ret = adm.parse_opt_vs_access_log_logrotate_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 3 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 3 check_parameter call check
        BOOST_CHECK_EQUAL( l7vs::logger_access_manager::access_log_logrotate_parameter_check_called, false );
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 3 access log file name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.access_log_file_name, "/var/log/l7vs/access.log" );
        // unit_test[xx] parse_opt_vs_access_log_logrotate_func normal case 3 rotate argument atring check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.access_log_rotate_key_info, "" );
    }

    // parse_opt_access_log_logrotate_func error case 1 (no access_log_file name)
    {
        l7vsadm_test    adm;
        int        pos     = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-A", "-a" };
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_called = false;
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_fail = false;
    
        bool ret = adm.parse_opt_vs_access_log_logrotate_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_logrotate_func error case 1 return value check
        BOOST_CHECK_EQUAL( ret, false );    

    }

    // parse_opt_access_log_logrotate_func error case 2 (access_log_file name too long)
    {
        std::string filename;
        filename.resize( L7VS_FILENAME_LEN, 'a');
        filename = "/" + filename;

        l7vsadm_test    adm;
        int        pos     = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-a", const_cast< char* >( filename.c_str() ) };
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_called = false;
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_fail = false;
    
        bool ret = adm.parse_opt_vs_access_log_logrotate_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_logrotate_func error case 2 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_access_log_logrotate_func error case 3 (access_log_file name not fullpath)
    {
        l7vsadm_test    adm;
        int        pos     = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-A", "-a", "access.log" };
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_called = false;
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_fail = false;
    
        bool ret = adm.parse_opt_vs_access_log_logrotate_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_logrotate_func error case 3 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_access_log_logrotate_func error case 4 (rotate argument check error)
    {
        l7vsadm_test    adm;
        int        pos     = 2;
        int        argc    = 6;
        char*    argv[]    = { "l7vsadm_test", "-A", "-a", "/var/log/l7vs/access.log", "--option", "zzz" };
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_called = false;
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_fail = true;
    
        bool ret = adm.parse_opt_vs_access_log_logrotate_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_logrotate_func error case 4 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_access_log_logrotate_func error case 5 (rotate argument is not pair)
    {
        l7vsadm_test    adm;
        int        pos     = 2;
        int        argc    = 5;
        char*    argv[]    = { "l7vsadm_test", "-A", "-a", "/var/log/l7vs/access.log", "--option" };
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_called = false;
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_fail = false;
    
        bool ret = adm.parse_opt_vs_access_log_logrotate_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_logrotate_func error case 5 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_access_log_logrotate_func error case 6 (rotate argument is duplicated)
    {
        l7vsadm_test    adm;
        int        pos     = 2;
        int        argc    = 8;
        char*    argv[]    = { "l7vsadm_test", "-A", "-a", "/var/log/l7vs/access.log", "--option", "zzz", "--option", "xxx" };
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_called = false;
        l7vs::logger_access_manager::access_log_logrotate_parameter_check_fail = false;
    
        bool ret = adm.parse_opt_vs_access_log_logrotate_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_access_log_logrotate_func error case 6 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_access_log_logrotate_func_test end -----" );

}

void    parse_opt_vs_socket_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_socket_func_test start -----" );

    // parse_opt_vs_socket_func normal case 1
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O", "deferaccept,nodelay,cork,quickackoff" };
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_socket_func normal case 1 option check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_defer_accept, 1 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_nodelay, 1 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_cork, 1 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_quickack, 2 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_string, "deferaccept,nodelay,cork,quickackoff" );
    }

    // parse_opt_vs_socket_func normal case 2
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O", "nodelay" };
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func normal case 3 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_socket_func normal case 3 option check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_defer_accept, 0 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_nodelay, 1 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_cork, 0 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_quickack, 0 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_string, "nodelay" );
    }

    // parse_opt_vs_socket_func normal case 3
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O", "deferaccept,quickackon" };
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func normal case 3 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_opt_vs_socket_func normal case 3 option check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_defer_accept, 1 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_nodelay, 0 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_cork, 0 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_tcp_quickack, 1 );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.socket_option_string, "deferaccept,quickackon" );
    }

    // pser_opt_vs_socket_func error case 1 (socket option not specified)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 3;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O"};
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func error case 1 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_socket_func error case 2 (unknown socket option)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O", "deferaccept,nodelay,cork,quickackoff,zzz" };
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func error case 2 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_socket_func error case 3 (deferaccept duplicated)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O", "deferaccept,nodelay,cork,quickackoff,deferaccept" };
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func error case 3 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_socket_func error case 4 (nodelay duplicated)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O", "deferaccept,nodelay,cork,quickackoff,nodelay" };
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func error case 4 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_socket_func error case 5 (cork duplicated)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O", "deferaccept,nodelay,cork,quickackoff,cork" };
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func error case 5 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_socket_func error case 6 (quickack duplicated 1)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O", "deferaccept,nodelay,cork,quickackoff,quickackoff" };
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func error case 6 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // pser_opt_vs_socket_func error case 7 (quickack duplicated 2)
    {
        l7vsadm_test    adm;
        int     pos         = 2;
        int     argc        = 4;
        char*   argv[]      = { "l7vsadm_test", "-A", "-O", "deferaccept,nodelay,cork,quickackoff,quickackon" };
    
        bool ret = adm.parse_opt_vs_socket_func_wp( pos, argc, argv );

        // unit_test[xx] parse_opt_vs_socket_func error case 7 return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_vs_socket_func_test end -----" );

}

void    parse_vs_func_test(){
    BOOST_MESSAGE( "----- parse_vs_func_test start -----" );

    // parse_vs_func normal case 1 (CMD_ADD_VS short_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_VS;
        int        argc    = 17;
        char*    argv[]    = {    "l7vsadm_test",
                            "-A",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-s",
                            "lc",
                            "-u",
                            "50",
                            "-b",
                            "10.144.169.86:8080",
                            "-Q",
                            "100M",
                            "-q",
                            "200M"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[122] parse_vs_func normal case 1 (CMD_ADD_VS short_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[123] parse_vs_func normal case 1 (CMD_ADD_VS short_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_ADD_VS );
        // unit_test[124] parse_vs_func normal case 1 (CMD_ADD_VS short_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[125] parse_vs_func normal case 1 (CMD_ADD_VS short_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[126] parse_vs_func normal case 1 (CMD_ADD_VS short_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[127] parse_vs_func normal case 1 (CMD_ADD_VS short_option) schedule module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "lc" );
        // unit_test[128] parse_vs_func normal case 1 (CMD_ADD_VS short_option) sorry_maxconnection check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 50 );
        // unit_test[129] parse_vs_func normal case 1 (CMD_ADD_VS short_option) sorry_endpoint check
        boost::asio::ip::tcp::endpoint    sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, sorry_ep );
        // unit_test[130] parse_vs_func normal case 1 (CMD_ADD_VS short_option) qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 100000000ULL / 8 ) );
        // unit_test[131] parse_vs_func normal case 1 (CMD_ADD_VS short_option) qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 200000000ULL / 8 ) );
    }

    // parse_vs_func normal case 2 (CMD_ADD_VS long_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_VS;
        int        argc    = 17;
        char*    argv[]    = {    "l7vsadm_test",
                            "--add-service",
                            "--tcp-service",
                            "10.144.169.86:11500",
                            "--proto-module",
                            "cinsert",
                            "mod_arg",
                            "--scheduler",
                            "wrr",
                            "--upper",
                            "96",
                            "--bypass",
                            "10.144.169.87:80",
                            "--qos-up",
                            "100M",
                            "--qos-down",
                            "200M"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );
    
        // unit_test[132] parse_vs_func normal case 2 (CMD_ADD_VS long_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[133] parse_vs_func normal case 2 (CMD_ADD_VS long_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_ADD_VS );
        // unit_test[134] parse_vs_func normal case 2 (CMD_ADD_VS long_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:11500" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[135] parse_vs_func normal case 2 (CMD_ADD_VS long_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[136] parse_vs_func normal case 2 (CMD_ADD_VS long_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[137] parse_vs_func normal case 2 (CMD_ADD_VS long_option) schedule module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "wrr" );
        // unit_test[138] parse_vs_func normal case 2 (CMD_ADD_VS long_option) sorry_maxconnection check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 96 );
        // unit_test[139] parse_vs_func normal case 2 (CMD_ADD_VS long_option) sorry_endpoint check
        boost::asio::ip::tcp::endpoint    sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:80" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, sorry_ep );
        // unit_test[140] parse_vs_func normal case 2 (CMD_ADD_VS long_option) qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 100000000ULL / 8 ) );
        // unit_test[141] parse_vs_func normal case 2 (CMD_ADD_VS long_option) qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 200000000ULL / 8 ) );
    }

    // parse_vs_func normal case 3 (CMD_EDIT_VS short_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_EDIT_VS;
        int        argc    = 19;
        char*    argv[]    = {    "l7vsadm_test",
                            "-E",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-s",
                            "rr",
                            "-u",
                            "50",
                            "-b",
                            "10.144.169.86:8080",
                            "-f",
                            "1",
                            "-Q",
                            "100M",
                            "-q",
                            "200M"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[142] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[143] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_EDIT_VS );
        // unit_test[144] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[145] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[146] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[147] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) schedule module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "rr" );
        // unit_test[148] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) sorry_maxconnection check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 50 );
        // unit_test[149] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) sorry_endpoint check
        boost::asio::ip::tcp::endpoint    sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, sorry_ep );
        // unit_test[150] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) sorry_flag check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_flag, true );
        // unit_test[151] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 100000000ULL / 8 ) );
        // unit_test[152] parse_vs_func normal case 3 (CMD_EDIT_VS short_option) qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 200000000ULL / 8 ) );
    }

    // parse_vs_func normal case 4 (CMD_EDIT_VS long_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_EDIT_VS;
        int        argc    = 19;
        char*    argv[]    = {    "l7vsadm_test",
                            "--edit-service",
                            "--tcp-service",
                            "10.144.169.86:11500",
                            "--proto-module",
                            "cinsert",
                            "mod_arg",
                            "--scheduler",
                            "lc",
                            "--upper",
                            "96",
                            "--bypass",
                            "10.144.169.87:80",
                            "--flag",
                            "1",
                            "--qos-up",
                            "100M",
                            "--qos-down",
                            "200M"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );
    
        // unit_test[153] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[154] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_EDIT_VS );
        // unit_test[155] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:11500" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[156] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[157] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[158] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) schedule module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "lc" );
        // unit_test[159] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) sorry_maxconnection check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_maxconnection, 96 );
        // unit_test[160] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) sorry_endpoint check
        boost::asio::ip::tcp::endpoint    sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:80" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_endpoint, sorry_ep );
        // unit_test[161] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) sorry_flag check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_flag, true );
        // unit_test[162] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) qos_upstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_upstream, ( 100000000ULL / 8 ) );
        // unit_test[163] parse_vs_func normal case 4 (CMD_EDIT_VS long_option) qos_downstream check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.qos_downstream, ( 200000000ULL / 8 ) );
    }

    // parse_vs_func normal case 5 (CMD_DEL_VS short_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_DEL_VS;
        int        argc    = 7;
        char*    argv[]    = {    "l7vsadm_test",
                            "-D",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[164] parse_vs_func normal case 5 (CMD_DEL_VS short_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[165] parse_vs_func normal case 5 (CMD_DEL_VS short_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_DEL_VS );
        // unit_test[166] parse_vs_func normal case 5 (CMD_DEL_VS short_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[167] parse_vs_func normal case 5 (CMD_DEL_VS short_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[168] parse_vs_func normal case 5 (CMD_DEL_VS short_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
    }

    // parse_vs_func normal case 6 (CMD_DEL_VS long_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_DEL_VS;
        int        argc    = 7;
        char*    argv[]    = {    "l7vsadm_test",
                            "--delete-service",
                            "--tcp-service",
                            "10.144.169.86:11500",
                            "--proto-module",
                            "curl",
                            "specified_url"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );
    
        // unit_test[169] parse_vs_func normal case 6 (CMD_DEL_VS long_option) return value check
        BOOST_CHECK_EQUAL( ret, true );
        // unit_test[170] parse_vs_func normal case 6 (CMD_DEL_VS long_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_DEL_VS );
        // unit_test[171] parse_vs_func normal case 6 (CMD_DEL_VS long_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:11500" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[172] parse_vs_func normal case 6 (CMD_DEL_VS long_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "curl" );
        // unit_test[173] parse_vs_func normal case 6 (CMD_DEL_VS long_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "specified_url" );
    }

    // parse_vs_func normal case 7 (CMD_ADD_VS scheduler omit)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_VS;
        int        argc    = 7;
        char*    argv[]    = {    "l7vsadm_test",
                            "-A",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[174] parse_vs_func normal case 7 (CMD_ADD_VS scheduler omit) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[175] parse_vs_func normal case 7 (CMD_ADD_VS scheduler omit) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_ADD_VS );
        // unit_test[176] parse_vs_func normal case 7 (CMD_ADD_VS scheduler omit) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[177] parse_vs_func normal case 7 (CMD_ADD_VS scheduler omit) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[178] parse_vs_func normal case 7 (CMD_ADD_VS scheduler omit) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[179] parse_vs_func normal case 7 (CMD_ADD_VS scheduler omit) schedule module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.schedule_module_name, "rr" );
    }

    // parse_vs_func normal case 8 (CMD_FLUSH_VS)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_FLUSH_VS;
        int        argc    = 2;
        char*    argv[]    = {    "l7vsadm_test",
                            "-C"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[180] parse_vs_func normal case 8 (CMD_FLUSH_VS) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[181] parse_vs_func normal case 8 (CMD_FLUSH_VS) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_FLUSH_VS );
    }

    // parse_vs_func normal case 9 (CMD_EDIT_VS minimum option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_EDIT_VS;
        int        argc    = 9;
        char*    argv[]    = {    "l7vsadm_test",
                            "-E",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-s",
                            "lc"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[182] parse_vs_func normal case 9 (CMD_EDIT_VS minimum option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[183] parse_vs_func normal case 9 (CMD_EDIT_VS minimum option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_EDIT_VS );
        // unit_test[184] parse_vs_func normal case 9 (CMD_EDIT_VS minimum option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[185] parse_vs_func normal case 9 (CMD_EDIT_VS minimum option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
    }

    // parse_vs_func normal case 10 (CMD_DEL_VS minimum option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_DEL_VS;
        int        argc    = 6;
        char*    argv[]    = {    "l7vsadm_test",
                            "-D",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[186] parse_vs_func normal case 10 (CMD_DEL_VS minimum option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[187] parse_vs_func normal case 10 (CMD_DEL_VS minimum option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_DEL_VS );
        // unit_test[188] parse_vs_func normal case 10 (CMD_DEL_VS minimum option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[189] parse_vs_func normal case 10 (CMD_DEL_VS minimum option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
    }

    // parse_vs_func error case 1 (CMD_ADD_VS protocol module not specified)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_VS;
        int        argc    = 4;
        char*    argv[]    = {    "l7vsadm_test",
                            "-A",
                            "-t",
                            "10.144.169.87:22100"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[190] parse_vs_func error case 1 (CMD_ADD_VS protocol module not specified) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_vs_func error case 2 (CMD_ADD_VS target address not specified)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_VS;
        int        argc    = 5;
        char*    argv[]    = {    "l7vsadm_test",
                            "-A",
                            "-m",
                            "cinsert",
                            "mod_arg"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[191] parse_vs_func error case 2 (CMD_ADD_VS target address not specified) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_vs_func error case 3 (upper option value under 0)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_VS;
        int        argc    = 9;
        char*    argv[]    = {    "l7vsadm_test",
                            "-A",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-u",
                            "-1"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[192] parse_vs_func error case 3 (upper option value under 0) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_vs_func error case 4 (access log on but not specified access log fil name)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_VS;
        int        argc    = 9;
        char*    argv[]    = {    "l7vsadm_test",
                            "-A",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-L",
                            "1"
                            };
    
        bool ret = adm.parse_vs_func_wp( cmd, argc, argv );

        // unit_test[xx] parse_vs_func error case 4  return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_vs_func_test end -----" );

}

void    parse_opt_rs_weight_func_test(){
    BOOST_MESSAGE( "----- parse_opt_rs_weight_func_test start -----" );

    // parse_opt_rs_weight_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-w", "64" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        bool ret = adm.parse_opt_rs_weight_func_wp( pos, argc, argv );

        // unit_test[193] parse_opt_rs_weight_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[194] parse_opt_rs_weight_func normal case 1 weight check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().weight, 64 );
    }

    // parse_opt_rs_weight_func error case 1 (invalid weight value (charactor))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-w", "a" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );
    
        bool ret = adm.parse_opt_rs_weight_func_wp( pos, argc, argv );

        // unit_test[195] parse_opt_rs_weight_func error case 1 (invalid weight value (charactor)) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

    // parse_opt_rs_weight_func error case 2 (invalid weight value(int over))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        std::stringstream    ss;
        ss << (1ULL) + INT_MAX;
        char*    argv[]    = { "l7vsadm_test", "-a", "-w", const_cast<char*>( ss.str().c_str() ) };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );
    
        bool ret = adm.parse_opt_rs_weight_func_wp( pos, argc, argv );

        // unit_test[196] parse_opt_rs_weight_func error case 2 (invalid weight value(int over)) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

    // parse_opt_rs_weight_func error case 3 (no weight value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-a", "-w" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );
    
        bool ret = adm.parse_opt_rs_weight_func_wp( pos, argc, argv );

        // unit_test[197] parse_opt_rs_weight_func error case 3 (no weight value) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

    // parse_opt_rs_weight_func error case 4 (under 0)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-w", "-1" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );
    
        bool ret = adm.parse_opt_rs_weight_func_wp( pos, argc, argv );

        // unit_test[198] parse_opt_rs_weight_func error case 4 (under 0) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

    // parse_opt_rs_weight_func error case 5 (over 100)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-w", "101" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );
    
        bool ret = adm.parse_opt_rs_weight_func_wp( pos, argc, argv );

        // unit_test[199] parse_opt_rs_weight_func error case 5 (over 100) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

    BOOST_MESSAGE( "----- parse_opt_rs_weight_func_test end -----" );

}

void    parse_opt_rs_realserver_func_test(){
    BOOST_MESSAGE( "----- parse_opt_rs_realserver_func_test start -----" );

    // parse_opt_rs_realserver_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-r", "10.144.169.87:8080" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );
    
        bool ret = adm.parse_opt_rs_realserver_func_wp( pos, argc, argv );

        // unit_test[200] parse_opt_rs_realserver_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[201] parse_opt_rs_realserver_func normal case 1 endpoint check
        boost::asio::ip::tcp::endpoint    ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, ep );
    }

    // parse_opt_rs_realserver_func normal case 2 (realserver specified in name)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-r", "localhost:http" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        bool ret = adm.parse_opt_rs_realserver_func_wp( pos, argc, argv );

        // unit_test[202] parse_opt_rs_realserver_func normal case 2 (realserver specified in name) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[203] parse_opt_rs_realserver_func normal case 2 (realserver specified in name) endpoint check
        boost::asio::ip::tcp::endpoint ep1 = string_to_endpoint<boost::asio::ip::tcp>("127.0.0.1:80");
        boost::asio::ip::tcp::endpoint ep2 = string_to_endpoint<boost::asio::ip::tcp>("[::1]:80");
        BOOST_CHECK( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint == ep1 ||
        	adm.get_request().vs_element.realserver_vector.front().tcp_endpoint == ep2);
    }

    // parse_opt_rs_realserver_func error case 1 (invalid endpoint)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-r", "10.144.169.257:8080" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        bool ret = adm.parse_opt_rs_realserver_func_wp( pos, argc, argv );

        // unit_test[204] parse_opt_rs_realserver_func error case 1 (invalid endpoint) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_rs_realserver_func error case 2 (no endpoint)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-a", "-r" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        bool ret = adm.parse_opt_rs_realserver_func_wp( pos, argc, argv );

        // unit_test[205] parse_opt_rs_realserver_func error case 2 (no endpoint) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_rs_realserver_func error case 3 (0.0.0.0 address)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-r", "0.0.0.0:8080" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        bool ret = adm.parse_opt_rs_realserver_func_wp( pos, argc, argv );

        // unit_test[206] parse_opt_rs_realserver_func error case 3 (0.0.0.0 address) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_rs_realserver_func error case 4 (0 port)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-r", "10.144.169.87:0" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        bool ret = adm.parse_opt_rs_realserver_func_wp( pos, argc, argv );

        // unit_test[207] parse_opt_rs_realserver_func error case 4 (0 port) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_rs_realserver_func error case 5 (invalid port (99999))
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-r", "10.144.169.87:99999" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        bool ret = adm.parse_opt_rs_realserver_func_wp( pos, argc, argv );

        // unit_test[208] parse_opt_rs_realserver_func error case 5 (invalid port (99999)) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_rs_realserver_func error case 6 (hostname omitted)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-r", ":22100" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        bool ret = adm.parse_opt_rs_realserver_func_wp( pos, argc, argv );

        // unit_test[209] parse_opt_rs_realserver_func error case 6 (hostname omitted) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_rs_realserver_func error case 7 (port omitted)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-a", "-r", "10.144.169.87:" };
        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        bool ret = adm.parse_opt_rs_realserver_func_wp( pos, argc, argv );

        // unit_test[210] parse_opt_rs_realserver_func error case 7 (port omitted) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_rs_realserver_func_test end -----" );

}

void    parse_rs_func_test(){
    BOOST_MESSAGE( "----- parse_rs_func_test start -----" );

    // parse_rs_func normal case 1 (CMD_ADD_RS short_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_RS;
        int        argc    = 11;
        char*    argv[]    = {    "l7vsadm_test",
                            "-a",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "10.144.169.86:8080",
                            "-w",
                            "10"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[211] parse_rs_func normal case 1 (CMD_ADD_RS short_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[212] parse_rs_func normal case 1 (CMD_ADD_RS short_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_ADD_RS );
        // unit_test[213] parse_rs_func normal case 1 (CMD_ADD_RS short_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[214] parse_rs_func normal case 1 (CMD_ADD_RS short_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[215] parse_rs_func normal case 1 (CMD_ADD_RS short_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[216] parse_rs_func normal case 1 (CMD_ADD_RS short_option) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
        // unit_test[217] parse_rs_func normal case 1 (CMD_ADD_RS short_option) weight check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().weight, 10 );
    }

    // parse_rs_func normal case 2 (CMD_ADD_RS long_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_RS;
        int        argc    = 11;
        char*    argv[]    = {    "l7vsadm_test",
                            "--add-server",
                            "--tcp-service",
                            "10.144.169.86:11500",
                            "--proto-module",
                            "cinsert",
                            "mod_arg",
                            "--real-server",
                            "10.144.169.87:8888",
                            "--weight",
                            "20"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[218] parse_rs_func normal case 2 (CMD_ADD_RS long_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[219] parse_rs_func normal case 2 (CMD_ADD_RS long_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_ADD_RS );
        // unit_test[220] parse_rs_func normal case 2 (CMD_ADD_RS long_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:11500" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[221] parse_rs_func normal case 2 (CMD_ADD_RS long_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[222] parse_rs_func normal case 2 (CMD_ADD_RS long_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[223] parse_rs_func normal case 2 (CMD_ADD_RS long_option) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:8888" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
        // unit_test[224] parse_rs_func normal case 2 (CMD_ADD_RS long_option) weight check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().weight, 20 );
    }

    // parse_rs_func normal case 3 (CMD_EDIT_RS short_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_EDIT_RS;
        int        argc    = 11;
        char*    argv[]    = {    "l7vsadm_test",
                            "-e",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "10.144.169.86:8080",
                            "-w",
                            "20"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[225] parse_rs_func normal case 3 (CMD_EDIT_RS short_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[226] parse_rs_func normal case 3 (CMD_EDIT_RS short_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_EDIT_RS );
        // unit_test[227] parse_rs_func normal case 3 (CMD_EDIT_RS short_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[228] parse_rs_func normal case 3 (CMD_EDIT_RS short_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[229] parse_rs_func normal case 3 (CMD_EDIT_RS short_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[230] parse_rs_func normal case 3 (CMD_EDIT_RS short_option) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
        // unit_test[231] parse_rs_func normal case 3 (CMD_EDIT_RS short_option) weight check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().weight, 20 );
    }

    // parse_rs_func normal case 4 (CMD_EDIT_RS long_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_EDIT_RS;
        int        argc    = 11;
        char*    argv[]    = {    "l7vsadm_test",
                            "--edit-server",
                            "--tcp-service",
                            "10.144.169.86:11500",
                            "--proto-module",
                            "cinsert",
                            "mod_arg",
                            "--real-server",
                            "10.144.169.87:8888",
                            "--weight",
                            "30"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[232] parse_rs_func normal case 4 (CMD_EDIT_RS long_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[233] parse_rs_func normal case 4 (CMD_EDIT_RS long_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_EDIT_RS );
        // unit_test[234] parse_rs_func normal case 4 (CMD_EDIT_RS long_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:11500" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[235] parse_rs_func normal case 4 (CMD_EDIT_RS long_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[236] parse_rs_func normal case 4 (CMD_EDIT_RS long_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[237] parse_rs_func normal case 4 (CMD_EDIT_RS long_option) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:8888" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
        // unit_test[238] parse_rs_func normal case 4 (CMD_EDIT_RS long_option) weight check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().weight, 30 );
    }

    // parse_rs_func normal case 5 (CMD_DEL_RS short_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_DEL_RS;
        int        argc    = 9;
        char*    argv[]    = {    "l7vsadm_test",
                            "-d",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "10.144.169.86:8080"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[239] parse_rs_func normal case 5 (CMD_DEL_RS short_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[240] parse_rs_func normal case 5 (CMD_DEL_RS short_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_DEL_RS );
        // unit_test[241] parse_rs_func normal case 5 (CMD_DEL_RS short_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[242] parse_rs_func normal case 5 (CMD_DEL_RS short_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[243] parse_rs_func normal case 5 (CMD_DEL_RS short_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[244] parse_rs_func normal case 5 (CMD_DEL_RS short_option) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
    }

    // parse_rs_func normal case 6 (CMD_DEL_RS long_option)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_DEL_RS;
        int        argc    = 9;
        char*    argv[]    = {    "l7vsadm_test",
                            "--delete-server",
                            "--tcp-service",
                            "10.144.169.86:11500",
                            "--proto-module",
                            "cinsert",
                            "mod_arg",
                            "--real-server",
                            "10.144.169.87:8888"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[245] parse_rs_func normal case 6 (CMD_DEL_RS long_option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[246] parse_rs_func normal case 6 (CMD_DEL_RS long_option) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_DEL_RS );
        // unit_test[247] parse_rs_func normal case 6 (CMD_DEL_RS long_option) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:11500" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[248] parse_rs_func normal case 6 (CMD_DEL_RS long_option) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[249] parse_rs_func normal case 6 (CMD_DEL_RS long_option) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[250] parse_rs_func normal case 6 (CMD_DEL_RS long_option) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:8888" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
    }

//ramiel_ipv6 add
    // parse_rs_func normal case 7 (CMD_ADD_RS ipv6 )
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_RS;
        int        argc    = 11;
        char*    argv[]    = {    "l7vsadm_test",
                            "-a",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "[2001::10]:8080",
                            "-w",
                            "10"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[xx] parse_rs_func normal case 7 (CMD_ADD_RS ipv6) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[xx] parse_rs_func normal case 7 (CMD_ADD_RS ipv6) request check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_ADD_RS );
        // unit_test[xx] parse_rs_func normal case 7 (CMD_ADD_RS ipv6) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[xx] parse_rs_func normal case 7 (CMD_ADD_RS ipv6) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[xx] parse_rs_func normal case 7 (CMD_ADD_RS ipv6) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[xx] parse_rs_func normal case 7 (CMD_ADD_RS ipv6) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "[2001::10]:8080" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
        // unit_test[xx] parse_rs_func normal case 7 (CMD_ADD_RS ipv6) weight check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().weight, 10 );
    }

    // parse_rs_func error case 1 (CMD_ADD_RS protocol module not specified)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_RS;
        int        argc    = 6;
        char*    argv[]    = {    "l7vsadm_test",
                            "-a",
                            "-t",
                            "10.144.169.87:22100",
                            "-r",
                            "10.144.169.86:8080"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[251] parse_rs_func error case 1 (CMD_ADD_RS protocol module not specified) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

    // parse_rs_func error case 2 (CMD_ADD_RS target address not specified)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_RS;
        int        argc    = 7;
        char*    argv[]    = {    "l7vsadm_test",
                            "-a",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "10.144.169.86:8080"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[252] parse_rs_func error case 2 (CMD_ADD_RS target address not specified) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

    // parse_rs_func error case 3 (CMD_ADD_RS realserver address not specified)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_RS;
        int        argc    = 7;
        char*    argv[]    = {    "l7vsadm_test",
                            "-a",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[253] parse_rs_func error case 3 (CMD_ADD_RS realserver address not specified) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

//ramiel_ipv6 add
    // parse_rs_func error case 4 (CMD_ADD_RS realserver any address ipv4 )
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_RS;
        int        argc    = 11;
        char*    argv[]    = {    "l7vsadm_test",
                            "-a",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "0.0.0.0:8080",
                            "-w",
                            "10"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[xx] parse_rs_func error case 4 (CMD_ADD_RS realserver any address ipv4) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

//ramiel_ipv6 add
    // parse_rs_func error case 5 (CMD_ADD_RS realserver any address ipv6 )
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_ADD_RS;
        int        argc    = 11;
        char*    argv[]    = {    "l7vsadm_test",
                            "-a",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "[::]:8080",
                            "-w",
                            "10"
                            };
    
        bool ret = adm.parse_rs_func_wp( cmd, argc, argv );

        // unit_test[xx] parse_rs_func error case 5 (CMD_ADD_RS realserver any address ipv6) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

    BOOST_MESSAGE( "----- parse_rs_func_test end -----" );

}

void    parse_opt_replication_switch_func_test(){
    BOOST_MESSAGE( "----- parse_opt_replication_switch_func_test start -----" );

    // parse_opt_replication_switch_func normal case 1 (replication start)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-R", "-s", "start" };

        bool ret = adm.parse_opt_replication_switch_func_wp( pos, argc, argv );
    
        // unit_test[254] parse_opt_replication_switch_func normal case 1 (replication start) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[255] parse_opt_replication_switch_func normal case 1 (replication start) replication command check
        BOOST_CHECK_EQUAL( adm.get_request().replication_command, l7vs::l7vsadm_request::REP_START );
    }

    // parse_opt_replication_switch_func normal case 2 (replication stop)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-R", "-s", "stop" };

        bool ret = adm.parse_opt_replication_switch_func_wp( pos, argc, argv );
    
        // unit_test[256] parse_opt_replication_switch_func normal case 2 (replication stop) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[257] parse_opt_replication_switch_func normal case 2 (replication stop) replication command check
        BOOST_CHECK_EQUAL( adm.get_request().replication_command, l7vs::l7vsadm_request::REP_STOP );
    }

    // parse_opt_replication_switch_func error case 1 (invalid switch option)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-R", "-s", "end" };

        bool ret = adm.parse_opt_replication_switch_func_wp( pos, argc, argv );
    
        // unit_test[258] parse_opt_replication_switch_func error case 1 (invalid switch option) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_replication_switch_func error case 2 (no switch option)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-R", "-s" };

        bool ret = adm.parse_opt_replication_switch_func_wp( pos, argc, argv );
    
        // unit_test[259] parse_opt_replication_switch_func error case 2 (no switch option) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_replication_switch_func_test end -----" );

}

void    parse_opt_replication_force_func_test(){
    BOOST_MESSAGE( "----- parse_opt_replication_force_func_test start -----" );

    // parse_opt_replication_force_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-R", "-f" };

        bool ret = adm.parse_opt_replication_force_func_wp( pos, argc, argv );
    
        // unit_test[260] parse_opt_replication_force_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[261] parse_opt_replication_force_func normal case 1 replication command check
        BOOST_CHECK_EQUAL( adm.get_request().replication_command, l7vs::l7vsadm_request::REP_FORCE );
    }

    BOOST_MESSAGE( "----- parse_opt_replication_force_func_test end -----" );

}

void    parse_opt_replication_dump_func_test(){
    BOOST_MESSAGE( "----- parse_opt_replication_dump_func_test start -----" );

    // parse_opt_replication_dump_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-R", "-d" };

        bool ret = adm.parse_opt_replication_dump_func_wp( pos, argc, argv );
    
        // unit_test[262] parse_opt_replication_dump_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[263] parse_opt_replication_dump_func normal case 1 replication command check
        BOOST_CHECK_EQUAL( adm.get_request().replication_command, l7vs::l7vsadm_request::REP_DUMP );
    }

    BOOST_MESSAGE( "----- parse_opt_replication_dump_func_test end -----" );

}

void    parse_replication_func_test(){
    BOOST_MESSAGE( "----- parse_replication_func_test start -----" );

    // parse_replication_func normal case 1 (replication start)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_REPLICATION;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-R", "-s", "start" };
    
        bool ret = adm.parse_replication_func_wp( cmd, argc, argv );

        // unit_test[264] parse_replication_func normal case 1 (replication start) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[265] parse_replication_func normal case 1 (replication start) request command check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_REPLICATION );
        // unit_test[266] parse_replication_func normal case 1 (replication start) replication command check
        BOOST_CHECK_EQUAL( adm.get_request().replication_command, l7vs::l7vsadm_request::REP_START );
    }

    // parse_replication_func normal case 2 (replication stop)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_REPLICATION;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-R", "-s", "stop" };
    
        bool ret = adm.parse_replication_func_wp( cmd, argc, argv );
    
        // unit_test[267] parse_replication_func normal case 2 (replication stop) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[268] parse_replication_func normal case 2 (replication stop) request command check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_REPLICATION );
        // unit_test[269] parse_replication_func normal case 2 (replication stop) replication command check
        BOOST_CHECK_EQUAL( adm.get_request().replication_command, l7vs::l7vsadm_request::REP_STOP );
    }

    // parse_replication_func normal case 3 (replication force)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_REPLICATION;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-R", "-f" };
    
        bool ret = adm.parse_replication_func_wp( cmd, argc, argv );
    
        // unit_test[270] parse_replication_func normal case 3 (replication force) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[271] parse_replication_func normal case 3 (replication force) request command check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_REPLICATION );
        // unit_test[272] parse_replication_func normal case 3 (replication force) replication command check
        BOOST_CHECK_EQUAL( adm.get_request().replication_command, l7vs::l7vsadm_request::REP_FORCE );
    }

    // parse_replication_func normal case 4 (replication dump)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_REPLICATION;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-R", "-d" };
    
        bool ret = adm.parse_replication_func_wp( cmd, argc, argv );
    
        // unit_test[273] parse_replication_func normal case 4 (replication dump) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[274] parse_replication_func normal case 4 (replication dump) request command check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_REPLICATION );
        // unit_test[275] parse_replication_func normal case 4 (replication dump) replication command check
        BOOST_CHECK_EQUAL( adm.get_request().replication_command, l7vs::l7vsadm_request::REP_DUMP );
    }

    // parse_replication_func error case 1 (replication command duplicate)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_REPLICATION;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-R", "-f", "-d" };
    
        bool ret = adm.parse_replication_func_wp( cmd, argc, argv );
    
        // unit_test[276] parse_replication_func error case 1 (command duplicate) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_replication_func error case 2 (no replication command)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_REPLICATION;
        int        argc    = 2;
        char*    argv[]    = { "l7vsadm_test", "-R" };
    
        bool ret = adm.parse_replication_func_wp( cmd, argc, argv );
        
        // unit_test[277] parse_replication_func error case 2 (no replication command) return value check
        BOOST_CHECK_EQUAL( ret, false );
    }

    BOOST_MESSAGE( "----- parse_replication_func_test end -----" );

}

void    parse_opt_log_category_func_test(){
    BOOST_MESSAGE( "----- parse_opt_log_category_func_test start -----" );

    // parse_opt_log_category_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-L", "-c", "l7vsd_network" };
    
        bool ret = adm.parse_opt_log_category_func_wp( pos, argc, argv );

        // unit_test[278] parse_opt_log_category_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[279] parse_opt_log_category_func normal case 1 logcategory check
        BOOST_CHECK_EQUAL( adm.get_request().log_category, l7vs::LOG_CAT_L7VSD_NETWORK );
    }

    // parse_opt_log_category_func error case 1 (invalid logcategory value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-L", "-c", "l7vsd" };
    
        bool ret = adm.parse_opt_log_category_func_wp( pos, argc, argv );

        // unit_test[280] parse_opt_log_category_func error case 1 (invalid logcategory value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_log_category_func error case 2 (no logcategory value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-L", "-c" };
    
        bool ret = adm.parse_opt_log_category_func_wp( pos, argc, argv );

        // unit_test[281] parse_opt_log_category_func error case 2 (no logcategory value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_log_category_func_test end -----" );

}

void    parse_opt_log_level_func_test(){
    BOOST_MESSAGE( "----- parse_opt_log_level_func_test start -----" );

    // parse_opt_log_level_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-L", "-l", "error" };
    
        bool ret = adm.parse_opt_log_level_func_wp( pos, argc, argv );

        // unit_test[282] parse_opt_log_level_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[283] parse_opt_log_level_func normal case 1 loglevel check
        BOOST_CHECK_EQUAL( adm.get_request().log_level, l7vs::LOG_LV_ERROR );
    }

    // parse_opt_log_level_func error case 1 (invalid loglevel value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-L", "-l", "l7vsd" };
    
        bool ret = adm.parse_opt_log_level_func_wp( pos, argc, argv );

        // unit_test[284] parse_opt_log_level_func error case 1 (invalid loglevel value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_log_level_func error case 2 (no loglevel value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-L", "-l" };
    
        bool ret = adm.parse_opt_log_level_func_wp( pos, argc, argv );

        // unit_test[285] parse_opt_log_level_func error case 2 (no loglevel value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_log_level_func_test end -----" );

}

void    parse_log_func_test(){
    BOOST_MESSAGE( "----- parse_log_func_test start -----" );

    // parse_log_func normal case 1
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_LOG;
        int        argc    = 6;
        char*    argv[]    = {    "l7vsadm_test",
                            "-L",
                            "-c",
                            "l7vsd_network",
                            "-l",
                            "debug"
                            };
    
        bool ret = adm.parse_log_func_wp( cmd, argc, argv );

        // unit_test[286] parse_log_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[287] parse_log_func normal case 1 request command check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_LOG );
        // unit_test[288] parse_log_func normal case 1 logcategory check
        BOOST_CHECK_EQUAL( adm.get_request().log_category, l7vs::LOG_CAT_L7VSD_NETWORK );
        // unit_test[289] parse_log_func normal case 1 logcategory check
        BOOST_CHECK_EQUAL( adm.get_request().log_level, l7vs::LOG_LV_DEBUG );
    }

    // parse_log_func error case 1 (no logcategory)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_LOG;
        int        argc    = 4;
        char*    argv[]    = {    "l7vsadm_test",
                            "-L",
                            "-l",
                            "debug"
                            };
    
        bool ret = adm.parse_log_func_wp( cmd, argc, argv );

        // unit_test[290] parse_log_func error case 1 (no logcategory) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_log_func error case 2 (no loglevel)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_LOG;
        int        argc    = 4;
        char*    argv[]    = {    "l7vsadm_test",
                            "-L",
                            "-c",
                            "l7vsd_network"
                            };
    
        bool ret = adm.parse_log_func_wp( cmd, argc, argv );

        // unit_test[291] parse_log_func error case 2 (no loglevel) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_log_func_test end -----" );

}

void    parse_opt_snmp_log_category_func_test(){
    BOOST_MESSAGE( "----- parse_opt_snmp_log_category_func_test start -----" );

    // parse_opt_snmp_log_category_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-S", "-c", "snmpagent_start_stop" };
    
        bool ret = adm.parse_opt_snmp_log_category_func_wp( pos, argc, argv );

        // unit_test[292] parse_opt_snmp_log_category_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[293] parse_opt_snmp_log_category_func normal case 1 logcategory check
        BOOST_CHECK_EQUAL( adm.get_request().snmp_log_category, l7vs::LOG_CAT_SNMPAGENT_START_STOP );
    }

    // parse_opt_snmp_log_category_func error case 1 (invalid logcategory value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-S", "-c", "snmpagent" };
    
        bool ret = adm.parse_opt_snmp_log_category_func_wp( pos, argc, argv );

        // unit_test[294] parse_opt_snmp_log_category_func error case 1 (invalid logcategory value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_snmp_log_category_func error case 2 (no logcategory value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-S", "-c" };
    
        bool ret = adm.parse_opt_snmp_log_category_func_wp( pos, argc, argv );

        // unit_test[295] parse_opt_snmp_log_category_func error case 2 (no logcategory value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_snmp_log_category_func_test end -----" );

}

void    parse_opt_snmp_log_level_func_test(){
    BOOST_MESSAGE( "----- parse_opt_snmp_log_level_func_test start -----" );

    // parse_opt_snmp_log_level_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-S", "-l", "info" };
    
        bool ret = adm.parse_opt_snmp_log_level_func_wp( pos, argc, argv );

        // unit_test[296] parse_opt_snmp_log_level_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[297] parse_opt_snmp_log_level_func normal case 1 loglevel check
        BOOST_CHECK_EQUAL( adm.get_request().snmp_log_level, l7vs::LOG_LV_INFO );
    }

    // parse_opt_snmp_log_level_func error case 1 (invalid loglevel value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-S", "-l", "snmpagent" };
    
        bool ret = adm.parse_opt_snmp_log_level_func_wp( pos, argc, argv );

        // unit_test[298] parse_opt_snmp_log_level_func error case 1 (invalid loglevel value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_snmp_log_level_func error case 2 (no loglevel value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-S", "-l" };
    
        bool ret = adm.parse_opt_snmp_log_level_func_wp( pos, argc, argv );

        // unit_test[299] parse_opt_snmp_log_level_func error case 2 (no loglevel value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_snmp_log_level_func_test end -----" );

}

void    parse_snmp_func_test(){
    BOOST_MESSAGE( "----- parse_snmp_func_test start -----" );

    // parse_snmp_func normal case 1
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_SNMP;
        int        argc    = 6;
        char*    argv[]    = {    "l7vsadm_test",
                            "-S",
                            "-c",
                            "snmpagent_start_stop",
                            "-l",
                            "warn"
                            };
    
        bool ret = adm.parse_snmp_func_wp( cmd, argc, argv );

        // unit_test[300] parse_snmp_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[301] parse_snmp_func normal case 1 request command check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_SNMP );
        // unit_test[302] parse_snmp_func normal case 1 logcategory check
        BOOST_CHECK_EQUAL( adm.get_request().snmp_log_category, l7vs::LOG_CAT_SNMPAGENT_START_STOP );
        // unit_test[303] parse_snmp_func normal case 1 logcategory check
        BOOST_CHECK_EQUAL( adm.get_request().snmp_log_level, l7vs::LOG_LV_WARN );
    }

    // parse_snmp_func error case 1 (no logcategory)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_SNMP;
        int        argc    = 4;
        char*    argv[]    = {    "l7vsadm_test",
                            "-S",
                            "-l",
                            "debug"
                            };
    
        bool ret = adm.parse_snmp_func_wp( cmd, argc, argv );

        // unit_test[304] parse_snmp_func error case 1 (no logcategory) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_snmp_func error case 2 (no loglevel)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_SNMP;
        int        argc    = 4;
        char*    argv[]    = {    "l7vsadm_test",
                            "-S",
                            "-c",
                            "snmpagent_start_stop"
                            };
    
        bool ret = adm.parse_snmp_func_wp( cmd, argc, argv );

        // unit_test[305] parse_snmp_func error case 2 (no loglevel) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_snmp_func_test end -----" );

}

void    parse_opt_parameter_reload_func_test(){
    BOOST_MESSAGE( "----- parse_opt_parameter_reload_func_test start -----" );

    // parse_opt_parameter_reload_func normal case 1
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-P", "-r", "replication" };
    
        bool ret = adm.parse_opt_parameter_reload_func_wp( pos, argc, argv );

        // unit_test[306] parse_opt_parameter_reload_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[307] parse_opt_parameter_reload_func normal case 1 reload_component check
        BOOST_CHECK_EQUAL( adm.get_request().reload_param, l7vs::PARAM_COMP_REPLICATION );
    }

    // parse_opt_parameter_reload_func error case 1 (invalid reload_component value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 4;
        char*    argv[]    = { "l7vsadm_test", "-P", "-r", "lsock" };
    
        bool ret = adm.parse_opt_parameter_reload_func_wp( pos, argc, argv );

        // unit_test[308] parse_opt_parameter_reload_func error case 1 (invalid reload_component value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    // parse_opt_parameter_reload_func error case 2 (no reload_component value)
    {
        l7vsadm_test    adm;
        int        pos        = 2;
        int        argc    = 3;
        char*    argv[]    = { "l7vsadm_test", "-P", "-r" };
    
        bool ret = adm.parse_opt_parameter_reload_func_wp( pos, argc, argv );

        // unit_test[309] parse_opt_parameter_reload_func error case 2 (no reload_component value) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_opt_parameter_reload_func_test end -----" );

}

void    parse_parameter_func_test(){
    BOOST_MESSAGE( "----- parse_parameter_func_test start -----" );

    // parse_parameter_func normal case 1
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_PARAMETER;
        int        argc    = 4;
        char*    argv[]    = {    "l7vsadm_test",
                            "-P",
                            "-r",
                            "logger"
                            };
    
        bool ret = adm.parse_parameter_func_wp( cmd, argc, argv );

        // unit_test[310] parse_parameter_func normal case 1 return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[311] parse_parameter_func normal case 1 request command check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_PARAMETER );
        // unit_test[312] parse_parameter_func normal case 1 reload_component check
        BOOST_CHECK_EQUAL( adm.get_request().reload_param, l7vs::PARAM_COMP_LOGGER );
    }

    // parse_parameter_func error case 1 (no reload_component)
    {
        l7vsadm_test    adm;
        l7vs::l7vsadm_request::COMMAND_CODE_TAG    cmd = l7vs::l7vsadm_request::CMD_PARAMETER;
        int        argc    = 2;
        char*    argv[]    = {    "l7vsadm_test",
                            "-P"
                            };
    
        bool ret = adm.parse_parameter_func_wp( cmd, argc, argv );

        // unit_test[313] parse_parameter_func error case 1 (no reload_component) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- parse_parameter_func_test end -----" );

}

void    parse_opt_vs_fwdmode_func_test(){
    BOOST_MESSAGE( "----- parse_opt_vs_fwdmode_func_test start -----" );

    // parse_opt_vs_fwdmode_func normal case 1 (masq)
    {
        l7vsadm_test  adm;
        int      pos  = 2;
        int      argc = 3;
        char*    argv1[] = { "l7vsadm_test", "-A", "-M" };
        char*    argv2[] = { "l7vsadm_test", "-A", "--masq" };
        bool     ret;

        ret = adm.parse_opt_vs_fwdmode_func_wp( pos, argc, argv1 );
        BOOST_CHECK_EQUAL( ret, true );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_fwdmode, 1 );

        ret = adm.parse_opt_vs_fwdmode_func_wp( pos, argc, argv2 );
        BOOST_CHECK_EQUAL( ret, true );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_fwdmode, 1 );
    }

    // parse_opt_vs_fwdmode_func normal case 2 (tproxy)
    {
        l7vsadm_test  adm;
        int      pos  = 2;
        int      argc = 3;
        char*    argv1[] = { "l7vsadm_test", "-A", "-T" };
        char*    argv2[] = { "l7vsadm_test", "-A", "--tproxy" };
        bool     ret;

        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        ret = adm.parse_opt_vs_fwdmode_func_wp( pos, argc, argv1 );
        BOOST_CHECK_EQUAL( ret, true );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_fwdmode, 2 );

        ret = adm.parse_opt_vs_fwdmode_func_wp( pos, argc, argv2 );
        BOOST_CHECK_EQUAL( ret, true );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_fwdmode, 2 );
    }

    BOOST_MESSAGE( "----- parse_opt_vs_fwdmode_func_test end -----" );

}

void    parse_opt_rs_fwdmode_func_test(){
    BOOST_MESSAGE( "----- parse_opt_rs_fwdmode_func_test start -----" );

    // parse_opt_rs_fwdmode_func normal case 1 (masq)
    {
        l7vsadm_test  adm;
        int      pos  = 2;
        int      argc = 3;
        char*    argv1[] = { "l7vsadm_test", "-a", "-M" };
        char*    argv2[] = { "l7vsadm_test", "-a", "--masq" };
        bool     ret;

        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        ret = adm.parse_opt_rs_fwdmode_func_wp( pos, argc, argv1 );
        BOOST_CHECK_EQUAL( ret, true );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().fwdmode, 1 );

        ret = adm.parse_opt_rs_fwdmode_func_wp( pos, argc, argv2 );
        BOOST_CHECK_EQUAL( ret, true );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().fwdmode, 1 );
    }

    // parse_opt_rs_fwdmode_func normal case 2 (tproxy)
    {
        l7vsadm_test  adm;
        int      pos  = 2;
        int      argc = 3;
        char*    argv1[] = { "l7vsadm_test", "-a", "-T" };
        char*    argv2[] = { "l7vsadm_test", "-a", "--tproxy" };
        bool     ret;

        adm.get_request().vs_element.realserver_vector.push_back( l7vs::realserver_element() );

        ret = adm.parse_opt_rs_fwdmode_func_wp( pos, argc, argv1 );
        BOOST_CHECK_EQUAL( ret, true );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().fwdmode, 2 );

        ret = adm.parse_opt_rs_fwdmode_func_wp( pos, argc, argv2 );
        BOOST_CHECK_EQUAL( ret, true );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.realserver_vector.front().fwdmode, 2 );
    }

    BOOST_MESSAGE( "----- parse_opt_rs_fwdmode_func_test end -----" );

}

void    execute_test(){
    BOOST_MESSAGE( "----- execute_test start -----" );

    // execute normal case 1 (no option)
    {
        l7vsadm_test    adm;
        int        argc    = 1;
        char*    argv[]    = {    "l7vsadm_test" };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        typedef std::list< l7vs::virtualservice_element >    vse_list_type;
        for(    vse_list_type::iterator itr = test_response.virtualservice_status_list.begin();
                itr != test_response.virtualservice_status_list.end();
                ++itr ){
            itr->protocol_module_name = "protomod";
            itr->schedule_module_name = "sched";
            itr->realserver_vector.push_back( l7vs::realserver_element() );
            itr->realserver_vector.push_back( l7vs::realserver_element() );
        }

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            // accept ready wait
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[314] execute normal case 1 (no option) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[315] execute normal case 1 (no option) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_LIST );
        // unit_test[316] execute normal case 1 (no option) response value check
        BOOST_CHECK_EQUAL( adm.get_response().virtualservice_status_list.size(), 2U );
    }


    // execute normal case 2 (list operation)
    {
        l7vsadm_test    adm;
        int        argc    = 2;
        char*    argv[]    = {    "l7vsadm_test",
                            "-l"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        typedef std::list< l7vs::virtualservice_element >    vse_list_type;
        for(    vse_list_type::iterator itr = test_response.virtualservice_status_list.begin();
                itr != test_response.virtualservice_status_list.end();
                ++itr ){
            itr->protocol_module_name = "protomod";
            itr->schedule_module_name = "sched";
            itr->realserver_vector.push_back( l7vs::realserver_element() );
            itr->realserver_vector.push_back( l7vs::realserver_element() );
        }

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[317] execute normal case 2 (list operation) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[318] execute normal case 2 (list operation) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_LIST );
        // unit_test[319] execute normal case 2 (list operation) response value check
        BOOST_CHECK_EQUAL( adm.get_response().virtualservice_status_list.size(), 2U );
    }

    // execute normal case 3 (list operation numeric)
    {
        l7vsadm_test    adm;
        int        argc    = 3;
        char*    argv[]    = {    "l7vsadm_test",
                            "-l",
                            "-n"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        typedef std::list< l7vs::virtualservice_element >    vse_list_type;
        for(    vse_list_type::iterator itr = test_response.virtualservice_status_list.begin();
                itr != test_response.virtualservice_status_list.end();
                ++itr ){
            itr->protocol_module_name = "protomod";
            itr->schedule_module_name = "sched";
            itr->realserver_vector.push_back( l7vs::realserver_element() );
            itr->realserver_vector.push_back( l7vs::realserver_element() );
        }

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[320] execute normal case 3 (list operation numeric) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[321] execute normal case 3 (list operation numeric) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_LIST );
        // unit_test[322] execute normal case 3 (list operation numeric) response value check
        BOOST_CHECK_EQUAL( adm.get_response().virtualservice_status_list.size(), 2U );

    }

    // execute normal case 4 (list operation key)
    {
        l7vsadm_test    adm;
        int        argc    = 2;
        char*    argv[]    = {    "l7vsadm_test",
                            "-K"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        typedef std::list< l7vs::virtualservice_element >    vse_list_type;
        for(    vse_list_type::iterator itr = test_response.virtualservice_status_list.begin();
                itr != test_response.virtualservice_status_list.end();
                ++itr ){
            itr->protocol_module_name = "protomod";
            itr->schedule_module_name = "sched";
            itr->realserver_vector.push_back( l7vs::realserver_element() );
            itr->realserver_vector.push_back( l7vs::realserver_element() );
        }

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[323] execute normal case 4 (list operation key) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[324] execute normal case 4 (list operation key) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_LIST_KEY );
        // unit_test[325] execute normal case 4 (list operation key) response value check
        BOOST_CHECK_EQUAL( adm.get_response().virtualservice_status_list.size(), 2U );
    }

    // execute normal case 5 (list operation verbose)
    {
        l7vsadm_test    adm;
        int        argc    = 2;
        char*    argv[]    = {    "l7vsadm_test",
                            "-V"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        test_response.virtualservice_status_list.push_back( l7vs::virtualservice_element() );
        typedef std::list< l7vs::virtualservice_element >    vse_list_type;
        for(    vse_list_type::iterator itr = test_response.virtualservice_status_list.begin();
                itr != test_response.virtualservice_status_list.end();
                ++itr ){
            itr->protocol_module_name = "protomod";
            itr->schedule_module_name = "sched";
            itr->realserver_vector.push_back( l7vs::realserver_element() );
            itr->realserver_vector.push_back( l7vs::realserver_element() );
        }

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[326] execute normal case 5 (list operation verbose) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[327] execute normal case 5 (list operation verbose) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_LIST_VERBOSE );
        // unit_test[328] execute normal case 5 (list operation verbose) response value check
        BOOST_CHECK_EQUAL( adm.get_response().virtualservice_status_list.size(), 2U );
    }

    // execute normal case 6 (vs operation add-vs)
    {
        l7vsadm_test    adm;
        int        argc    = 17;
        char*    argv[]    = {    "l7vsadm_test",
                            "-A",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-s",
                            "lc",
                            "-u",
                            "50",
                            "-b",
                            "10.144.169.86:8080",
                            "-Q",
                            "100M",
                            "-q",
                            "200M"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[329] execute normal case 6 (vs operation add-vs) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[330] execute normal case 6 (vs operation add-vs) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_ADD_VS );
        // unit_test[331] execute normal case 6 (vs operation add-vs) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( test_request.vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[332] execute normal case 6 (vs operation add-vs) protocol module name check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_module_name, "cinsert" );
        // unit_test[333] execute normal case 6 (vs operation add-vs) protocol module arg check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[334] execute normal case 6 (vs operation add-vs) schedule module name check
        BOOST_CHECK_EQUAL( test_request.vs_element.schedule_module_name, "lc" );
        // unit_test[335] execute normal case 6 (vs operation add-vs) sorry_maxconnection check
        BOOST_CHECK_EQUAL( test_request.vs_element.sorry_maxconnection, 50 );
        // unit_test[336] execute normal case 6 (vs operation add-vs) sorry_endpoint check
        boost::asio::ip::tcp::endpoint    sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( test_request.vs_element.sorry_endpoint, sorry_ep );
        // unit_test[337] execute normal case 6 (vs operation add-vs) qos_upstream check
        BOOST_CHECK_EQUAL( test_request.vs_element.qos_upstream, ( 100000000ULL / 8 ) );
        // unit_test[338] execute normal case 6 (vs operation add-vs) qos_downstream check
        BOOST_CHECK_EQUAL( test_request.vs_element.qos_downstream, ( 200000000ULL / 8 ) );
    }

    // execute normal case 7 (vs operation edit-vs)
    {
        l7vsadm_test    adm;
        int        argc    = 19;
        char*    argv[]    = {    "l7vsadm_test",
                            "-E",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-s",
                            "rr",
                            "-u",
                            "50",
                            "-b",
                            "10.144.169.86:8080",
                            "-f",
                            "1",
                            "-Q",
                            "100M",
                            "-q",
                            "200M"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[339] execute normal case 7 (vs operation edit-vs) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[340] execute normal case 7 (vs operation edit-vs) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_EDIT_VS );
        // unit_test[341] execute normal case 7 (vs operation edit-vs) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( test_request.vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[342] execute normal case 7 (vs operation edit-vs) protocol module name check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_module_name, "cinsert" );
        // unit_test[343] execute normal case 7 (vs operation edit-vs) protocol module arg check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[344] execute normal case 7 (vs operation edit-vs) schedule module name check
        BOOST_CHECK_EQUAL( test_request.vs_element.schedule_module_name, "rr" );
        // unit_test[345] execute normal case 7 (vs operation edit-vs) sorry_maxconnection check
        BOOST_CHECK_EQUAL( test_request.vs_element.sorry_maxconnection, 50 );
        // unit_test[346] execute normal case 7 (vs operation edit-vs) sorry_endpoint check
        boost::asio::ip::tcp::endpoint    sorry_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( test_request.vs_element.sorry_endpoint, sorry_ep );
        // unit_test[347] execute normal case 7 (vs operation edit-vs) sorry_flag check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.sorry_flag, true );
        // unit_test[348] execute normal case 7 (vs operation edit-vs) qos_upstream check
        BOOST_CHECK_EQUAL( test_request.vs_element.qos_upstream, ( 100000000ULL / 8 ) );
        // unit_test[349] execute normal case 7 (vs operation edit-vs) qos_downstream check
        BOOST_CHECK_EQUAL( test_request.vs_element.qos_downstream, ( 200000000ULL / 8 ) );
    }

    // execute normal case 8 (vs operation del-vs)
    {
        l7vsadm_test    adm;
        int        argc    = 7;
        char*    argv[]    = {    "l7vsadm_test",
                            "-D",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[350] execute normal case 8 (vs operation del-vs) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[351] execute normal case 8 (vs operation del-vs) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_DEL_VS );
        // unit_test[352] execute normal case 8 (vs operation del-vs) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[353] execute normal case 8 (vs operation del-vs) protocol module name check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_module_name, "cinsert" );
        // unit_test[354] execute normal case 8 (vs operation del-vs) protocol module arg check
        BOOST_CHECK_EQUAL( adm.get_request().vs_element.protocol_args.front(), "mod_arg" );
    }

    // execute normal case 9 (vs operation flush-vs)
    {
        l7vsadm_test    adm;
        int        argc    = 2;
        char*    argv[]    = {    "l7vsadm_test",
                            "-C"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[355] execute normal case 9 (vs operation flush-vs) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[356] execute normal case 9 (vs operation flush-vs) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_FLUSH_VS );
    }

    // execute normal case 10 (rs operation add-rs)
    {
        l7vsadm_test    adm;
        int        argc    = 11;
        char*    argv[]    = {    "l7vsadm_test",
                            "-a",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "10.144.169.86:8080",
                            "-w",
                            "10"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[357] execute normal case 10 (rs operation add-rs) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[358] execute normal case 10 (rs operation add-rs) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_ADD_RS );
        // unit_test[359] execute normal case 10 (rs operation add-rs) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( test_request.vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[360] execute normal case 10 (rs operation add-rs) protocol module name check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_module_name, "cinsert" );
        // unit_test[361] execute normal case 10 (rs operation add-rs) protocol module arg check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[362] execute normal case 10 (rs operation add-rs) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( test_request.vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
        // unit_test[363] execute normal case 10 (rs operation add-rs) weight check
        BOOST_CHECK_EQUAL( test_request.vs_element.realserver_vector.front().weight, 10 );
    }

    // execute normal case 11 (rs operation edit-rs)
    {
        l7vsadm_test    adm;
        int        argc    = 11;
        char*    argv[]    = {    "l7vsadm_test",
                            "-e",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "10.144.169.86:8080",
                            "-w",
                            "20"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[364] execute normal case 11 (rs operation edit-rs) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[365] execute normal case 11 (rs operation edit-rs) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_EDIT_RS );
        // unit_test[366] execute normal case 11 (rs operation edit-rs) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( test_request.vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[367] execute normal case 11 (rs operation edit-rs) protocol module name check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_module_name, "cinsert" );
        // unit_test[368] execute normal case 11 (rs operation edit-rs) protocol module arg check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[369] execute normal case 11 (rs operation edit-rs) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( test_request.vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
        // unit_test[370] execute normal case 11 (rs operation edit-rs) weight check
        BOOST_CHECK_EQUAL( test_request.vs_element.realserver_vector.front().weight, 20 );
    }

    // execute normal case 12 (rs operation del-rs)
    {
        l7vsadm_test    adm;
        int        argc    = 9;
        char*    argv[]    = {    "l7vsadm_test",
                            "-d",
                            "-t",
                            "10.144.169.87:22100",
                            "-m",
                            "cinsert",
                            "mod_arg",
                            "-r",
                            "10.144.169.86:8080"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[371] execute normal case 12 (rs operation del-rs) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[372] execute normal case 12 (rs operation del-rs) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_DEL_RS );
        // unit_test[373] execute normal case 12 (rs operation del-rs) tcp_accept_endpoint check
        boost::asio::ip::tcp::endpoint    tcp_acc_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.87:22100" );
        BOOST_CHECK_EQUAL( test_request.vs_element.tcp_accept_endpoint, tcp_acc_ep );
        // unit_test[374] execute normal case 12 (rs operation del-rs) protocol module name check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_module_name, "cinsert" );
        // unit_test[375] execute normal case 12 (rs operation del-rs) protocol module arg check
        BOOST_CHECK_EQUAL( test_request.vs_element.protocol_args.front(), "mod_arg" );
        // unit_test[376] execute normal case 12 (rs operation del-rs) realserver endpoint check
        boost::asio::ip::tcp::endpoint    rs_ep = string_to_endpoint<boost::asio::ip::tcp>( "10.144.169.86:8080" );
        BOOST_CHECK_EQUAL( test_request.vs_element.realserver_vector.front().tcp_endpoint, rs_ep );
    }

    // execute normal case 13 (replication operation)
    {
        l7vsadm_test    adm;
        int        argc    = 4;
        char*    argv[]    = {    "l7vsadm_test",
                            "-R",
                            "-s",
                            "start"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[377] execute normal case 13 (replication operation) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[378] execute normal case 13 (replication operation) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_REPLICATION );
        // unit_test[379] execute normal case 13 (replication operation) replication command check
        BOOST_CHECK_EQUAL( test_request.replication_command, l7vs::l7vsadm_request::REP_START );
    }

    // execute normal case 14 (log operation)
    {
        l7vsadm_test    adm;
        int        argc    = 6;
        char*    argv[]    = {    "l7vsadm_test",
                            "-L",
                            "-c",
                            "l7vsd_network",
                            "-l",
                            "debug"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[380] execute normal case 14 (log operation) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[381] execute normal case 14 (log operation) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_LOG );
        // unit_test[382] execute normal case 14 (log operation) logcategory check
        BOOST_CHECK_EQUAL( test_request.log_category, l7vs::LOG_CAT_L7VSD_NETWORK );
        // unit_test[383] execute normal case 14 (log operation) logcategory check
        BOOST_CHECK_EQUAL( test_request.log_level, l7vs::LOG_LV_DEBUG );
    }

    // execute normal case 15 (snmp log operation)
    {
        l7vsadm_test    adm;
        int        argc    = 6;
        char*    argv[]    = {    "l7vsadm_test",
                            "-S",
                            "-c",
                            "snmpagent_start_stop",
                            "-l",
                            "warn"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[384] execute normal case 15 (snmp log operation) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[385] execute normal case 15 (snmp log operation) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_SNMP );
        // unit_test[386] execute normal case 15 (snmp log operation) logcategory check
        BOOST_CHECK_EQUAL( test_request.snmp_log_category, l7vs::LOG_CAT_SNMPAGENT_START_STOP );
        // unit_test[387] execute normal case 15 (snmp log operation) logcategory check
        BOOST_CHECK_EQUAL( test_request.snmp_log_level, l7vs::LOG_LV_WARN );
    }

    // execute normal case 16 (parameter operation)
    {
        l7vsadm_test    adm;
        int        argc    = 4;
        char*    argv[]    = {    "l7vsadm_test",
                            "-P",
                            "-r",
                            "logger"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();
        test_response.status = l7vs::l7vsd_response::RESPONSE_OK;

        accept_ready = false;
        boost::thread    thd1( &server_thread );
        {
            //boost::mutex::scoped_lock    lock( accept_mutex );
            //accept_condition.wait( lock );
            for(;;) { if(accept_ready)    break; }
        }
        bool ret = adm.execute_wp( argc, argv );
        thd1.join();

        // unit_test[388] execute normal case 16 (parameter operation) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[389] execute normal case 16 (parameter operation) request command check
        BOOST_CHECK_EQUAL( test_request.command, l7vs::l7vsadm_request::CMD_PARAMETER );
        // unit_test[390] execute normal case 16 (parameter operation) reload_component check
        BOOST_CHECK_EQUAL( test_request.reload_param, l7vs::PARAM_COMP_LOGGER );
    }

    // execute normal case 17 (help operation)
    {
        l7vsadm_test    adm;
        int        argc    = 2;
        char*    argv[]    = {    "l7vsadm_test",
                            "-h"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();

        bool ret = adm.execute_wp( argc, argv );

        // unit_test[391] execute normal case 17 (help operation) return value check
        BOOST_CHECK_EQUAL( ret, true );    
        // unit_test[392] execute normal case 17 (help operation) request command check
        BOOST_CHECK_EQUAL( adm.get_request().command, l7vs::l7vsadm_request::CMD_HELP );
    }

    // execute error case 1 (invalid operation)
    {
        l7vsadm_test    adm;
        int        argc    = 2;
        char*    argv[]    = {    "l7vsadm_test",
                            "-Z"
                            };

        test_request = l7vs::l7vsadm_request();
        test_response = l7vs::l7vsd_response();

        bool ret = adm.execute_wp( argc, argv );

        // unit_test[393] execute 1 (invalid operation) return value check
        BOOST_CHECK_EQUAL( ret, false );    
    }

    BOOST_MESSAGE( "----- execute_test end -----" );

}

void    l7vsadm_test::file_lock_class_test(){
    BOOST_MESSAGE( "----- file_lock_class_test start -----" );

    char file_path[256];
    memset(file_path, 0, sizeof(file_path));
    readlink("/proc/self/exe", file_path, sizeof(file_path));

    {
        l7vs::error_code    err;
    
        // unit_test[394] file_lock_class normal case 1 (first lock) error check
        l7vs::l7vsadm::file_lock    lock( file_path, err );
        BOOST_CHECK( !err );    
    
        // unit_test[395] file_lock_class normal case 1 (first lock) locable check
        bool ret = lock.try_lock();
        BOOST_CHECK( ret );
    
        l7vs::error_code    err2;
        // unit_test[396] file_lock_class normal case 2 (second lock) error check
        l7vs::l7vsadm::file_lock    lock2( file_path, err2 );
        BOOST_CHECK( !err );    
    
        // unit_test[397] file_lock_class normal case 2 (second lock) unlocable check
        ret = lock2.try_lock();
        BOOST_CHECK( !ret );
    }

    {
        l7vs::error_code    err;
    
        // unit_test[398] file_lock_class normal case 3 (relock) error check
        l7vs::l7vsadm::file_lock    lock( file_path, err );
        BOOST_CHECK( !err );    
    
        // unit_test[399] file_lock_class normal case 3 (relock) locable check
        bool ret = lock.try_lock();
        BOOST_CHECK( ret );
    }

    BOOST_MESSAGE( "----- file_lock_class_test end -----" );

}


test_suite*    init_unit_test_suite( int argc, char* argv[] ){
    test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

    l7vs::Logger    logger_instance;
    l7vs::Parameter    parameter_instance;
    logger_instance.loadConf();

    ts->add( BOOST_TEST_CASE( &parse_list_func_test ) );

    ts->add( BOOST_TEST_CASE( &parse_opt_vs_target_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_module_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_scheduler_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_upper_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_bypass_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_qosup_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_qosdown_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_udp_func_test ) );

    ts->add( BOOST_TEST_CASE( &parse_opt_vs_ssl_file_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_access_log_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_access_log_logrotate_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_vs_socket_func_test ) );

    ts->add( BOOST_TEST_CASE( &parse_vs_func_test ) );

    ts->add( BOOST_TEST_CASE( &parse_opt_rs_weight_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_rs_realserver_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_rs_func_test ) );

    ts->add( BOOST_TEST_CASE( &parse_opt_replication_switch_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_replication_force_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_replication_dump_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_replication_func_test ) );

    ts->add( BOOST_TEST_CASE( &parse_opt_log_category_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_log_level_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_log_func_test ) );

    ts->add( BOOST_TEST_CASE( &parse_opt_snmp_log_category_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_snmp_log_level_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_snmp_func_test ) );

    ts->add( BOOST_TEST_CASE( &parse_opt_parameter_reload_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_parameter_func_test ) );

    ts->add( BOOST_TEST_CASE( &parse_opt_vs_fwdmode_func_test ) );
    ts->add( BOOST_TEST_CASE( &parse_opt_rs_fwdmode_func_test ) );

    ts->add( BOOST_TEST_CASE( &execute_test ) );

    ts->add( BOOST_TEST_CASE( &l7vsadm_test::file_lock_class_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}
