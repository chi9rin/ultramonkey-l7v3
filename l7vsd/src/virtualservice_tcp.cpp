/*!
 *    @file    virtualservice_tcp.cpp
 *    @brief    VirtualService class implementations for TCP
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#include <vector>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "virtualservice.h"
#include "logger_enum.h"
#include "logger.h"
#include "parameter.h"

#include "utility.h"

#include "logger_implement_access.h"

//static int new_session_cb(SSL *ssl, SSL_SESSION *session);
//static SSL_SESSION *get_session_cb(SSL *ssl, unsigned char *ssid, int ssid_len, int *ref);
//static void remove_session_cb(SSL_CTX *ssl_ctx, SSL_SESSION *session);


// imprementation for virtualservice_tcp
/*!
 * virtualservice_tcp class constructor.
 */
l7vs::virtualservice_tcp::virtualservice_tcp(const l7vsd& invsd,
                         const replication& inrep,
                         const virtualservice_element& inelement )
                         :
                         virtualservice_base( invsd, inrep, inelement ),
                         acceptor_( dispatcher ),
                         sslcontext(dispatcher, DEFAULT_SSL_METHOD)
{
}
/*!
 * virtualservice_tcp class destructor.
 */
l7vs::virtualservice_tcp::~virtualservice_tcp(){
}

/*!
 * replication interrupt event
 *
 * @param   err
 * @return  void
 */
void    l7vs::virtualservice_tcp::handle_replication_interrupt( const boost::system::error_code& err ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format    fmt("in_function : void virtualservice_tcp::handle_replication_interrupt( "
                            "const boost::system::error_code& err ) : err = %s, err.message = %s");
        fmt % ( err ? "true" : "false") % err.message();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 41, fmt.str(), __FILE__, __LINE__ );
    }

    if( unlikely( err ) ){
        //ERROR case
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 3, err.message(), __FILE__, __LINE__ );
        return;
    }

    bool    newdata    = true;
    replication&    rep_noconst = const_cast<replication&>( rep );

    if( replication::REPLICATION_SINGLE == rep_noconst.get_status() ){
        Logger::putLogInfo( LOG_CAT_L7VSD_VIRTUALSERVICE, 1, "replication mode is single.", __FILE__, __LINE__ );
        return;
    }

    std::stringstream    tmp_tcp_ep;
    tmp_tcp_ep        << element.tcp_accept_endpoint;
    std::stringstream    tmp_udp_ep;
    tmp_udp_ep        << element.udp_recv_endpoint;
    std::stringstream    tmp_sorry_ep;
    tmp_sorry_ep    << element.sorry_endpoint;
    
    //get replication area
    unsigned int    rep_size = 0;
    replication_header*    rep_header_ptr = reinterpret_cast<replication_header*>( rep_noconst.pay_memory( REP_AREA_NAME, rep_size) );
    if( (rep_header_ptr == NULL) || (0 == rep_size) ){
        Logger::putLogWarn( LOG_CAT_L7VSD_VIRTUALSERVICE, 1, REP_BLOCK_SIZE_ERR_MSG, __FILE__, __LINE__ );
        return;
    }

    //check maxdatasize
    if( ( rep_size * DATA_SIZE ) <
        ((sizeof(replication_data) * MAX_REPLICATION_DATA_NUM) + sizeof(replication_header)) ){
        Logger::putLogWarn( LOG_CAT_L7VSD_VIRTUALSERVICE, 2, REP_AREA_SIZE_ERR_MSG, __FILE__, __LINE__ );
        return;
    }

    //lock replication area
    rep_noconst.lock( REP_AREA_NAME );

    //read header value and set loop count
    unsigned int loop_cnt = rep_header_ptr->data_num;
    //if data_num over MAX_REPLICATION_DATA_NUM, data_num = 0
    if( static_cast<unsigned int>(MAX_REPLICATION_DATA_NUM) < loop_cnt )
        loop_cnt = 0;

    //set start pointer(pointer of replication_data)
    replication_data*    rep_data_ptr = reinterpret_cast<replication_data*>( ++rep_header_ptr );

    //find vs(loop)
    for( unsigned int i = 0; i < loop_cnt; ++i ){
        //check equal udpmode and tcp_accept_endpoint
        if( (rep_data_ptr->udpmode == element.udpmode )&&
            ( 0 == strncmp( rep_data_ptr->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) ) ){
            newdata = false;
            break;
        }
        //increment data pointer
        ++rep_data_ptr;
    }

    //if it is new data, increment data num.
    if( newdata ){
        rep_header_ptr = reinterpret_cast<replication_header*>( rep_noconst.pay_memory( REP_AREA_NAME, rep_size) );
        ++(rep_header_ptr->data_num);
    }
    //write replication data
    rep_data_ptr->udpmode        = element.udpmode;
    strncpy( rep_data_ptr->tcp_endpoint, tmp_tcp_ep.str().c_str(), 48 );
    strncpy( rep_data_ptr->udp_endpoint, tmp_udp_ep.str().c_str(), 48 );
    rep_data_ptr->sorry_maxconnection    = element.sorry_maxconnection;
    strncpy( rep_data_ptr->sorry_endpoint, tmp_sorry_ep.str().c_str(), 48 );
    rep_data_ptr->sorry_flag    = element.sorry_flag;
    rep_data_ptr->qos_up        = element.qos_upstream;
    rep_data_ptr->qos_down        = element.qos_downstream;

    //unlock replication area
    rep_noconst.unlock( REP_AREA_NAME );

    //register handle_replication_interrupt
    replication_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
    replication_timer->async_wait( boost::bind( &virtualservice_tcp::handle_replication_interrupt, 
                                            this, boost::asio::placeholders::error ) );

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 42, "out_function : void virtualservice_tcp::handle_replication_interrupt( const boost::system::error_code& err )", __FILE__, __LINE__ );
    }
}

/*!
 * read replication data
 *
 * @param   read_replicationdata
 * @param    error_code
 * @return  void
 */
void    l7vs::virtualservice_tcp::read_replicationdata(){
    Logger    funclog( LOG_CAT_L7VSD_VIRTUALSERVICE, 43, "function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );

    replication&    rep_noconst = const_cast<replication&>( rep );

    if( replication::REPLICATION_SINGLE == rep_noconst.get_status() ){
        Logger::putLogInfo( LOG_CAT_L7VSD_VIRTUALSERVICE, 2, "replication mode is single.", __FILE__, __LINE__ );
        return;
    }

    unsigned int    rep_size = 0;
    replication_header*    rep_header_ptr = reinterpret_cast<replication_header*>( rep_noconst.pay_memory( REP_AREA_NAME, rep_size) );
    if( (rep_header_ptr == NULL) || (0 == rep_size) ){
        Logger::putLogWarn( LOG_CAT_L7VSD_VIRTUALSERVICE, 3, REP_BLOCK_SIZE_ERR_MSG, __FILE__, __LINE__ );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 44, "out_function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );
        }
        return;
    }

    //check maxdatasize
    if( ( rep_size * DATA_SIZE ) <
        ((sizeof(replication_data) * MAX_REPLICATION_DATA_NUM) + sizeof(replication_header)) ){
        Logger::putLogWarn( LOG_CAT_L7VSD_VIRTUALSERVICE, 4, REP_AREA_SIZE_ERR_MSG, __FILE__, __LINE__ );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 45, "out_function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );
        }
        return;
    }
    //lock replication area
    rep_noconst.lock( REP_AREA_NAME );

    //read header value and set loop count
    unsigned int loop_cnt = rep_header_ptr->data_num;
    //if data_num over MAX_REPLICATION_DATA_NUM, set data_num = 0
    if( static_cast<unsigned int>(MAX_REPLICATION_DATA_NUM) < loop_cnt ){
        Logger::putLogWarn( LOG_CAT_L7VSD_VIRTUALSERVICE, 5, REP_BLOCK_SIZE_ERR_MSG, __FILE__, __LINE__ );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 46, "out_function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );
        }
        rep_header_ptr->data_num = 0;
        rep_noconst.unlock( REP_AREA_NAME );
        return;
    }

    //set start pointer(pointer of replication_data)
    replication_data*    rep_data_ptr = reinterpret_cast<replication_data*>( ++rep_header_ptr );

    //find vs(loop)
    for( unsigned int i = 0; i < loop_cnt; ++i ){
        //get tcp endpointdata
        std::string        tmpstr = rep_data_ptr->tcp_endpoint;
        std::string        ipaddr;
        unsigned short    portno;
        try{
            portno    = boost::lexical_cast<unsigned short>( tmpstr.substr( (tmpstr.rfind( ":" )+1) ) );
        }
        catch( const boost::bad_lexical_cast& err ){
            portno    = 0;
        }
        if( tmpstr[0] == '[' ){
            //IPv6 endpoint
            ipaddr    = tmpstr.substr( 1, (tmpstr.rfind( "]" )-1) );
        }else{
            //IPv4 endpoint
            ipaddr    = tmpstr.substr( 0, tmpstr.rfind( ":" ) );
        }
        //check equal udpmode and tcp_accept_endpoint
        if( (rep_data_ptr->udpmode == element.udpmode )&&
            ( tcp_endpoint_type( boost::asio::ip::address::from_string( ipaddr ), portno ) == element.tcp_accept_endpoint ) ){
                element.sorry_maxconnection    = rep_data_ptr->sorry_maxconnection;
            tmpstr    = rep_data_ptr->sorry_endpoint;
            try{
                portno    = boost::lexical_cast<unsigned short>( tmpstr.substr( (tmpstr.rfind( ":" )+1) ) );
            }
            catch( const boost::bad_lexical_cast& err ){
                portno    = 0;
            }
            if( tmpstr[0] == '[' ){
                //IPv6 endpoint
                ipaddr    = tmpstr.substr( 1, (tmpstr.rfind( "]" )-1) );
            }else{
                //IPv4 endpoint
                ipaddr    = tmpstr.substr( 0, tmpstr.rfind( ":" ) );
            }
            element.sorry_endpoint        = 
                    tcp_endpoint_type( boost::asio::ip::address::from_string( ipaddr ), portno );
            element.sorry_flag            = rep_data_ptr->sorry_flag;
            element.qos_upstream        = rep_data_ptr->qos_up;
            element.qos_downstream        = rep_data_ptr->qos_down;
            break;
        }
        //increment data pointer
        ++rep_data_ptr;
    }

    //unlock replication area
    rep_noconst.unlock( REP_AREA_NAME );

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 47, "out_function : void virtualservice_tcp::read_replicationdata()", __FILE__, __LINE__ );
    }
}

/*!
 * accept event
 *
 * @param   handle_accept
 * @param    error_code
 * @return  void
 */
void    l7vs::virtualservice_tcp::handle_accept( const l7vs::session_thread_control* stc_ptr, const boost::system::error_code& err ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format funclog_fmt("in_function : void virtualservice_tcp::handle_accept( "
                                "const boost::shared_ptr<session_thread_control> , "
                                "const boost::system::error_code& err ) : err = %s, err.message = %s");
        funclog_fmt % ( err ? "true" : "false") % err.message();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 48, funclog_fmt.str(), __FILE__, __LINE__ );
    }

    session_thread_control*        stc_ptr_noconst = const_cast<session_thread_control*>( stc_ptr );

    if( unlikely( err ) ){
        //ERROR case
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 4, err.message(), __FILE__, __LINE__ );
        return;
    }

    tcp_session*    tmp_session    = stc_ptr_noconst->get_session().get();

    if( is_session_cache_use == true ) {
        long ssl_cache_num = SSL_CTX_sess_number(sslcontext.impl());
        if ( ssl_cache_num >= session_cache_size ) {
            flush_ssl_session();
        }
    }

//    /* @001 virtualservice_element
    stc_ptr_noconst->session_access_log_output_mode_change( access_log_flag );
//    */

    active_sessions.insert( tmp_session, stc_ptr_noconst );

    //check sorry status
    if( unlikely( 0 < element.sorry_maxconnection ) ){
        if( ( ( active_sessions.size() - sorry_count.get() ) >= static_cast<size_t>( element.sorry_maxconnection ) ) ||
            ( ( 0 != element.sorry_flag ) && ( INT_MAX != element.sorry_flag ) ) ||
            ( ( 0 == rs_list.size() ) && ( boost::asio::ip::tcp::endpoint() != element.sorry_endpoint ) ) ){
            if( unlikely( LOG_LV_ERROR == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
                boost::format    fmt( "Connection switch Sorry mode. : active_session.size = %d / sorry_count.get = %d" );
                fmt % active_sessions.size() % pool_sessions.size();
                Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 49, fmt.str(), __FILE__, __LINE__ );
            }
            sorry_count++;
            stc_ptr_noconst->get_session()->set_virtual_service_message( tcp_session::SORRY_STATE_ENABLE );
        }
    }
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format    fmt1( "active session thread id = %d" );
        fmt1 % stc_ptr_noconst->get_upthread_id();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 50, fmt1.str(), __FILE__, __LINE__ );
        boost::format    fmt2( "pool_session.size   = %d" );
        fmt2 % pool_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 51, fmt2.str(), __FILE__, __LINE__ );
        boost::format    fmt3( "active_session.size = %d" );
        fmt3 % active_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 52, fmt3.str(), __FILE__, __LINE__ );
        boost::format    fmt4( "sorry_count = %d" );
        fmt4 % sorry_count.get();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 53, fmt4.str(), __FILE__, __LINE__ );
    }

    session_thread_control* tmp_stc_ptr = waiting_sessions.find( stc_ptr_noconst->get_session().get() );
    waiting_sessions.erase( tmp_stc_ptr->get_session().get() );

    stc_ptr_noconst->startupstream();
    stc_ptr_noconst->startdownstream();

    //waiting, pool_sessions.size become over 1
    //pick up session from pool
    session_thread_control*        stc_ptr_register_accept;
    do{
        stc_ptr_register_accept = pool_sessions.pop();
    }
    while( unlikely( !stc_ptr_register_accept ) );

    //session add wait_sessions
    boost::mutex::scoped_lock    up_wait_lk( stc_ptr_register_accept->get_upthread_mutex() );
    boost::mutex::scoped_lock    down_wait_lk( stc_ptr_register_accept->get_downthread_mutex() );

    waiting_sessions.insert( stc_ptr_register_accept->get_session().get(), stc_ptr_register_accept );

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format    fmt1( "active session thread id = %d" );
        fmt1 % stc_ptr_register_accept->get_upthread_id();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 54, fmt1.str(), __FILE__, __LINE__ );
        boost::format    fmt2( "pool_session.size   = %d" );
        fmt2 % pool_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 55, fmt2.str(), __FILE__, __LINE__ );
        boost::format    fmt3( "active_session.size = %d" );
        fmt3 % active_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 56, fmt3.str(), __FILE__, __LINE__ );
        boost::format    fmt4( "sorry_count = %d" );
        fmt4 % sorry_count.get();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 57, fmt4.str(), __FILE__, __LINE__ );
    }

    //regist accept event handler
    if (!ssl_virtualservice_mode_flag) {
        acceptor_.async_accept( stc_ptr_register_accept->get_session()->get_client_socket(),
                    boost::bind( &virtualservice_tcp::handle_accept, this, stc_ptr_register_accept, boost::asio::placeholders::error ) );
    } else {
        acceptor_.async_accept( stc_ptr_register_accept->get_session()->get_client_ssl_socket().lowest_layer(),
                    boost::bind( &virtualservice_tcp::handle_accept, this, stc_ptr_register_accept, boost::asio::placeholders::error ) );
    }

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 58, "out_function : void virtualservice_tcp::handle_accept( const boost::shared_ptr<session_thread_control> , const boost::system::error_code& err )", __FILE__, __LINE__ );
    }
}

/*!
 * initialize virtualservice(TCP)
 *
 * @param   erro_code
 * @return  void
 */
void    l7vs::virtualservice_tcp::initialize( l7vs::error_code& err ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("in_function : void virtualservice_tcp::initialize( "
                                "l7vs::error_code& err )");
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 59, formatter.str(), __FILE__, __LINE__ );
    }
    if( pool_sessions.size() != 0 ){
        err.setter( true, "VirtualService already initialized." );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::initialize( "
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 60, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }

    //load parameter value
    load_parameter( err );
    if( err ){
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::initialize( "
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 61, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }

    //get cpumask
    boost::asio::ip::address    address    = element.tcp_accept_endpoint.address();
#ifdef    SCHED_SETAFFINITY
    vsnic_cpumask    = get_cpu_mask( address );
    rsnic_cpumask    = get_cpu_mask( param_data.nic_realserver_side );
#endif

    //bind acceptor
    boost::system::error_code    acceptor_err;
    acceptor_.open( element.tcp_accept_endpoint.protocol(), acceptor_err );
    if( acceptor_err ){
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 5, acceptor_err.message(), __FILE__, __LINE__ );
        err.setter( true, acceptor_err.message() );
        return;
    }
    acceptor_.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ), acceptor_err );
    if( acceptor_err ){
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 6, acceptor_err.message(), __FILE__, __LINE__ );
        err.setter( true, acceptor_err.message() );
        return;
    }
    acceptor_.bind( element.tcp_accept_endpoint, acceptor_err );
    if( acceptor_err ){
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 7, acceptor_err.message(), __FILE__, __LINE__ );
        err.setter( true, acceptor_err.message() );
        return;
    }

    boost::asio::ip::address address_check = element.tcp_accept_endpoint.address();
    if( likely(address_check.is_v6()) ) {
        boost::asio::ip::v6_only option(true);
        acceptor_.set_option(option);
    }

    //read replication data
    read_replicationdata();

    //load protocol module
    protomod = protocol_module_control::getInstance().load_module( element.protocol_module_name );
    if( NULL == protomod ){
        //ERROR case
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 8, PROTOMOD_LOAD_ERROR_MSG, __FILE__, __LINE__ );
        err.setter( true, PROTOMOD_LOAD_ERROR_MSG );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::initialize( "
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 62, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }
    //Protocol Module Initialize
    protomod->init_logger_functions(
                    boost::bind( &Logger::getLogLevel, LOG_CAT_PROTOCOL ),
                    boost::bind( &Logger::putLogFatal, LOG_CAT_PROTOCOL, _1, _2, _3, _4 ),
                    boost::bind( &Logger::putLogError, LOG_CAT_PROTOCOL, _1, _2, _3, _4 ),
                    boost::bind( &Logger::putLogWarn, LOG_CAT_PROTOCOL, _1, _2, _3, _4 ),
                    boost::bind( &Logger::putLogInfo, LOG_CAT_PROTOCOL, _1, _2, _3, _4 ),
                    boost::bind( &Logger::putLogDebug, LOG_CAT_PROTOCOL, _1, _2, _3, _4 ) );
    protomod->initialize(
                    boost::bind( &virtualservice_tcp::rs_list_begin, this ),
                    boost::bind( &virtualservice_tcp::rs_list_end, this ),
                    boost::bind( &virtualservice_tcp::rs_list_next, this, _1 ),
                    boost::bind( &virtualservice_tcp::rs_list_lock, this ),
                    boost::bind( &virtualservice_tcp::rs_list_unlock, this ) );
    protomod->init_replication_functions(
                    boost::bind( &virtualservice_tcp::replication_pay_memory, this, _1, _2 ),
                    boost::bind( &virtualservice_tcp::replication_area_lock, this, element.protocol_module_name ),
                    boost::bind( &virtualservice_tcp::replication_area_unlock, this, element.protocol_module_name ),
                    element.tcp_accept_endpoint,
                    element.udp_recv_endpoint );
    tcp_schedule_func_type    sched_rs_func    = boost::bind( &virtualservice_tcp::schedule_rs, this, _1, _2, _3, _4, _5 );
    protomod->register_schedule( sched_rs_func );

    protocol_module_base::check_message_result pm_result;
    
    pm_result    =    protomod->check_parameter( element.protocol_args );
    if( !pm_result.flag ){
        err.setter( true, "Protocol Module argument error." );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::initialize( "
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 63, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }
    pm_result    =    protomod->set_parameter( element.protocol_args );
    if( !pm_result.flag ){
        err.setter( true, "Protocol Module argument error." );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::initialize( "
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 64, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }

    protomod->get_option_info(protocol_module_for_indication_options);
    element.protocol_module_for_indication_options = protocol_module_for_indication_options;

    //load schedule module    
    schedmod = schedule_module_control::getInstance().load_module( element.schedule_module_name );
    if( NULL == schedmod ){
        //ERROR case
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 9, SCHEDMOD_LOAD_ERROR_MSG, __FILE__, __LINE__ );
        err.setter( true, SCHEDMOD_LOAD_ERROR_MSG );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::initialize( "
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 65, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }
    //Schedule Module Initialize
    schedmod->init_logger_functions(
                    boost::bind( &Logger::getLogLevel, LOG_CAT_SCHEDULE ),
                    boost::bind( &Logger::putLogFatal, LOG_CAT_SCHEDULE, _1, _2, _3, _4 ),
                    boost::bind( &Logger::putLogError, LOG_CAT_SCHEDULE, _1, _2, _3, _4 ),
                    boost::bind( &Logger::putLogWarn, LOG_CAT_SCHEDULE, _1, _2, _3, _4 ),
                    boost::bind( &Logger::putLogInfo, LOG_CAT_SCHEDULE, _1, _2, _3, _4 ),
                    boost::bind( &Logger::putLogDebug, LOG_CAT_SCHEDULE, _1, _2, _3, _4 ) );
    schedmod->initialize();
    schedmod->init_replication_functions(
                    boost::bind( &virtualservice_tcp::replication_pay_memory, this, _1, _2 ),
                    boost::bind( &virtualservice_tcp::replication_area_lock, this, element.schedule_module_name ),
                    boost::bind( &virtualservice_tcp::replication_area_unlock, this, element.schedule_module_name ),
                    element.tcp_accept_endpoint,
                    element.udp_recv_endpoint );

    // SSL setting
    ssl_virtualservice_mode_flag = false;
    ssl_file_name = element.ssl_file_name;
    if ( ssl_file_name == "" ) {
        ssl_file_name = element.ssl_file_name;
        // get SSL parameter
        if(unlikely(!get_ssl_parameter())) {
            //Error
            Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "get ssl parameter failed", __FILE__, __LINE__ );
            err.setter( true, "get ssl parameter failed" );
            return;
        }
        // set SSL configuration
        if(unlikely(!set_ssl_config())) {
            //Error
            Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "set ssl config failed", __FILE__, __LINE__ );
            err.setter( true, "set ssl config failed" );
            return;
        }
        ssl_virtualservice_mode_flag = true;
    }

//    /* @001 virtualservice_element 
    access_log_flag = false;
    if ( element.access_log_flag == 1 ) {
        access_log_flag = true;
    }
//    */

//    // @002 getInstance().find_LoggerAccess
//    /* @002
    if ( element.access_log_rotate_key_info == "none" ) {
       //element.access_log_rotate_verbose_info = getInstance().get_rotate_default_verbose_displayed_contents(); 
    }
    access_log_file_name = element.access_log_file_name;
    access_log_rotate_arguments = element.access_log_rotate_arguments;
        
    //logger_implement_access *logger_access_instance = new logger_implement_access(access_log_file_name);

    //if( logger_access_instance == NULL  ) {
        boost::format formatter("access logger Instance acquisition error:%s");
        formatter % ( err ? "true" : "false") % err.get_message();
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, formatter.str(), __FILE__, __LINE__ );
        err.setter( true, "access log class instance create failed" );
        return;
    //}

//    */

    set_socket_option();

    //create session pool
    {
        for( int i = 0; i < param_data.session_pool_size; ++i ){
            try{
                tcp_session*    sess    = new tcp_session(*this,
                                      dispatcher,
                                      set_sock_opt,
                                      element.tcp_accept_endpoint,
                                      ssl_virtualservice_mode_flag,
                                      sslcontext,
                                      is_session_cache_use,
                                      handshake_timeout,
                                      NULL);
                session_result_message    result    = sess->initialize();
                if( result.flag == true ){
                    err.setter( result.flag, result.message );
                    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
                        boost::format formatter("out_function : void virtualservice_tcp::initialize( "
                                                "l7vs::error_code& err ) : err = %s, err.message = %s");
                        formatter % ( err ? "true" : "false") % err.get_message();
                        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 66, formatter.str(), __FILE__, __LINE__ );
                    }
                    return;
                }
                session_thread_control*    p_stc = new session_thread_control( sess, vsnic_cpumask, rsnic_cpumask, -1 );
                while( !pool_sessions.push( p_stc ) ){}
            }
            catch( std::bad_alloc ex ){
                Logger::putLogFatal( 
                    LOG_CAT_L7VSD_VIRTUALSERVICE, 1, "error, create session.", __FILE__, __LINE__ );
                if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
                    boost::format formatter("out_function : void virtualservice_tcp::initialize( "
                                            "l7vs::error_code& err ) : err = %s, err.message = %s");
                    formatter % ( err ? "true" : "false") % err.get_message();
                    Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 67, formatter.str(), __FILE__, __LINE__ );
                }
                return;
            }
        }
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format    fmt1( "pool_session.size   = %d" );
            fmt1 % pool_sessions.size();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 68, fmt1.str(), __FILE__, __LINE__ );
            boost::format    fmt2( "active_session.size = %d" );
            fmt2 % active_sessions.size();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 69, fmt2.str(), __FILE__, __LINE__ );
        }
    }

    err.setter( false, "" );

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("out_function : void virtualservice_tcp::initialize( "
                                "l7vs::error_code& err ) : err = %s, err.message = %s");
        formatter % ( err ? "true" : "false") % err.get_message();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 70, formatter.str(), __FILE__, __LINE__ );
    }
}

/*!
 * finalize virtualservice(TCP)
 *
 * @param   erro_code
 * @return  void
 */
void        l7vs::virtualservice_tcp::finalize( l7vs::error_code& err ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("in_function : void virtualservice_tcp::finalize( "
                                "l7vs::error_code& err )");
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 71, formatter.str(), __FILE__, __LINE__ );

        boost::format    fmt1( "pool_session.size   = %d" );
        fmt1 % pool_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 72, fmt1.str(), __FILE__, __LINE__ );
        boost::format    fmt2( "active_session.size = %d" );
        fmt2 % active_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 73, fmt2.str(), __FILE__, __LINE__ );
    }

    //stop main loop
    stop();

    while(active_sessions.size()){
        boost::this_thread::yield();
    }

    for(;;){
        tcp_session*                tmp_session    = NULL;
        session_thread_control*        tmp_stc        = NULL;
        waiting_sessions.pop( tmp_session, tmp_stc );
        if( !tmp_stc ){
            break;
        }
        for(;;){
            if( likely( pool_sessions.push( tmp_stc ) ) )break;
        }
    }

    //release sessions[i]->join();
    while( !pool_sessions.empty() ){
        session_thread_control*    stc = pool_sessions.pop();
        stc->join();
        {
            boost::mutex::scoped_lock upthread_wait( stc->get_upthread_mutex() );
            boost::mutex::scoped_lock downthread_wait( stc->get_downthread_mutex() );
        }
        delete    stc;
        stc = NULL;
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format    fmt( "join pool sessin : pool_sessions.size = %d" );
            fmt % pool_sessions.size();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 74, fmt.str(), __FILE__, __LINE__ );
        }
    }

    //unload ProtocolModule
    if( protomod ){
        //finalize ProtocolModule
        protomod->finalize();
        //unload ProtocolModule
        protocol_module_control::getInstance().unload_module( protomod );
        protomod = NULL;
    }
    //unload ScheduleModule
    if( schedmod ){
        schedule_module_control::getInstance().unload_module( schedmod );
        schedmod = NULL;
    }

    active_sessions.clear();
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format    fmt1( "pool_session.size   = %d" );
        fmt1 % pool_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 75, fmt1.str(), __FILE__, __LINE__ );
        boost::format    fmt2( "active_session.size = %d" );
        fmt2 % active_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 76, fmt2.str(), __FILE__, __LINE__ );
    }

    //clear replication_data
    bool    replication_status    = true;
    replication&    rep_noconst = const_cast<replication&>( rep );

    if( replication::REPLICATION_SINGLE != rep_noconst.get_status() ){
        unsigned int    rep_size = 0;
        replication_header*    rep_header_ptr = reinterpret_cast<replication_header*>( rep_noconst.pay_memory( REP_AREA_NAME, rep_size) );
        if( (rep_header_ptr == NULL) || (0 == rep_size) ){
            err.setter( true, REP_BLOCK_SIZE_ERR_MSG );
            Logger::putLogWarn( LOG_CAT_L7VSD_VIRTUALSERVICE, 6, REP_BLOCK_SIZE_ERR_MSG, __FILE__, __LINE__ );
            replication_status = false;
        }
    
        //check maxdatasize
        if( ( rep_size * DATA_SIZE ) <
            ((sizeof(replication_data) * MAX_REPLICATION_DATA_NUM) + sizeof(replication_header)) ){
            err.setter( true, REP_AREA_SIZE_ERR_MSG );
            Logger::putLogWarn( LOG_CAT_L7VSD_VIRTUALSERVICE, 7, REP_AREA_SIZE_ERR_MSG, __FILE__, __LINE__ );
            replication_status = false;
        }
        if( (NULL != rep_header_ptr) && (replication_status) ){
            //lock replication area
            rep_noconst.lock( REP_AREA_NAME );
            //set data_num = 0
            rep_header_ptr->data_num = 0;
            //unlock replication area
            rep_noconst.unlock( REP_AREA_NAME );
        }
    }

    vsd.release_virtual_service( element );

    // @002 getInstance().erase_LoggerAccess

    err.setter( false, "" );

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("out_function : void virtualservice_tcp::finalize( "
                                "l7vs::error_code& err ) : err = %s, err.message = %s");
        formatter % ( err ? "true" : "false") % err.get_message();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 77, formatter.str(), __FILE__, __LINE__ );
    }
}

/*!
 * equal compare virtualservice(TCP)
 *
 * @param   virtualservice_base
 * @return  bool ( true = equal )
 */
bool    l7vs::virtualservice_tcp::operator==( const l7vs::virtualservice_base& in ){
    boost::format formatter("function : void virtualservice_tcp::operator==( "
                            "const l7vs::virtualservice_base& in )");
    Logger    funcLog( LOG_CAT_L7VSD_VIRTUALSERVICE, 78, formatter.str(), __FILE__, __LINE__ );

    virtualservice_base&    vs = const_cast<virtualservice_base&>( in );
    return (    ( element.tcp_accept_endpoint == vs.get_element().tcp_accept_endpoint )
            &&    ( element.udpmode == vs.get_element().udpmode ) );
}

/*!
 * not-equal compare virtualservice(TCP)
 *
 * @param   virtualservice_base
 * @return  bool ( true = not-equal )
 */
bool    l7vs::virtualservice_tcp::operator!=( const l7vs::virtualservice_base& in ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("in/out_function : void virtualservice_tcp::operator!=( "
                                "const l7vs::virtualservice_base& in )");
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 79, formatter.str(), __FILE__, __LINE__ );
    }
    virtualservice_base&    vs = const_cast<virtualservice_base&>( in );
    return (    ( element.tcp_accept_endpoint != vs.get_element().tcp_accept_endpoint )
            ||    ( element.udpmode != vs.get_element().udpmode ) );
}

/*!
 * add VirtualService( not-imprement )
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void    l7vs::virtualservice_tcp::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    err.setter( false, "" );
}

/*!
 * edit VirtualService
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void    l7vs::virtualservice_tcp::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("in_function : void virtualservice_tcp::edit_virtualservice( "
                                "const l7vs::virtualservice_element& in,"
                                "l7vs::error_code& err ) : dump in ");
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 80, formatter.str(), __FILE__, __LINE__ );
    }

    virtualservice_element&    elem = const_cast<virtualservice_element&>( in );
    //繝代Λ繝｡繝ｼ繧ｿ縺祁irtualService縺ｫ荳閾ｴ縺吶ｋ縺区､懈渊
    //udpmode縺ｨtcp_accept_endpoint縺ｨprotocol_module_name縺御ｸ閾ｴ縺吶ｋ縺薙→
    if( ( element.udpmode != elem.udpmode ) ||
        ( element.tcp_accept_endpoint != elem.tcp_accept_endpoint ) ||
        ( element.protocol_module_name != elem.protocol_module_name ) ){
        err.setter( true, "Virtual Service does not exist." );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::edit_virtualservice( "
                                    "const l7vs::virtualservice_element& in,"
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 81, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }

    //if change ScueduleModule Name, unload old ScheduleModule and load new ScheduleModule
    if( ( element.schedule_module_name != elem.schedule_module_name ) && ( "" != elem.schedule_module_name ) ){
        schedule_module_control::getInstance().unload_module( schedmod );
        schedmod = schedule_module_control::getInstance().load_module( elem.schedule_module_name );
        if( NULL == schedmod ){
            //ERROR case
            Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 10, SCHEDMOD_LOAD_ERROR_MSG, __FILE__, __LINE__ );
            err.setter( true, SCHEDMOD_LOAD_ERROR_MSG );
            if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
                boost::format formatter("out_function : void virtualservice_tcp::edit_virtualservice( "
                                        "const l7vs::virtualservice_element& in,"
                                        "l7vs::error_code& err ) : err = %s, err.message = %s");
                formatter % ( err ? "true" : "false") % err.get_message();
                Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 82, formatter.str(), __FILE__, __LINE__ );
            }
            return;
        }
        element.schedule_module_name = elem.schedule_module_name;
    }
/*
    //additional PM options(for protomod_url)
    protocol_module_base::check_message_result result;
    if( NULL != protomod ){
        result = protomod->add_parameter( elem.protocol_args );
        if( result.flag ){
            for( size_t i = 0; i < elem.protocol_args.size(); ++i ){
                element.protocol_args.push_back( elem.protocol_args[i] );
            }
        }else{
            err.setter( true, result.message );
            if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
                boost::format formatter("out_function : void virtualservice_tcp::edit_virtualservice( "
                                        "const l7vs::virtualservice_element& in,"
                                        "l7vs::error_code& err ) : err = %s, err.message = %s");
                formatter % ( err ? "true" : "false") % err.get_message();
                Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 83, formatter.str(), __FILE__, __LINE__ );
            }
            return;
        }
    }else{
        //ERROR case
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 11, PROTOMOD_LOAD_ERROR_MSG, __FILE__, __LINE__ );
        err.setter( true, PROTOMOD_LOAD_ERROR_MSG );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::edit_virtualservice( "
                                    "const l7vs::virtualservice_element& in,"
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 84, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }
*/
    //update values
    //qos_upstream;
    if( ULLONG_MAX == elem.qos_upstream )
        element.qos_upstream    = 0ULL;
    else if( 0ULL != elem.qos_upstream )
        element.qos_upstream    = elem.qos_upstream;
    //qos_downstream;
    if( ULLONG_MAX == elem.qos_downstream )
        element.qos_downstream    = 0ULL;
    else if( 0ULL != elem.qos_downstream )
        element.qos_downstream    = elem.qos_downstream;
    //sorry_maxconnection;
    if( LLONG_MAX == elem.sorry_maxconnection )
        element.sorry_maxconnection    = 0;
    else if( 0 != elem.sorry_maxconnection )
        element.sorry_maxconnection    = elem.sorry_maxconnection;
    //sorry_flag;
    if( INT_MAX == elem.sorry_flag )
        element.sorry_flag            = 0;
    else if( 0 != elem.sorry_flag )
        element.sorry_flag            = 1;
    //sorry_endpoint;
    //if endpoint of SorryServer equal 255.255.255.255:0,not update
    //if equal endpoint 255.255.255.255:0, clear sorry parameters
    if( elem.sorry_endpoint ==
            boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "255.255.255.255" ), (0)) ){
        element.sorry_endpoint            = boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "0.0.0.0" ), (0) );
        element.sorry_maxconnection    = 0LL;
        element.sorry_flag            = false;
        active_sessions.do_all( boost::bind( &session_thread_control::session_sorry_disable, _1 ) );
    }else{
        if( elem.sorry_endpoint !=
                boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "0.0.0.0" ), (0) ) )
            element.sorry_endpoint        = elem.sorry_endpoint;

        active_sessions.do_all( boost::bind( &session_thread_control::session_sorry_mode_change, _1, elem.sorry_flag ) );
    }

//    /* @001 virtualservice_element
    element.access_log_flag = elem.access_log_flag;
    if (elem.access_log_flag==1 || access_log_flag==false ) {
        active_sessions.do_all( boost::bind( &session_thread_control::session_accesslog_output_mode_on, _1 ) );
        access_log_flag = true;
    } else if ( elem.access_log_flag==0 || access_log_flag==true ) {
        active_sessions.do_all( boost::bind( &session_thread_control::session_accesslog_output_mode_off, _1 ) );
        access_log_flag = false;
    }
//    */

    err.setter( false, "" );

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("out_function : void virtualservice_tcp::set_virtualservice( "
                                "const l7vs::virtualservice_element& in,"
                                "l7vs::error_code& err ) : err = %s, err.message = %s");
        formatter % ( err ? "true" : "false") % err.get_message();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 85, formatter.str(), __FILE__, __LINE__ );
    }
}

/*!
 * add realserver
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void    l7vs::virtualservice_tcp::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("in_function : void virtualservice_tcp::add_realserver( "
                                "const l7vs::virtualservice_element& in,"
                                "l7vs::error_code& err )");
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 86, formatter.str(), __FILE__, __LINE__ );
    }

    virtualservice_element&    in_element = const_cast<virtualservice_element&>( in );

    //check equal virtualservice
    if( (element.udpmode != in.udpmode) || (element.tcp_accept_endpoint != in.tcp_accept_endpoint) ){
        err.setter( true, "Virtual Service does not exist." );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::add_realserver( "
                                    "const l7vs::virtualservice_element& in,"
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 87, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }    
    //check rs num
    if( in.realserver_vector.size() == 0U ){
        err.setter( true, "." );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::add_realserver( "
                                    "const l7vs::virtualservice_element& in,"
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 88, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }
    //lock rs_list_ref_count_inc_mutex
    rw_scoped_lock    inc_lock( rs_list_ref_count_inc_mutex );

    //waiting, rs_list_ref_count become 0
    boost::xtime        interval;
    boost::mutex        mtx;
    boost::condition    cond;

    interval.nsec = virtualservice_base::REFCOUNT_WAIT_INTERVAL;
    boost::mutex::scoped_lock lk( mtx );
    while( rs_list_ref_count != 0ULL ){
        cond.timed_wait( lk, interval );
    }

    //check duplication realserver
    for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
         itr != in_element.realserver_vector.end();
         ++itr ){
        for( std::list<realserver>::iterator rs_itr = rs_list.begin();
             rs_itr != rs_list.end(); ++rs_itr ){
            if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
                err.setter( true, "Real Server already exist." );
                if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
                    boost::format formatter("out_function : void virtualservice_tcp::add_realserver( "
                                            "const l7vs::virtualservice_element& in,"
                                            "l7vs::error_code& err ) : err = %s, err.message = %s");
                    formatter % ( err ? "true" : "false") % err.get_message();
                    Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 89, formatter.str(), __FILE__, __LINE__ );
                }
                return;
            }
        }
    }

    //pause active sessions
    active_sessions.do_all( boost::bind( &session_thread_control::session_pause_on, _1) );

    //add realserver
    for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
         itr != in_element.realserver_vector.end();
         ++itr ){
        realserver    rs;
        rs.tcp_endpoint        = itr->tcp_endpoint;
        rs.udp_endpoint        = itr->udp_endpoint;
        rs.weight            = itr->weight;
        rs_list.push_back( rs );
    }

    //run active sessions
    active_sessions.do_all( boost::bind( &session_thread_control::session_pause_off, _1) );

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("out_function : void virtualservice_tcp::add_realserver( "
                                "const l7vs::virtualservice_element& in,"
                                "l7vs::error_code& err ) : err = %s, err.message = %s");
        formatter % ( err ? "true" : "false") % err.get_message();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 90, formatter.str(), __FILE__, __LINE__ );
    }
}

/*!
 * edit realserver
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void    l7vs::virtualservice_tcp::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("in_function : void virtualservice_tcp::edit_realserver( "
                                "const l7vs::virtualservice_element& in,"
                                "l7vs::error_code& err )");
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 91, formatter.str(), __FILE__, __LINE__ );
    }
    //check equal virtualservice
    if( (element.udpmode != in.udpmode) || (element.tcp_accept_endpoint != in.tcp_accept_endpoint) ){
        err.setter( true, "Virtual Service does not exist." );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::edit_realserver( "
                                    "const l7vs::virtualservice_element& in,"
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 92, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }    

    //lock rs_list_ref_count_inc_mutex
    rw_scoped_lock    inc_lock( rs_list_ref_count_inc_mutex );

    //waiting, rs_list_ref_count become 0
    boost::xtime        interval;
    boost::mutex        mtx;
    boost::condition    cond;

    interval.nsec = virtualservice_base::REFCOUNT_WAIT_INTERVAL;
    boost::mutex::scoped_lock lk( mtx );
    while( rs_list_ref_count != 0ULL ){
        cond.timed_wait( lk, interval );
    }

    virtualservice_element&    in_element = const_cast<virtualservice_element&>( in );

    //check is exist realserver
    for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
         itr != in_element.realserver_vector.end();
         ++itr ){
        bool    exist_flag = false;
        for( std::list<realserver>::iterator rs_itr = rs_list.begin();
             rs_itr != rs_list.end(); ++rs_itr ){
            if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
                exist_flag = true;
            }
        }
        if( !exist_flag ){
            err.setter( true, "Real Server does not exist." );
            if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
                boost::format formatter("out_function : void virtualservice_tcp::edit_realserver( "
                                        "const l7vs::virtualservice_element& in,"
                                        "l7vs::error_code& err ) : err = %s, err.message = %s");
                formatter % ( err ? "true" : "false") % err.get_message();
                Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 93, formatter.str(), __FILE__, __LINE__ );
            }
            return;
        }
    }

    //pause active sessions
    active_sessions.do_all( boost::bind( &session_thread_control::session_pause_on, _1) );

    //edit realserver
    for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
         itr != in_element.realserver_vector.end();
         ++itr ){
        for( std::list<realserver>::iterator rs_itr = rs_list.begin();
             rs_itr != rs_list.end(); ++rs_itr ){
            if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
                rs_itr->udp_endpoint    = itr->udp_endpoint;
                rs_itr->weight            = itr->weight;
            }
        }
    }

    //run active sessions
    active_sessions.do_all( boost::bind( &session_thread_control::session_pause_off, _1) );

    err.setter( false, "" );
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("out_function : void virtualservice_tcp::edit_realserver( "
                                "const l7vs::virtualservice_element& in,"
                                "l7vs::error_code& err ) : err = %s, err.message = %s");
        formatter % ( err ? "true" : "false") % err.get_message();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 94, formatter.str(), __FILE__, __LINE__ );
    }
}

/*!
 * delete realserver
 *
 * @param   virtualservice_element
 * @param   err
 * @return  void
 */
void    l7vs::virtualservice_tcp::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("in_function : void virtualservice_tcp::del_realserver( "
                                "const l7vs::virtualservice_element& in,"
                                "l7vs::error_code& err )");
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 95, formatter.str(), __FILE__, __LINE__ );
    }

    //check equal virtualservice
    if( (element.udpmode != in.udpmode) || (element.tcp_accept_endpoint != in.tcp_accept_endpoint) ){
        err.setter( true, "Virtual Service is not equal." );
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format formatter("out_function : void virtualservice_tcp::del_realserver( "
                                    "const l7vs::virtualservice_element& in,"
                                    "l7vs::error_code& err ) : err = %s, err.message = %s");
            formatter % ( err ? "true" : "false") % err.get_message();
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 96, formatter.str(), __FILE__, __LINE__ );
        }
        return;
    }    

    //lock rs_list_ref_count_inc_mutex
    rw_scoped_lock    inc_lock( rs_list_ref_count_inc_mutex );

    //waiting, rs_list_ref_count become 0
    boost::xtime        interval;
    boost::mutex        mtx;
    boost::condition    cond;

    interval.nsec = virtualservice_base::REFCOUNT_WAIT_INTERVAL;
    boost::mutex::scoped_lock lk( mtx );
    while( rs_list_ref_count != 0ULL ){
        cond.timed_wait( lk, interval );
    }

    virtualservice_element&    in_element = const_cast<virtualservice_element&>( in );

    //check is exist realserver
    for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
         itr != in_element.realserver_vector.end();
         ++itr ){
        bool    exist_flag = false;
        for( std::list<realserver>::iterator rs_itr = rs_list.begin();
             rs_itr != rs_list.end(); ++rs_itr ){
            if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
                exist_flag = true;
            }
        }
        if( !exist_flag ){
            err.setter( true, "Real Server does not exist." );
            if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
                boost::format formatter("out_function : void virtualservice_tcp::del_realserver( "
                                        "const l7vs::virtualservice_element& in,"
                                        "l7vs::error_code& err ) : err = %s, err.message = %s");
                formatter % ( err ? "true" : "false") % err.get_message();
                Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 97, formatter.str(), __FILE__, __LINE__ );
            }
            return;
        }
    }

    //pause active sessions
    active_sessions.do_all( boost::bind( &session_thread_control::session_pause_on, _1) );

    //del realserver
    for( std::vector<realserver_element>::iterator itr = in_element.realserver_vector.begin();
         itr != in_element.realserver_vector.end();
         ++itr ){
        for( std::list<realserver>::iterator rs_itr = rs_list.begin();
             rs_itr != rs_list.end(); ++rs_itr ){
            if( itr->tcp_endpoint == rs_itr->tcp_endpoint ){
                rs_list.erase( rs_itr );
                break;
            }
        }
    }

    //run active sessions
    active_sessions.do_all( boost::bind( &session_thread_control::session_pause_off, _1) );

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("out_function : void virtualservice_tcp::del_realserver( "
                                "const l7vs::virtualservice_element& in,"
                                "l7vs::error_code& err ) : err = %s, err.message = %s");
        formatter % ( err ? "true" : "false") % err.get_message();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 98, formatter.str(), __FILE__, __LINE__ );
    }
}

/*!
 * main loop of virtualservice(TCP)
 *
 * @param   void
 * @return  void
 */
void    l7vs::virtualservice_tcp::run(){
    Logger    funcLog( LOG_CAT_L7VSD_VIRTUALSERVICE, 99, "function : void virtualservice_tcp::run()", __FILE__, __LINE__ );

    if( pool_sessions.size() == 0 ){
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 12, "VirtualService not initialized.", __FILE__, __LINE__ );
        return;
    }
    boost::asio::socket_base::receive_buffer_size option(8192 * 192);
    acceptor_.set_option(option);
    //set socket option TCP_DEFER_ACCEPT
    if(defer_accept_opt){
        boost::system::error_code ec;
        size_t len = sizeof(defer_accept_val);
        boost::asio::detail::socket_ops::setsockopt(acceptor_.native(),IPPROTO_TCP,TCP_DEFER_ACCEPT,&defer_accept_val,len,ec);
        if(unlikely(ec)){
            //ERROR
            Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 100, "socket option(TCP_DEFER_ACCEPT) set failed" , __FILE__, __LINE__ );
        }
    }
    //start listen
    acceptor_.listen();

    //switch active a session
    session_thread_control*    stc_ptr;
    {
        stc_ptr = pool_sessions.pop();
        if( !stc_ptr ){
            return;
        }
        //regist accept event handler
        waiting_sessions.insert( stc_ptr->get_session().get(), stc_ptr );
    }
    if (!ssl_virtualservice_mode_flag) {
        acceptor_.async_accept( stc_ptr->get_session()->get_client_socket(),
                        boost::bind( &virtualservice_tcp::handle_accept, this, stc_ptr, boost::asio::placeholders::error ) );
    } else {
        acceptor_.async_accept( stc_ptr->get_session()->get_client_ssl_socket().lowest_layer(),
                        boost::bind( &virtualservice_tcp::handle_accept, this, stc_ptr, boost::asio::placeholders::error ) );
    }
    //regist timer event handler
    calc_bps_timer->expires_from_now( boost::posix_time::milliseconds( param_data.bps_interval ) );
    calc_bps_timer->async_wait( boost::bind( &virtualservice_tcp::handle_throughput_update, 
                                             this, boost::asio::placeholders::error ) );

    //register handle_replication_interrupt
    replication_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
    replication_timer->async_wait( boost::bind( &virtualservice_tcp::handle_replication_interrupt, 
                                             this, boost::asio::placeholders::error ) );
    //register handle_protomod_replication
    protomod_rep_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
    protomod_rep_timer->async_wait( boost::bind( &virtualservice_tcp::handle_protomod_replication, 
                                             this, boost::asio::placeholders::error ) );
    //register handle_schedmod_replication
    schedmod_rep_timer->expires_from_now( boost::posix_time::milliseconds( param_data.rep_interval ) );
    schedmod_rep_timer->async_wait( boost::bind( &virtualservice_tcp::handle_schedmod_replication, 
                                             this, boost::asio::placeholders::error ) );

    //run dispatcher(start io_service loop)
    dispatcher.run();

    //stop all active sessions
    {
        active_sessions.do_all( boost::bind( &session_thread_control::session_stop, _1 ) );
    }
}

/*!
 * stop loop
 *
 * @param   void
 * @return  void
 */
void    l7vs::virtualservice_tcp::stop(){
    Logger    funcLog( LOG_CAT_L7VSD_VIRTUALSERVICE, 100, "function : void virtualservice_tcp::stop()", __FILE__, __LINE__ );

    boost::system::error_code    err;

    acceptor_.close( err );
    if( err ){
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 14, err.message(), __FILE__, __LINE__ );
    }

    //stop dispatcher
    dispatcher.stop();
}

/*!
 * increment active-connection count
 *
 * @param   endpoint
 * @return  void
 */
void    l7vs::virtualservice_tcp::connection_active( const boost::asio::ip::tcp::endpoint& in ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("in_function : void virtualservice_tcp::connection_active( "
                                "const boost::asio::ip::tcp::endpoint& in ) : in = %s");
        formatter % in;
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 101, formatter.str(), __FILE__, __LINE__ );
    }
    rs_list_lock();
    for( std::list<realserver>::iterator itr = rs_list.begin();
         itr != rs_list.end();
         ++itr ){
        if( itr->tcp_endpoint == in ){
            itr->increment_active();
            break;
        }
    }
    rs_list_unlock();

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 102, "out_function : void virtualservice_tcp::connection_active()", __FILE__, __LINE__ );
    }
}

/*!
 * increment in-active-connection (and decrement active-connection count)
 *
 * @param   endpoint
 * @return  void
 */
void    l7vs::virtualservice_tcp::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("in_function : void virtualservice_tcp::connection_inactive( "
                                "const boost::asio::ip::tcp::endpoint& in ) : in = %s");
        formatter % in;
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 103, formatter.str(), __FILE__, __LINE__ );
    }
    rs_list_lock();
    for( std::list<realserver>::iterator itr = rs_list.begin();
         itr != rs_list.end();
         ++itr ){
        if( itr->tcp_endpoint == in ){
            itr->decrement_active();
            itr->increment_inact();
            break;
        }
    }
    rs_list_unlock();

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 104, "out_function : void virtualservice_tcp::connection_inactive()", __FILE__, __LINE__ );
    }
}

/*!
 * release_session
 *
 * @param   session_ptr
 * @return  void
 */
void    l7vs::virtualservice_tcp::release_session( const tcp_session* session_ptr ){
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format funclog_fmt("in_function : void virtualservice_tcp::release_session( "
                                "const tcp_session* session_ptr ) : session_ptr = %d");
        funclog_fmt % session_ptr;
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 105, funclog_fmt.str(), __FILE__, __LINE__ );
    }

    session_thread_control*        stc_ptr = active_sessions.find( session_ptr );
    if( unlikely( NULL == stc_ptr ) ){
        if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
            boost::format    fmt( "session release fail : active_sessions.find( const tcp_session* session_ptr = %d )");
            fmt % session_ptr;
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 106, fmt.str(), __FILE__, __LINE__ );
            Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 107, "out_function : void virtualservice_tcp::release_session( const boost::thread::id thread_id )", __FILE__, __LINE__ );
        }
        return;
    }

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format    fmt1( "pool_session.size   = %d" );
        fmt1 % pool_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 108, fmt1.str(), __FILE__, __LINE__ );
        boost::format    fmt2( "active_session.size = %d" );
        fmt2 % active_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 109, fmt2.str(), __FILE__, __LINE__ );
        boost::format    fmt3( "sorry_count = %d" );
        fmt3 % sorry_count.get();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 110, fmt3.str(), __FILE__, __LINE__ );
    }
    active_sessions.erase( session_ptr );
    stc_ptr->get_session()->initialize();
    for(;;){
        if( likely( pool_sessions.push( stc_ptr ) ) ){
            break;
        }
    }

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format    fmt1( "pool_session.size   = %d" );
        fmt1 % pool_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 111, fmt1.str(), __FILE__, __LINE__ );
        boost::format    fmt2( "active_session.size = %d" );
        fmt2 % active_sessions.size();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 112, fmt2.str(), __FILE__, __LINE__ );
        boost::format    fmt3( "sorry_count = %d" );
        fmt3 % sorry_count.get();
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 113, fmt3.str(), __FILE__, __LINE__ );

        boost::format funclog_fmt("out_function : void virtualservice_tcp::release_session( "
                                "const tcp_session* session_ptr ) : session_ptr = %d");
        funclog_fmt % session_ptr;
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 114, funclog_fmt.str(), __FILE__, __LINE__ );
    }
}

/*!
 * set_socket_option
 *
 * @param   module option
 * @return  void
 */
void l7vs::virtualservice_tcp::set_socket_option(){

    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format funclog_fmt("in_function : void virtualservice_tcp::et_socket_option");
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, funclog_fmt.str(), __FILE__, __LINE__ );
    }
 
    // socket option check & set
    //! is set option TCP_DEFER_ACCEPT
    defer_accept_opt = false;
        //! TCP_DEFER_ACCEPT option value
    defer_accept_val = 0;
        
        //! TCP_NODELAY   (false:not set,true:set option)
    set_sock_opt.nodelay_opt = false;
        //! TCP_NODELAY option value  (false:off,true:on)
    set_sock_opt.nodelay_val = false;
        //! TCP_CORK      (false:not set,true:set option)
    set_sock_opt.cork_opt = false;
        //! TCP_CORK option value     (false:off,true:on)
    set_sock_opt.cork_val = false;
        //! TCP_QUICKACK  (false:not set,true:set option)
    set_sock_opt.quickack_opt = false;
        //! TCP_QUICKACK option value (false:off,true:on)
    set_sock_opt.quickack_val = false;

    // set socket option
    if ( element.socket_option_tcp_defer_accept != 0 ) {
        defer_accept_opt = true;
        if ( element.socket_option_tcp_defer_accept == 1 ) {
            defer_accept_val = 1;
        }
    }

    if ( element.socket_option_tcp_nodelay != 0 ) {
        set_sock_opt.nodelay_opt = true;
        if ( element.socket_option_tcp_nodelay == 1 ) {
            set_sock_opt.nodelay_val = true;
        }
    }

    if ( element.socket_option_tcp_cork != 0 ) {
        set_sock_opt.cork_opt = true;
        if ( element.socket_option_tcp_cork == 1 ) {
            set_sock_opt.cork_val = true;
        }
    }

    if ( element.socket_option_tcp_quickack != 0  ) {
        
        set_sock_opt.quickack_opt = true;
        if ( element.socket_option_tcp_quickack == 1  ) {
            set_sock_opt.quickack_val = true;
        }
    }

    //----Debug log----------------------------------------------------------------------
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( LOG_CAT_L7VSD_VIRTUALSERVICE ) ) ){
        boost::format formatter("set_socket_option"
                    " defer_accept_opt[%s] defer_accept_val[%d]"
                    " nodelay_opt[%s] nodelay_val[%s]"
                    " cork_opt[%s] cork_val[%s]"
                    " quickack_opt[%s]" "quickack_val[%s]");
        formatter %(defer_accept_opt ? "true" : "false") %defer_accept_val 
                        %(set_sock_opt.nodelay_opt ? "true" : "false") %(set_sock_opt.nodelay_val ? "true" : "false") 
                        %(set_sock_opt.cork_opt ? "true" : "false") %(set_sock_opt.cork_val ? "true" : "false") 
                        %(set_sock_opt.quickack_opt ? "true" : "false") %(set_sock_opt.quickack_val ? "true" : "false");
        Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, formatter.str(), __FILE__, __LINE__ );
    }
    //----Debug log----------------------------------------------------------------------

}

/*!
 * get private key file password (for callback function)
 *
 * @return password string
 */
std::string    l7vs::virtualservice_tcp::get_ssl_password()
{
    // Get password from file.
    std::string retstr = "";
    FILE  *fp;
    char buf[MAX_SSL_PASSWD_SIZE + 3];
    if ((fp = fopen((private_key_passwd_dir + private_key_passwd_file).c_str(), "r")) == NULL) {
        Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "Password file cannot open.", __FILE__, __LINE__ );
    } else {
        if (fgets(buf, MAX_SSL_PASSWD_SIZE + 3, fp) == NULL) {
            Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "Password not found in file.", __FILE__, __LINE__ );
        } else {
            if (strlen(buf) > MAX_SSL_PASSWD_SIZE) {
                Logger::putLogError( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "Password is too long.", __FILE__, __LINE__ );
            } else {
                buf[strlen(buf) - 1] = '\0';
                retstr = buf;
            }
        }
        fclose(fp);
    }
    return retstr;
}

/*!
 * Convert verify option string to intger(#define).
 *
 * @param[in]    opt_string    option string
 * @retval    ret    option value
 * @retval    -1    no match
 */
int l7vs::virtualservice_tcp::conv_verify_option(std::string opt_string)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
        std::stringstream buf;
        buf << "in_function : int virtualservice_tcp::conv_verify_option(std::string opt_string) : ";
        buf << "opt_string = " << opt_string;
        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                    buf.str(),
                    __FILE__, __LINE__ );
    }
    /*------ DEBUG LOG END ------*/

    int ret = -1;
    /*!
     * /usr/include/openssl/ssl.h
     * #define SSL_VERIFY_NONE                 0x00
     * #define SSL_VERIFY_PEER                 0x01
     * #define SSL_VERIFY_FAIL_IF_NO_PEER_CERT 0x02
     * #define SSL_VERIFY_CLIENT_ONCE          0x04
     */
    if (opt_string == "SSL_VERIFY_NONE") {
        ret = SSL_VERIFY_NONE;
    } else if (opt_string == "SSL_VERIFY_PEER") {
        ret = SSL_VERIFY_PEER;
    } else if (opt_string == "SSL_VERIFY_FAIL_IF_NO_PEER_CERT") {
        ret = SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
    } else if (opt_string == "SSL_VERIFY_CLIENT_ONCE") {
        ret = SSL_VERIFY_CLIENT_ONCE;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
        std::stringstream buf;
        buf << "out_function : int virtualservice_tcp::conv_verify_option(std::string opt_string) : ";
        buf << "return_value = " << ret;
        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                    buf.str(),
                    __FILE__, __LINE__ );
    }
    /*------ DEBUG LOG END ------*/
    // if ret == -1 then No match.
    return ret;
}

/*!
 * Convert SSL option string to intger(#define).
 *
 * @param[in]    opt_string    option string
 * @retval    ret    option value
 * @retval    -1    no match
 */
long int l7vs::virtualservice_tcp::conv_ssl_option(std::string opt_string)
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
        std::stringstream buf;
        buf << "in_function : long int virtualservice_tcp::conv_ssl_option(std::string opt_string) : ";
        buf << "opt_string = " << opt_string;
        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                    buf.str(),
                    __FILE__, __LINE__ );
    }
    /*------ DEBUG LOG END ------*/

    long int ret = -1;
    /*!
     * /usr/include/openssl/ssl.h
     * #define SSL_OP_MICROSOFT_SESS_ID_BUG                    0x00000001L
     * #define SSL_OP_NETSCAPE_CHALLENGE_BUG                   0x00000002L
     * #define SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG         0x00000008L
     * #define SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG              0x00000010L
     * #define SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER               0x00000020L
     * #define SSL_OP_MSIE_SSLV2_RSA_PADDING                   0x00000040L
     * #define SSL_OP_SSLEAY_080_CLIENT_DH_BUG                 0x00000080L
     * #define SSL_OP_TLS_D5_BUG                               0x00000100L
     * #define SSL_OP_TLS_BLOCK_PADDING_BUG                    0x00000200L
     * #define SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS              0x00000800L
     * #define SSL_OP_ALL                                      0x00000FF7L
     * #define SSL_OP_NO_QUERY_MTU                             0x00001000L
     * #define SSL_OP_COOKIE_EXCHANGE                          0x00002000L
     * #define SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION   0x00010000L
     * #define SSL_OP_SINGLE_ECDH_USE                          0x00080000L
     * #define SSL_OP_SINGLE_DH_USE                            0x00100000L
     * #define SSL_OP_EPHEMERAL_RSA                            0x00200000L
     * #define SSL_OP_CIPHER_SERVER_PREFERENCE                 0x00400000L
     * #define SSL_OP_TLS_ROLLBACK_BUG                         0x00800000L
     * #define SSL_OP_NO_SSLv2                                 0x01000000L
     * #define SSL_OP_NO_SSLv3                                 0x02000000L
     * #define SSL_OP_NO_TLSv1                                 0x04000000L
     * #define SSL_OP_PKCS1_CHECK_1                            0x08000000L
     * #define SSL_OP_PKCS1_CHECK_2                            0x10000000L
     * #define SSL_OP_NETSCAPE_CA_DN_BUG                       0x20000000L
     * #define SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG          0x40000000L
     */
    if (opt_string == "SSL_OP_MICROSOFT_SESS_ID_BUG") {
        ret = SSL_OP_MICROSOFT_SESS_ID_BUG;
    } else if (opt_string == "SSL_OP_NETSCAPE_CHALLENGE_BUG") {
        ret = SSL_OP_NETSCAPE_CHALLENGE_BUG;
    } else if (opt_string == "SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG") {
        ret = SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG;
    } else if (opt_string == "SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG") {
        ret = SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG;
    } else if (opt_string == "SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER") {
        ret = SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER;
    } else if (opt_string == "SSL_OP_MSIE_SSLV2_RSA_PADDING") {
        ret = SSL_OP_MSIE_SSLV2_RSA_PADDING;
    } else if (opt_string == "SSL_OP_SSLEAY_080_CLIENT_DH_BUG") {
        ret = SSL_OP_SSLEAY_080_CLIENT_DH_BUG;
    } else if (opt_string == "SSL_OP_TLS_D5_BUG") {
        ret = SSL_OP_TLS_D5_BUG;
    } else if (opt_string == "SSL_OP_TLS_BLOCK_PADDING_BUG") {
        ret = SSL_OP_TLS_BLOCK_PADDING_BUG;
    } else if (opt_string == "SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS") {
        ret = SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
    } else if (opt_string == "SSL_OP_ALL") {
        // boost::asio::ssl::context::default_workarounds
        ret = SSL_OP_ALL;
    } else if (opt_string == "SSL_OP_NO_QUERY_MTU") {
        ret = SSL_OP_NO_QUERY_MTU;
    } else if (opt_string == "SSL_OP_COOKIE_EXCHANGE") {
        ret = SSL_OP_COOKIE_EXCHANGE;
    } else if (opt_string == "SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION") {
        ret = SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION;
    } else if (opt_string == "SSL_OP_SINGLE_ECDH_USE") {
        ret = SSL_OP_SINGLE_ECDH_USE;
    } else if (opt_string == "SSL_OP_SINGLE_DH_USE") {
        // boost::asio::ssl::context::single_dh_use
        ret = SSL_OP_SINGLE_DH_USE;
    } else if (opt_string == "SSL_OP_EPHEMERAL_RSA") {
        ret = SSL_OP_EPHEMERAL_RSA;
    } else if (opt_string == "SSL_OP_CIPHER_SERVER_PREFERENCE") {
        ret = SSL_OP_CIPHER_SERVER_PREFERENCE;
    } else if (opt_string == "SSL_OP_TLS_ROLLBACK_BUG") {
        ret = SSL_OP_TLS_ROLLBACK_BUG;
    } else if (opt_string == "SSL_OP_NO_SSLv2") {
        // boost::asio::ssl::context::no_sslv2
        ret = SSL_OP_NO_SSLv2;
    } else if (opt_string == "SSL_OP_NO_SSLv3") {
        // boost::asio::ssl::context::no_sslv3
        ret = SSL_OP_NO_SSLv3;
    } else if (opt_string == "SSL_OP_NO_TLSv1") {
        // boost::asio::ssl::context::no_tlsv1
        ret = SSL_OP_NO_TLSv1;
    } else if (opt_string == "SSL_OP_PKCS1_CHECK_1") {
        ret = SSL_OP_PKCS1_CHECK_1;
    } else if (opt_string == "SSL_OP_PKCS1_CHECK_2") {
        ret = SSL_OP_PKCS1_CHECK_2;
    } else if (opt_string == "SSL_OP_NETSCAPE_CA_DN_BUG") {
        ret = SSL_OP_NETSCAPE_CA_DN_BUG;
    } else if (opt_string == "SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG") {
        ret = SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
        std::stringstream buf;
        buf << "out_function : long int virtualservice_tcp::conv_ssl_option(std::string opt_string) : ";
        buf << "return_value = " << ret;
        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                    buf.str(),
                    __FILE__, __LINE__ );
    }
    /*------ DEBUG LOG END ------*/
    // if ret == -1 then No match.
    return ret;
}

/*!
 * get ssl parameter
 *
 * @return get ssl parameter result
 */
bool l7vs::virtualservice_tcp::get_ssl_parameter()
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
            "in_function : bool virtualservice_tcp::get_ssl_parameter()",
             __FILE__, __LINE__ );
    }
    /*------ DEBUG LOG END ------*/
    typedef std::vector< std::string > string_vector_type;

    Parameter param;
    string_vector_type string_vector;
    l7vs::error_code err;
    bool retbool = false;

    try {
        // param init ( ssl configuration file )
        if (unlikely(!param.init(l7vs::PARAM_COMP_SSL, ssl_file_name))) {
            Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                                "SSL config file read error.",
                                __FILE__, __LINE__ );
            throw -1;
        }

        //// SSL context parameter
        // Get parameter "ca_dir".
        ca_dir = param.get_string(l7vs::PARAM_COMP_SSL, "ca_dir", err);
        if (unlikely(err) || ca_dir == "") {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                               "ca_dir parameter not found. Use default value.",
                               __FILE__, __LINE__ );
            ca_dir = DEFAULT_SSL_CA_DIR;
        }

        // Get parameter "ca_file".
        ca_file = param.get_string(l7vs::PARAM_COMP_SSL, "ca_file", err);
        if (unlikely(err)) {
            Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        "Cannot get ca_file parameter.",
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Get parameter "cert_chain_dir".
        cert_chain_dir = param.get_string(l7vs::PARAM_COMP_SSL,
                                          "cert_chain_dir",
                                          err);
        if (unlikely(err) || cert_chain_dir == "") {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                       "cert_chain_dir parameter not found. Use default value.",
                       __FILE__, __LINE__ );
            cert_chain_dir = DEFAULT_SSL_CERT_CHAIN_DIR;
        }

        // Get parameter "cert_chain_file".
        cert_chain_file = param.get_string(l7vs::PARAM_COMP_SSL,
                                           "cert_chain_file",
                                           err);
        if (unlikely(err) || cert_chain_file == "") {
            Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        "Cannot get cert_chain_file parameter.",
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Get parameter "private_key_dir".
        private_key_dir = param.get_string(l7vs::PARAM_COMP_SSL,
                                           "private_key_dir",
                                           err);
        if (unlikely(err) || private_key_dir == "") {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                    "private_key_dir parameter not found. Use default value.",
                    __FILE__, __LINE__ );
            private_key_dir = DEFAULT_SSL_PRIVATE_KEY_DIR;
        }

        // Get parameter "private_key_file".
        private_key_file = param.get_string(l7vs::PARAM_COMP_SSL,
                                            "private_key_file",
                                            err);
        if (unlikely(err) || private_key_file == "") {
            Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        "Cannot get private_key_file parameter.",
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Get parameter "private_key_filetype".
        // and convert string to filetype define value.
        /*!
         * /usr/include/openssl/ssl.h
         * #define SSL_FILETYPE_PEM        X509_FILETYPE_PEM  ->1
         * #define SSL_FILETYPE_ASN1       X509_FILETYPE_ASN1 ->2
         */

        std::string filetype_str = param.get_string(
                                       l7vs::PARAM_COMP_SSL,
                                       "private_key_filetype",
                                       err);
        if (unlikely(err) || filetype_str == "") {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                "private_key_filetype parameter not found. Use default value.",
                __FILE__, __LINE__ );
            private_key_filetype = DEFAULT_SSL_PRIVATE_KEY_FILETYPE;
        } else if (filetype_str == "SSL_FILETYPE_PEM") {
            private_key_filetype = boost::asio::ssl::context::pem;
        } else if (filetype_str == "SSL_FILETYPE_ASN1") {
            private_key_filetype = boost::asio::ssl::context::asn1;
        } else {
            Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        "private_key_filetype convert error.",
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Get parameter "private_key_passwd_dir".
        private_key_passwd_dir = param.get_string(l7vs::PARAM_COMP_SSL,
                                                  "private_key_passwd_dir",
                                                  err);
        if (unlikely(err) || private_key_passwd_dir == "") {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
              "private_key_passwd_dir parameter not found. Use default value.",
              __FILE__, __LINE__ );
            private_key_passwd_dir = DEFAULT_SSL_PRIVATE_KEY_PASSWD_DIR;
        }

        // Get parameter "private_key_passwd_file".
        private_key_passwd_file = param.get_string(l7vs::PARAM_COMP_SSL,
                                                   "private_key_passwd_file",
                                                   err);
        if (unlikely(err) || private_key_passwd_file == "") {
            Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        "Cannot get private_key_passwd_file parameter.",
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Get parameter "verify_options".
        param.get_multistring(l7vs::PARAM_COMP_SSL,
                              "verify_options",
                              string_vector,
                              err);
        if (unlikely(err)) {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                       "verify_options parameter not found. Use default value.",
                       __FILE__, __LINE__ );
            verify_options = DEFAULT_SSL_VERIFY_OPTIONS;
        } else {
            // Make verify option bit data.
            for (string_vector_type::iterator itr = string_vector.begin();
                 itr != string_vector.end(); ++itr) {
                // Convert string to define value.
                int int_val = conv_verify_option(*itr);
                if (unlikely(int_val == -1)) {
                    Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                                "verify_options convert error.",
                                __FILE__, __LINE__ );
                    throw -1;
                }
                verify_options = (verify_options | int_val);
            }
        }

        // Get parameter "verify_cert_depth".
        verify_cert_depth = param.get_int(l7vs::PARAM_COMP_SSL,
                                          "verify_cert_depth",
                                          err);
        if (unlikely(err)) {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                "verify_cert_depth parameter not found. Use default value.",
                __FILE__, __LINE__ );
            verify_cert_depth = DEFAULT_SSL_VERIFY_CERT_DEPTH;
        } else if (unlikely(verify_cert_depth < 0 ||
                   verify_cert_depth > INT_MAX)) {
            Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        "Invalid verify_cert_depth parameter value.",
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Get parameter "ssl_options".
        // and Check dh parameter file use or not.
        is_tmp_dh_use = false;
        param.get_multistring(l7vs::PARAM_COMP_SSL,
                              "ssl_options",
                              string_vector,
                              err);
        if (unlikely(err)) {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                       "ssl_options parameter not found. Use default value.",
                       __FILE__, __LINE__ );
            ssl_options = DEFAULT_SSL_OPTIONS;
            is_tmp_dh_use = true;
        } else {
            // Make ssl option bit data.
            for (string_vector_type::iterator itr = string_vector.begin();
                 itr != string_vector.end(); ++itr) {
                // Convert string to define value.
                long int longint_val = conv_ssl_option(*itr);
                if (unlikely(longint_val == -1)) {
                    Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                                "ssl_options convert error.",
                                __FILE__, __LINE__ );
                    throw -1;
                } else if (longint_val == SSL_OP_SINGLE_DH_USE) {
                    is_tmp_dh_use = true;
                }
                ssl_options = (ssl_options | longint_val);
            }
        }

        if (is_tmp_dh_use) {
            // Get parameter "tmp_dh_dir".
            tmp_dh_dir = param.get_string(l7vs::PARAM_COMP_SSL,
                                          "tmp_dh_dir",
                                          err);
            if (unlikely(err) || tmp_dh_dir == "") {
                Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                           "tmp_dh_dir parameter not found. Use default value.",
                           __FILE__, __LINE__ );
                tmp_dh_dir = DEFAULT_SSL_TMP_DH_DIR;
            }
           // Get parameter "tmp_dh_file".
            tmp_dh_file = param.get_string(l7vs::PARAM_COMP_SSL,
                                           "tmp_dh_file",
                                           err);
            if (unlikely(err) || tmp_dh_file == "") {
                Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                            "Cannot get tmp_dh_file parameter.",
                            __FILE__, __LINE__ );
                throw -1;
            }
        }

        // Get parameter "cipher_list".
        cipher_list = param.get_string(l7vs::PARAM_COMP_SSL,
                                       "cipher_list",
                                       err);
        if (unlikely(err) || cipher_list == "") {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                       "cipher_list parameter not found. Use default value.",
                       __FILE__, __LINE__ );
            cipher_list = DEFAULT_SSL_CIPHER_LIST;
        }

        //// SSL session cache parameter
        // Get parameter "session_cache".
        is_session_cache_use = false;
        std::string cache_str = param.get_string(l7vs::PARAM_COMP_SSL,
                                                 "session_cache",
                                                 err);
        if (unlikely(err) || cache_str == "") {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                       "session_cache parameter not found. Use default value.",
                       __FILE__, __LINE__ );
            is_session_cache_use = true;
        } else if (cache_str == "on") {
            is_session_cache_use = true;
        } else if (cache_str == "off") {
            is_session_cache_use = false;
        } else {
            Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        "Invalid session_cache parameter value.",
                        __FILE__, __LINE__ );
            throw -1;
        }

        if (is_session_cache_use) {
            session_cache_mode = DEFAULT_SSL_SESSION_CACHE_MODE;
            // Get parameter "session_cache_size".
            session_cache_size = param.get_int(l7vs::PARAM_COMP_SSL,
                                               "session_cache_size",
                                               err);
            if (unlikely(err)) {
                Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                  "session_cache_size parameter not found. Use default value.",
                  __FILE__, __LINE__ );
                session_cache_size = DEFAULT_SSL_SESSION_CACHE_SIZE;
            } else if (session_cache_size < 0 ||
                       session_cache_size > LONG_MAX) {
                Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                            "Invalid session_cache_size parameter value.",
                            __FILE__, __LINE__ );
                throw -1;
            }
            // Get parameter "session_cache_timeout".
            session_cache_timeout = param.get_int(l7vs::PARAM_COMP_SSL,
                                                  "session_cache_timeout",
                                                  err);
            if (unlikely(err)) {
                Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                "session_cache_timeout parameter not found. Use default value.",
                __FILE__, __LINE__ );
                session_cache_timeout = DEFAULT_SSL_SESSION_CACHE_TIMEOUT;
            } else if (session_cache_timeout < 0 ||
                       session_cache_timeout > LONG_MAX) {
                Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                            "Invalid session_cache_timeout parameter value.",
                            __FILE__, __LINE__ );
                throw -1;
            }
        } else {
            session_cache_mode = SSL_SESS_CACHE_OFF;
        }

        //// SSL handshake timer parameter
        // Get parameter "timeout_sec".
        handshake_timeout = param.get_int(l7vs::PARAM_COMP_SSL,
                                          "timeout_sec",
                                          err);
        if (unlikely(err)) {
            Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                       "timeout_sec parameter not found. Use default value.",
                       __FILE__, __LINE__ );
            handshake_timeout = DEFAULT_SSL_HANDSHAKE_TIMEOUT;
        } else if (handshake_timeout <= 0 || handshake_timeout > INT_MAX) {
            Logger::putLogError(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        "Invalid timeout_sec parameter value.",
                        __FILE__, __LINE__ );
            throw -1;
        }

        retbool = true;

    } catch (int e) {
        retbool = false;
    }

    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
        std::stringstream buf;
        buf<<"out_function : bool virtualservice_tcp::get_ssl_parameter() : ";
        buf<<"ca_dir = "                 << ca_dir                  << ", ";
        buf<<"ca_file = "                << ca_file                 << ", ";
        buf<<"cert_chain_dir = "         << cert_chain_dir          << ", ";
        buf<<"cert_chain_file = "        << cert_chain_file         << ", ";
        buf<<"private_key_dir = "        << private_key_dir         << ", ";
        buf<<"private_key_file = "       << private_key_file        << ", ";
        buf<<"private_key_filetype = "   << private_key_filetype    << ", ";
        buf<<"private_key_passwd_dir = " << private_key_passwd_dir  << ", ";
        buf<<"private_key_passwd_file = "<< private_key_passwd_file << ", ";
        buf<<"verify_options = "         << verify_options          << ", ";
        buf<<"verify_cert_depth = "      << verify_cert_depth       << ", ";
        buf<<"ssl_options = "            << ssl_options             << ", ";
        if (is_tmp_dh_use) {
            buf<< "tmp_dh_dir = "        << tmp_dh_dir              << ", ";
            buf<< "tmp_dh_file = "       << tmp_dh_file             << ", ";
        }
        buf<<"cipher_list = "            << cipher_list             << ", ";
        buf<<"session_cache_mode = "     << session_cache_mode      << ", ";
        if (is_session_cache_use) {
            buf<<"session_cache_size = " << session_cache_size      << ", ";
            buf<<"session_cache_timeout = "<< session_cache_timeout << ", ";
        }
        buf<<"handshake_timeout = "      << handshake_timeout;
        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                            buf.str(),
                            __FILE__, __LINE__ );
    }
    /*------ DEBUG LOG END ------*/
    return retbool;
}

/*!
 * set ssl context and ssl session cache configuration
 *
 * @return set ssl config result
 */
bool    l7vs::virtualservice_tcp::set_ssl_config()
{
    /*-------- DEBUG LOG --------*/
    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
            "in_function : bool virtualservice_tcp::set_ssl_config()",
             __FILE__, __LINE__ );
    }
    /*------ DEBUG LOG END ------*/

    bool retbool = false;

    try {
        //// SSL context setting.
        // Set root CA.
        if (ca_file.size() == 0) {
            // specified CA path.
            try {
                sslcontext.add_verify_path(ca_dir);
            } catch (std::exception& e) {
                std::stringstream buf;
                buf << "Set root CA path error : " << e.what() << ".";
                Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                            buf.str(),
                            __FILE__, __LINE__ );
                throw -1;
            }
        } else {
            // specified CA file.
            try {
                sslcontext.load_verify_file(ca_dir + ca_file);
            } catch (std::exception& e) {
                std::stringstream buf;
                buf << "Set root CA file error : " << e.what() << ".";
                Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                            buf.str(),
                            __FILE__, __LINE__ );
                throw -1;
            }
        }

        // Set certificate chain file.
        try {
            sslcontext.use_certificate_chain_file(cert_chain_dir + cert_chain_file);
        } catch (std::exception& e) {
            std::stringstream buf;
            buf << "Set certificate chain file error : " << e.what() << ".";
            Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        buf.str(),
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Set password callback function.
        try {
            sslcontext.set_password_callback(boost::bind(&virtualservice_tcp::get_ssl_password, this));
        } catch (std::exception& e) {
            std::stringstream buf;
            buf << "Set password callback error : " << e.what() << ".";
            Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        buf.str(),
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Set private key file and filetype.
        try {
            sslcontext.use_private_key_file(private_key_dir + private_key_file, private_key_filetype);
        } catch (std::exception& e) {
            std::stringstream buf;
            buf << "Set private key file and filetype error : " << e.what() << ".";
            Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        buf.str(),
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Set verify options on the context.
        try {
            sslcontext.set_verify_mode(verify_options);
        } catch (std::exception& e) {
            std::stringstream buf;
            buf << "Set verify option error : " << e.what() << ".";
            Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        buf.str(),
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Set verify depth on the context.
        SSL_CTX_set_verify_depth(sslcontext.impl(), verify_cert_depth);

        // Set SSL options on the context.
        try {
            sslcontext.set_options(ssl_options);
        } catch (std::exception& e) {
            std::stringstream buf;
            buf << "Set SSL option error : " << e.what() << ".";
            Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        buf.str(),
                        __FILE__, __LINE__ );
            throw -1;
        }

        // Set temporary Diffie-Hellman parameters file.
        if (is_tmp_dh_use) {
            try {
                sslcontext.use_tmp_dh_file(tmp_dh_dir + tmp_dh_file);
            } catch (std::exception& e) {
                std::stringstream buf;
                buf << "Set tmp DH file error : " << e.what() << ".";
                Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                            buf.str(),
                            __FILE__, __LINE__ );
                throw -1;
            }

        }

        // Set cipher list on the context.
        if (unlikely(SSL_CTX_set_cipher_list(sslcontext.impl(), cipher_list.c_str()) != 1)) {
            std::stringstream buf;
            buf << "Set cipher list error.";
            Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                        buf.str(),
                        __FILE__, __LINE__ );
            throw -1;
        }

        //// SSL session cache setting.
        if (is_session_cache_use) {
            // Set session id context on the context.
            if (unlikely(SSL_CTX_set_session_id_context(sslcontext.impl(), (const unsigned char *)"sslproxy", 8) != 1)) {
                std::stringstream buf;
                buf << "Set session id context error.";
                Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                            buf.str(),
                            __FILE__, __LINE__ );
                throw -1;
            }

            // Set session cache mode on the context.
            SSL_CTX_set_session_cache_mode(sslcontext.impl(), session_cache_mode);

            // Set session cache size on the context.
            SSL_CTX_sess_set_cache_size(sslcontext.impl(), session_cache_size);

            // Set session cache timeout on the context.
            SSL_CTX_set_timeout(sslcontext.impl(), session_cache_timeout);

//// For external SSL session cache
//            // Set session cache mode on the context (no internal cache).
//            SSL_CTX_set_session_cache_mode(sslcontext.impl(), session_cache_mode | SSL_SESS_CACHE_NO_INTERNAL);
//            // Set SSL session cache callback function.(new/get/remove)
//            int (*pFunc)(ssl_st*, SSL_SESSION*) = &virtualservice_tcp::new_session_cb;
//            SSL_CTX_sess_set_new_cb(sslcontext.impl(), pFunc);
//            SSL_CTX_sess_set_new_cb(sslcontext.impl(), new_session_cb);
//            SSL_CTX_sess_set_get_cb(sslcontext.impl(), get_session_cb);
//            SSL_CTX_sess_set_remove_cb(sslcontext.impl(), remove_session_cb);
        } else {
            // session cache OFF.
            SSL_CTX_set_session_cache_mode(sslcontext.impl(), SSL_SESS_CACHE_OFF);
        }

        retbool = true;

    } catch (int e) {
        retbool = false;
    }
    /*-------- DEBUG LOG --------*/
//    if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
        std::stringstream buf;
        buf << "out_function : bool virtualservice_tcp::set_ssl_config() : ";
        get_ssl_config(buf);
        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 999,
                    buf.str(),
                    __FILE__, __LINE__ );
//    }
    /*------ DEBUG LOG END ------*/
    return retbool;
}

/*!
 * flush ssl session
 */
void    l7vs::virtualservice_tcp::flush_ssl_session()
{
    // check expired cached sessions and do flushing
    // Need ssl_context lock?
    SSL_CTX_flush_sessions(sslcontext.impl(), time(0));
}

//// For external SSL session cache
//! SSL session cache table
//std::map<std::string, SSL_SESSION>        sessioncacheTable;
//boost::mutex                    sessioncacheTable_mutex;

//// For external SSL session cache
//! default value of session-id size and session cache table size
//#define MAX_SESSION_ID_SIZE SSL_MAX_SSL_SESSION_ID_LENGTH * 2 + 1
//#define MAX_SESSION_CACHE_SIZE 10

/*
//// For external SSL session cache
//! session callback functions (new/remove/get)
//  global function -> OK. virtualservice_tcp class member function -> NG.
//static int new_session_cb(SSL *ssl, SSL_SESSION *session)
int l7vs::virtualservice_tcp::new_session_cb(SSL *ssl, SSL_SESSION *session)
{
    {
        std::stringstream buf;
        buf << "Callback new_session_cb() SSL_MAX_SSL_SESSION_ID_LENGTH[";
        buf << SSL_MAX_SSL_SESSION_ID_LENGTH;
        buf << "]";
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
    }

    char session_key[MAX_SESSION_ID_SIZE] = {0};

    // Make session_key for add.
    for (unsigned int i = 0; i < session->session_id_length; i++) {
        sprintf(session_key, "%s%02X", session_key, session->session_id[i]);
    }
    session_key[strlen(session_key)] = '\0';

    struct timeval now;

    gettimeofday(&now, NULL);

    std::vector<std::string> cleanup_list;

    boost::mutex::scoped_lock sclock(sessioncacheTable_mutex);

    if (sessioncacheTable.size() >= MAX_SESSION_CACHE_SIZE) {
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In new_session_cb() : sessioncacheTable overflow. Cleanup session.", __FILE__, __LINE__ );

        std::map<std::string, SSL_SESSION>::iterator it = sessioncacheTable.begin();
        std::map<std::string, SSL_SESSION>::iterator ite = sessioncacheTable.end();

        for (std::map<std::string, SSL_SESSION>::iterator its = it; its != ite; its++) {
            {
                std::stringstream buf;
                buf << "For clean session.time[";
                buf << its->second.time;
                buf << "] session.timeout[";
                buf << its->second.timeout;
                buf << "] now[";
                buf << now.tv_sec;
                buf << "]";
                l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
            }
            {
                std::stringstream buf;
                buf << "For clean session_id[";
                buf << its->first;
                buf << "]";
                l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
            }
            // Check expire.
            if (its->second.time + its->second.timeout < now.tv_sec) {
                cleanup_list.push_back(its->first);
                // sessioncacheTable.erase(its);
                // 消した状態でイテレータループまわすと消したものにもアクセスする？
                l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In new_session_cb() : Expire session found.", __FILE__, __LINE__ );
            }
        }
        {
            std::stringstream buf;
            buf << "In new_session_cb() : Expire session count[";
            buf << cleanup_list.size();
            buf << "]";
            l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
        }
        for (unsigned int i = 0; i < cleanup_list.size(); i++) {
            sessioncacheTable.erase(cleanup_list[i]);
        }
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In new_session_cb() : Cleanup END.", __FILE__, __LINE__ );
    }

    std::string ssid = boost::lexical_cast<std::string>(session_key);
    {
        std::stringstream buf;
        buf << "String session_id[";
        buf << ssid;
        buf << "] len[";
        buf << ssid.size();
        buf << "]";
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
    }
    {
        std::stringstream buf;
        buf << "For new session.time[";
        buf << session->time;
        buf << "] session.timeout[";
        buf << session->timeout;
        buf << "] now[";
        buf << now.tv_sec;
        buf << "]";
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
    }

    if (sessioncacheTable.size() < MAX_SESSION_CACHE_SIZE) {
        if (sessioncacheTable.end() != sessioncacheTable.find(ssid)) {
            sessioncacheTable.erase(ssid);
            l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In new_session_cb() : Session already exist. Session erased.", __FILE__, __LINE__ );
        }
        memcpy(&sessioncacheTable[ssid], session, sizeof(SSL_SESSION));
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In new_session_cb() : Session added.", __FILE__, __LINE__ );
    } else {
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In new_session_cb() : sessioncacheTable overflow. Session not added.", __FILE__, __LINE__ );
    }

    {
        std::stringstream buf;
        buf << "sessioncacheTable.size[";
        buf << sessioncacheTable.size();
        buf << "]";
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
    }

    return 0;
}

//static SSL_SESSION* get_session_cb(SSL *ssl, unsigned char *session_id, int session_id_len, int *ref)
SSL_SESSION* l7vs::virtualservice_tcp::get_session_cb(SSL *ssl, unsigned char *session_id, int session_id_len, int *ref)
{
    l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "Callback get_session_cb()", __FILE__, __LINE__ );

    char session_key[MAX_SESSION_ID_SIZE] = {0};

    // Make session_key for get.
    for (int i = 0; i < session_id_len; i++) {
        sprintf(session_key, "%s%02X", session_key, session_id[i]);
    }
    session_key[strlen(session_key)] = '\0';

    SSL_SESSION *ret_session = NULL;
    struct timeval now;

    gettimeofday(&now, NULL);

    boost::mutex::scoped_lock sclock(sessioncacheTable_mutex);

    std::string ssid = boost::lexical_cast<std::string>(session_key);
    {
        std::stringstream buf;
        buf << "String session_id[";
        buf << ssid;
        buf << "] len[";
        buf << ssid.size();
        buf << "]";
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
    }

    if (sessioncacheTable.end() != sessioncacheTable.find(ssid)) {
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In get_session_cb() : Session found.", __FILE__, __LINE__ );
        {
            std::stringstream buf;
            buf << "For get sessioncacheTable[ssid].time[";
            buf << sessioncacheTable[ssid].time;
            buf << "] sessioncacheTable[ssid].timeout[";
            buf << sessioncacheTable[ssid].timeout;
            buf << "] now[";
            buf << now.tv_sec;
            buf << "]";
            l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
        }
        // Check expire.
        if (sessioncacheTable[ssid].time + sessioncacheTable[ssid].time < now.tv_sec) {
            sessioncacheTable.erase(ssid);
            ret_session = NULL;
            l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In get_session_cb() : Session expired. Session erased.", __FILE__, __LINE__ );
        } else {
            ret_session = &sessioncacheTable[ssid];
            l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In get_session_cb() : Get session OK.", __FILE__, __LINE__ );
        }
    } else {
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In get_session_cb() : Session not found.", __FILE__, __LINE__ );
    }

    ref = 0;
    return ret_session;
}

//static void remove_session_cb(SSL_CTX *ssl_ctx, SSL_SESSION *session)
void l7vs::virtualservice_tcp::remove_session_cb(SSL_CTX *ssl_ctx, SSL_SESSION *session)
{
    l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "Callback remove_session_cb()", __FILE__, __LINE__ );

    char session_key[MAX_SESSION_ID_SIZE] = {0};

    // Make session_key for add.
    for (unsigned int i = 0; i < session->session_id_length; i++) {
        sprintf(session_key, "%s%02X", session_key, session->session_id[i]);
    }
    session_key[strlen(session_key)] = '\0';

    struct timeval now;

    gettimeofday(&now, NULL);

    boost::mutex::scoped_lock sclock(sessioncacheTable_mutex);

    std::string ssid = boost::lexical_cast<std::string>(session->session_id);
    {
        std::stringstream buf;
        buf << "String session_id[";
        buf << ssid;
        buf << "] len[";
        buf << ssid.size();
        buf << "]";
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
    }
    {
        std::stringstream buf;
        buf << "For remove session.time[";
        buf << session->time;
        buf << "] session.timeout[";
        buf << session->timeout;
        buf << "] now[";
        buf << now.tv_sec;
        buf << "]";
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
    }
    if(sessioncacheTable.end() != sessioncacheTable.find(ssid)) {
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In remove_session_cb() : Session found. ", __FILE__, __LINE__ );
        {
            std::stringstream buf;
            buf << "sessioncacheTable[ssid].time[";
            buf << sessioncacheTable[ssid].time;
            buf << "] sessioncacheTable[ssid].timeout[";
            buf << sessioncacheTable[ssid].timeout;
            buf << "] now[";
            buf << now.tv_sec;
            buf << "]";
            l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, buf.str(), __FILE__, __LINE__ );
        }
        // Check expire.
        if (sessioncacheTable[ssid].time + sessioncacheTable[ssid].time < now.tv_sec) {
            sessioncacheTable.erase(ssid);
            l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In remove_session_cb() : Remove session OK.", __FILE__, __LINE__ );
        } else {
            l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In remove_session_cb() : Session not expired. Session not erased.", __FILE__, __LINE__ );
        }
    } else {
        l7vs::Logger::putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 999, "In remove_session_cb() : Session not found.", __FILE__, __LINE__ );
    }

    return;
}
//// For external SSL session cache
*/

/*!
 * get ssl configuration form ssl context. (for debug)
 */
void l7vs::virtualservice_tcp::get_ssl_config(std::stringstream& buf)
{
    buf << "SSL configuration information : ";
    buf << "Verify mode["    << SSL_CTX_get_verify_mode(sslcontext.impl())        << "] ";
    buf << "Verify depth["    << SSL_CTX_get_verify_depth(sslcontext.impl())        << "] ";
    buf << "SSL options["    << SSL_CTX_get_options(sslcontext.impl())        << "] ";
    buf << "Cache mode["    << SSL_CTX_get_session_cache_mode(sslcontext.impl())    << "] ";
    buf << "Cache size["    << SSL_CTX_sess_get_cache_size(sslcontext.impl())    << "] ";
    buf << "Cache timeout["    << SSL_CTX_get_timeout(sslcontext.impl())        << "] ";
}

/*!
 * get ssl session cache information form ssl context. (for debug)
 */
void l7vs::virtualservice_tcp::get_ssl_session_cache_info(std::stringstream& buf)
{
    // Need Lock?
    buf << "SSL session cache information : ";
    buf << "Session number["    << SSL_CTX_sess_number(sslcontext.impl())        << "] ";
    buf << "Accept["        << SSL_CTX_sess_accept(sslcontext.impl())        << "] ";
    buf << "Accept good["        << SSL_CTX_sess_accept_good(sslcontext.impl())        << "] ";
    buf << "Accept renegotiate["    << SSL_CTX_sess_accept_renegotiate(sslcontext.impl())    << "] ";
    buf << "Hits["            << SSL_CTX_sess_hits(sslcontext.impl())            << "] ";
    buf << "Misses["        << SSL_CTX_sess_misses(sslcontext.impl())        << "] ";
    buf << "Timeouts["        << SSL_CTX_sess_timeouts(sslcontext.impl())        << "] ";
    buf << "Cache full["        << SSL_CTX_sess_cache_full(sslcontext.impl())        << "] ";
}
