#define    TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/multi_array.hpp>

#include "error_code.h"

//#include "logger_enum.h"
//#include "logger_stub.h"
#include "logger_access_stub.h"
#include "logger.h"

////#include "parameter_enum.h"
////#include "parameter_stub.h"
//#include "parameter.h"
#include "parameter_stub.h"

#include "command_receiver_stub.h"
#include "virtualservice_stub.h"
//#include "virtualservice_element_stub.h"
#include "realserver_stub.h"
//#include "realserver_element_stub.h"
#include "replication_stub.h"
#include "snmpbridge_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"
#include "utility.h"

#include "../../src/l7vsd.cpp"

using namespace boost::unit_test_framework;

// test class
class    l7vsd_test    :public    l7vs::l7vsd {
public:
    vslist_type&    get_vslist(){ return vslist; }
    boost::thread_group&    get_tg(){ return vs_threads; }

    bool&    get_help() { return help; }
//    bool&    get_debug() { return debug; }

    vslist_type::iterator    search_vslist( l7vs::virtualservice_element& in_elem ){
        return l7vsd::search_vslist( in_elem );
    }
    void    set_replication( boost::shared_ptr< l7vs::replication > inrep ){
        rep = inrep;
    }
    void    set_snmpbridge( boost::shared_ptr< l7vs::snmpbridge > inbridge ){
        bridge = inbridge;
    }

};
//variable
boost::multi_array< l7vs::virtualservice_element, 2 > vselement( boost::extents[2][100] );
int    err_count[2] = {0, 0};
l7vsd_test    vsd_test;

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
void    list_vs( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        l7vs::error_code err;
        l7vs::l7vsd::vselist_type    vselist;

        vsd_test.list_virtual_service( &vselist, err );
        if( err )    ++err_count[id];
    }
}

void    list_vs_verbose( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        l7vs::error_code err;
        l7vs::l7vsd_response    response;

        vsd_test.list_virtual_service_verbose( &response, err );
        if( err )    ++err_count[id];
    }
}

void    add_vs_del_vs( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        {
            l7vs::error_code err;
            vsd_test.add_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
        {
            l7vs::error_code err;
            vsd_test.del_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
    }
}

void    add_vs_edit_vs( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        {
            l7vs::error_code err;
            vsd_test.add_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
        {
            l7vs::error_code err;
            vsd_test.edit_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
        {
            l7vs::error_code err;
            vsd_test.del_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
    }
}

void    add_vs_add_rs( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        {
            l7vs::error_code err;
            vsd_test.add_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
        {
            l7vs::error_code err;
            vsd_test.add_real_server( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
        {
            l7vs::error_code err;
            vsd_test.del_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
    }
}

void    add_vs_del_rs( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        {
            l7vs::error_code err;
            vsd_test.add_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
        {
            l7vs::error_code err;
            vsd_test.del_real_server( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
        {
            l7vs::error_code err;
            vsd_test.del_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
    }
}

void    add_vs_edit_rs( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        {
            l7vs::error_code err;
            vsd_test.add_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
        {
            l7vs::error_code err;
            vsd_test.edit_real_server( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
        {
            l7vs::error_code err;
            vsd_test.del_virtual_service( &vselement[id][i], err );
            if( err )    ++err_count[id];
        }
    }
}

void    flush_vs( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        l7vs::error_code err;
        vsd_test.flush_virtual_service( err );
        if( err )    ++err_count[id];
    }
}

void    rep_command( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        l7vs::error_code err;
        l7vs::l7vsadm_request::REPLICATION_COMMAND_TAG    cmd = l7vs::l7vsadm_request::REP_START;

        vsd_test.replication_command( &cmd, err );
        if( err )    ++err_count[id];
    }
}

void    set_loglevel( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        l7vs::error_code err;
        l7vs::LOG_CATEGORY_TAG    cat        = l7vs::LOG_CAT_L7VSD_NETWORK;
        l7vs::LOG_LEVEL_TAG        level    = l7vs::LOG_LV_INFO;

        vsd_test.set_loglevel( &cat, &level, err );
        if( err )    ++err_count[id];
    }
}

void    snmp_set_loglevel( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        l7vs::error_code err;
        l7vs::LOG_CATEGORY_TAG    cat        = l7vs::LOG_CAT_SNMPAGENT_START_STOP;
        l7vs::LOG_LEVEL_TAG        level    = l7vs::LOG_LV_INFO;

        vsd_test.snmp_set_loglevel( &cat, &level, err );
        if( err )    ++err_count[id];
    }
}

void    reload_parameter( unsigned int id ){
    for( size_t i = 0 ; i < 100; ++i ){
        l7vs::error_code err;
        l7vs::PARAMETER_COMPONENT_TAG    comp = l7vs::PARAM_COMP_REPLICATION;

        vsd_test.reload_parameter( &comp, err );
        if( err )    ++err_count[id];
    }
}

void    thread_test(){
    BOOST_MESSAGE( "----- thread_test start -----" );

    boost::asio::io_service            io;
    boost::shared_ptr< l7vs::replication >
                                    rep( new l7vs::replication() );
    vsd_test.set_replication( rep );

    boost::shared_ptr< l7vs::snmpbridge >
                                    bridge( new l7vs::snmpbridge( vsd_test, io ) );
    vsd_test.set_snmpbridge( bridge );

    //make element
    for( size_t i = 0 ; i < 2; ++i ){
        for( size_t j = 0 ; j < 100; ++j ){
            std::stringstream buf;
            buf << boost::format( "%s.10.10.10:%04d" )
                % (i == 0 ? "10" : "20")
                % ((j + 1) * 10);
            vselement[i][j].tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>( buf.str() );
        }
    }
    
    typedef std::map<std::string, boost::function<void( unsigned int ) > > func_map_type;
    func_map_type func_map;

    func_map["list_vs"] = &list_vs;
    func_map["list_vs_verbose"] = &list_vs_verbose;
    func_map["add_vs_del_vs"] = &add_vs_del_vs;
    func_map["add_vs_edit_vs"] = &add_vs_edit_vs;
    func_map["add_vs_add_rs"] = &add_vs_add_rs;
    func_map["add_vs_del_rs"] = &add_vs_del_rs;
    func_map["add_vs_edit_rs"] = &add_vs_edit_rs;

    func_map["rep_command"] = &rep_command;
    func_map["set_loglevel"] = &set_loglevel;
    func_map["snmp_set_loglevel"] = &snmp_set_loglevel;
    func_map["reload_parameter"] = &reload_parameter;

    // command combination test
    for( func_map_type::iterator itr = func_map.begin();
         itr != func_map.end();
         ++itr ){

        for( func_map_type::iterator itr2 = func_map.begin();
            itr2 != func_map.end();
            ++itr2 ){

            err_count[0] = 0;
            err_count[1] = 0;

            boost::thread    thd1( boost::bind( itr->second, 0 ) );
            boost::thread    thd2( boost::bind( itr2->second, 1 ) );
            thd1.join();
            thd2.join();

            BOOST_MESSAGE( itr->first + " and " + itr2->first );
            BOOST_CHECK_EQUAL( err_count[0], 0 );
            BOOST_CHECK_EQUAL( err_count[1], 0 );

        }
    }


    // flush vs command combination test
    for( func_map_type::iterator itr = func_map.begin();
         itr != func_map.end();
         ++itr ){

        err_count[0] = 0;
        err_count[1] = 0;

        boost::thread    thd1( boost::bind( &flush_vs, 0 ) );
        boost::thread    thd2( boost::bind( itr->second, 1 ) );
        thd1.join();
        thd2.join();

        BOOST_MESSAGE( "flush and " + itr->first );
        BOOST_CHECK_EQUAL( err_count[0], 0 );

    }

    BOOST_MESSAGE( "----- thread_test end -----" );

}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){
    test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

    l7vs::Logger    logger_instance;
    l7vs::Parameter    parameter_instance;
    logger_instance.loadConf();

    ts->add( BOOST_TEST_CASE( &thread_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}
