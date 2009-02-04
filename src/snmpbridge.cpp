/*
 * @file  snmpbridge.cpp
 * @brief snmpagent connection module.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2005  NTT COMWARE Corporation.
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
#include <map>
#include <vector>
#include <boost/lexical_cast.hpp>
/*
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
*/
#include <string.h>
#include <error.h>
#include "realserver_element.h"
#include "realserver.h"
#include "virtualservice_element.h"
#include "virtualservice.h"
#include "snmpbridge.h"
#include "logger.h"
#include "parameter.h"
#include "../snmpagent/message.h"

/*!
 * initialize function
 * @return  int  0 is nomal end not 0 is abnomal end
 */
int l7vs::snmpbridge::initialize(){
    int retval =  0;
//    int fd     = -1;
//    int on     =  1;

    //cleate log level table
    snmp_loglevel_map.clear();
    snmp_loglevel_map.insert( std::pair<l7vs::LOG_CATEGORY_TAG,std::string>( LOG_CAT_SNMPAGENT_START_STOP, "snmpagent_start_stop" ) );
    snmp_loglevel_map.insert( std::pair<l7vs::LOG_CATEGORY_TAG,std::string>( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE, "snmpagent_manager_receive" ) );
    snmp_loglevel_map.insert( std::pair<l7vs::LOG_CATEGORY_TAG,std::string>( LOG_CAT_SNMPAGENT_MANAGER_SEND, "snmpagent_manager_send" ) );
    snmp_loglevel_map.insert( std::pair<l7vs::LOG_CATEGORY_TAG,std::string>( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE, "snmpagent_l7vsd_receive" ) );
    snmp_loglevel_map.insert( std::pair<l7vs::LOG_CATEGORY_TAG,std::string>( LOG_CAT_SNMPAGENT_L7VSD_SEND, "snmpagent_l7vsd_send" ) );
    snmp_loglevel_map.insert( std::pair<l7vs::LOG_CATEGORY_TAG,std::string>( LOG_CAT_SNMPAGENT_LOGGER, "snmpagent_logger" ) );
    snmp_loglevel_map.insert( std::pair<l7vs::LOG_CATEGORY_TAG,std::string>( LOG_CAT_SNMPAGENT_PARAMETER, "snmpagent_parameter" ) );

    levelstring_map.clear();
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("debug",LOG_LV_DEBUG) );
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("info",LOG_LV_INFO) );
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("warn",LOG_LV_WARN) );
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("error",LOG_LV_ERROR) );
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("fatal",LOG_LV_FATAL) );

    if( load_config() != 0 ) return -1;//don't read config

//    std::string servicestr = boost::lexical_cast<std::string>( snmp_param.portno );
	boost::asio::ip::tcp::endpoint recv_endpoint( boost::asio::ip::tcp::v4(), snmp_param.portno );
//	boost::asio::ip::tcp::endpoint recv_endpoint( boost::asio::ip::address::from_string(snmp_param.address), snmp_param.portno );
	if( snmp_acceptor.is_open() == false ){
		snmp_acceptor.open( recv_endpoint.protocol() );
		snmp_acceptor.set_option( boost::asio::socket_base::reuse_address(true) );
		snmp_acceptor.bind( recv_endpoint );
		snmp_acceptor.listen();

		snmp_acceptor.async_accept( snmp_socket, boost::bind( &snmpbridge::handle_accept, this, boost::asio::placeholders::error ) );
	}
#if 0
    //Create Lisning Socket
    struct addrinfo hints;
    memset( &hints, 0, sizeof( struct addrinfo ) );
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_flags = AI_PASSIVE; don't connect another machine. only loopback address.
    struct addrinfo* res;
    std::string servicestr = boost::lexical_cast<std::string>( snmp_param.portno );
    int error = getaddrinfo( NULL, servicestr.c_str() , &hints, &res );
    if( error ){
        retval = -1;
        goto initialize_error_end;
    }
    for( struct addrinfo* itr = res; itr != NULL; itr = itr->ai_next ){
        if( itr->ai_family == AF_INET ){
            fd = socket( itr->ai_family,
                    itr->ai_socktype,
                    itr->ai_protocol );
            if( socket < 0 ){
                retval = -3;
                goto initialize_error_end;
            }
            break;
        }
    }
    if( fd < 0 ){
        retval = -3;
        goto initialize_error_end;
    }

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof(int));

    //bind socket
    if( bind( fd, res->ai_addr, res->ai_addrlen ) < 0 ){
        retval = -4;
        goto initialize_error_end;
    }

    //lisen
    if( listen( fd, 1 ) < 0 ){
        retval = -5;
        goto initialize_error_end;
    }

    // get iomux
    l7vs_snmp_listen_iomux = l7vs_iomux_get_from_avail_list();
    if (!l7vs_snmp_listen_iomux) {
        retval = -6;
        goto initialize_error_end;
    };
    //iom setting
    l7vs_snmp_listen_iomux->fd = fd;
    l7vs_snmp_listen_iomux->data = NULL;
    l7vs_snmp_listen_iomux->status = iomux_snmp_waiting;
    l7vs_snmp_listen_iomux->callback = l7vs_snmpbridge_listen_callback;

    //register iomux
    l7vs_iomux_add( l7vs_snmp_listen_iomux, iom_read );

    freeaddrinfo( res );
    return 0;

initialize_error_end:
    close( fd );
    freeaddrinfo( res );
#endif
    return retval;
}

/*!
 * finalize function
 * @param[in]
 * @return
 */
void l7vs::snmpbridge::finalize(){
#if 0
    //close socket
    close( l7vs_snmp_listen_iomux->fd );
    close( l7vs_snmp_iomux->fd );
    l7vs_snmp_iomux->fd = -1;
    l7vs_snmp_listen_iomux->fd = -1;
    //remove iomux.
    l7vs_iomux_remove( l7vs_snmp_listen_iomux );
    l7vs_iomux_remove( l7vs_snmp_iomux );
#endif
	connection_state = false;
	snmp_acceptor.close();
	snmp_socket.close();
//	if( snmp_acceptor.is_open() == true ){
//		snmp_acceptor.close();
//	}
//	if( snmp_socket.is_open() == true ){
//		snmp_socket.close();
//	}
    if( !send_buffer ){
        free(send_buffer);
        send_buffer      = NULL;
        send_buffer_size = 0;
	}
}

/*!
 * TRAP Request send
 * @param[in]   *message   Trap message
 * @return      int
 */
int l7vs::snmpbridge::send_trap( const std::string& message ){
#if 1
	boost::mutex::scoped_lock lock( send_buffer_mutex );

    if( send_buffer ) free( send_buffer );
    send_buffer_size = sizeof( struct l7ag_message_header ) +
                   sizeof( struct l7ag_payload_header ) +
                   sizeof( struct l7ag_traprequest_message );
    send_buffer = (char*) calloc( 1, send_buffer_size );
    if( !send_buffer ){
        //malloc error!
        return -1;
    }
    struct l7ag_message_header* header        = (struct l7ag_message_header*) send_buffer;
    struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) (header + 1);
    struct l7ag_traprequest_message* trapmes  = (struct l7ag_traprequest_message*) (payloadheader + 1);

    //create message header
    header->magic[0] = 0x4d;    // M
    header->magic[1] = 0x47;    // G
    header->version = 1;
    time_t  tim;
    time( &tim );
    header->time = ( unsigned long long ) tim;
    header->payload_count = 1;
    header->size = send_buffer_size;

    //create payload header
    payloadheader->magic[0] = 0x50; // P
    payloadheader->magic[1] = 0x59; // Y
    payloadheader->message_id = MESSAGE_ID_TRAPREQUEST;
    payloadheader->payload_datasize = sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_traprequest_message );

    //create message
    char oid[] = "1.3.6.1.4.1.60000.1.0.2";
    strncpy( trapmes->oid, oid , sizeof( oid ) );
    strncpy( trapmes->message, message.c_str(), TRAPREQUESTMESSAGESIZE );
    trapmes->magic[0] = 0x54;   // T
    trapmes->magic[1] = 0x52;   // R

    // register iom (send)
    if (send_message() != 0) {
        free(send_buffer);
        send_buffer      = NULL;
        send_buffer_size = 0;
        return -1;
    }
#endif

    return 0;
}

/*!
 * Reload config command to subagent
 */
void l7vs::snmpbridge::reload_config(){
#if 1
	boost::mutex::scoped_lock lock( send_buffer_mutex );

    if( send_buffer ) free( send_buffer );
    send_buffer_size = sizeof( struct l7ag_message_header ) +
                   sizeof( struct l7ag_payload_header ) +
                   sizeof( struct l7ag_settingcommand_message );

    send_buffer = (char*) calloc( 1, send_buffer_size );
    struct l7ag_message_header* header             = (struct l7ag_message_header*) send_buffer;
    struct l7ag_payload_header* payloadheader      = (struct l7ag_payload_header*) (header + 1);
    struct l7ag_settingcommand_message* settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);
    //create message header
    header->magic[0] = 0x4d;    // M
    header->magic[1] = 0x47;    // G
    header->version = 1;
    time_t  tim;
    time( &tim );
    header->time = ( unsigned long long ) tim;
    header->payload_count = 1;
    header->size = send_buffer_size;

    //create payload header
    payloadheader->magic[0] = 0x50; // P
    payloadheader->magic[1] = 0x59; // Y
    payloadheader->message_id = MESSAGE_ID_COMMANDREQUEST;
    payloadheader->payload_datasize = sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_settingcommand_message );

    //create settingcommand_message
    settingcmd->command_id = COMMAND_SETTINGFILE_RELOAD;
    memset( settingcmd->data, 0, COMMANDDATASIZE );
    settingcmd->magic[0] = 0x53;    // S
    settingcmd->magic[1] = 0x54;    // T

    // register iom (send)
    if(send_message() != 0 ){
        free(send_buffer);
        send_buffer      = NULL;
        send_buffer_size = 0;
    }
#endif
}

/*!
 * Set log level command to subagent
 */
int l7vs::snmpbridge::change_loglevel( const l7vs::LOG_CATEGORY_TAG snmp_log_category, const l7vs::LOG_LEVEL_TAG loglevel ){
    int retval = 0;
#if 1
	boost::mutex::scoped_lock lock( send_buffer_mutex );

    if( send_buffer ) free( send_buffer );
    send_buffer_size = sizeof( struct l7ag_message_header ) +
                   sizeof( struct l7ag_payload_header ) +
                   sizeof( struct l7ag_settingcommand_message );

    send_buffer = (char*) calloc( 1, send_buffer_size );
    struct l7ag_message_header* header             = (struct l7ag_message_header*) send_buffer;
    struct l7ag_payload_header* payloadheader      = (struct l7ag_payload_header*) (header + 1);
    struct l7ag_settingcommand_message* settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);
    struct l7ag_changeloglevel_parameter* param    = (struct l7ag_changeloglevel_parameter*) settingcmd->data;
    //create message header
    header->magic[0] = 0x4d;    // M
    header->magic[1] = 0x47;    // G
    header->version = 1;
    time_t  tim;
    time( &tim );
    header->time = ( unsigned long long ) tim;
    header->payload_count = 1;
    header->size = send_buffer_size;

    //create payload header
    payloadheader->magic[0] = 0x50; // P
    payloadheader->magic[1] = 0x59; // Y
    payloadheader->message_id = MESSAGE_ID_COMMANDREQUEST;
    payloadheader->payload_datasize = sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_settingcommand_message);

    //create settingcmd
    settingcmd->command_id  = COMMAND_LOGLEVEL_CHANGE;
    settingcmd->magic[0]    = 0x4c;    // L
    settingcmd->magic[1]    = 0x4c;    // L
    //create log level
    param->log_category = snmp_log_category;
    param->log_level = loglevel;

    // register iom (send)
    retval = send_message();
    if( retval != 0 ){
        free(send_buffer);
        send_buffer      = NULL;
        send_buffer_size = 0;
    }
#endif
    return retval;
}

/*!
 * Set all log level command to subagent
 */
int l7vs::snmpbridge::change_loglevel_allcategory( const l7vs::LOG_LEVEL_TAG loglevel ){
    int retval = 0;
#if 1
    std::vector<struct l7ag_settingcommand_message> settingcmd_vec;
    for( std::map<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>::iterator it = snmp_param.loglevel.begin();
        it != snmp_param.loglevel.end();
        ++it ){
        struct l7ag_settingcommand_message cmd;
        cmd.command_id = COMMAND_LOGLEVEL_CHANGE;
        struct l7ag_changeloglevel_parameter* param = (l7ag_changeloglevel_parameter*) cmd.data;
        param->log_category = it->first;
        param->log_level = loglevel;
        settingcmd_vec.push_back( cmd );
    }
	boost::mutex::scoped_lock lock( send_buffer_mutex );

    if( send_buffer ) free( send_buffer );
    send_buffer_size = sizeof( struct l7ag_message_header ) +
                   sizeof( struct l7ag_payload_header ) * settingcmd_vec.size() +
                   sizeof( struct l7ag_settingcommand_message ) * settingcmd_vec.size();
    send_buffer = (char*) calloc( 1, send_buffer_size );
    struct l7ag_message_header* header = (struct l7ag_message_header*) send_buffer;
    struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) (header + 1);
    struct l7ag_settingcommand_message* settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);
#if 0
    struct l7ag_changeloglevel_parameter* param = (l7ag_changeloglevel_parameter*) settingcmd->data;
#endif
    //create message header
    header->magic[0] = 0x4d;    // M
    header->magic[1] = 0x47;    // G
    header->version = 1;
    time_t tim;
    time( &tim );
    header->time = ( unsigned long long ) tim;
    header->payload_count = settingcmd_vec.size();
    header->size = send_buffer_size;

    //datas
    for( std::vector<struct l7ag_settingcommand_message>::iterator itr = settingcmd_vec.begin();
        itr != settingcmd_vec.end();
        ++itr ){
        payloadheader->magic[0] = 0x50; // P
        payloadheader->magic[1] = 0x59; // Y
        payloadheader->message_id = MESSAGE_ID_COMMANDREQUEST;
        payloadheader->payload_datasize = sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_settingcommand_message );
        settingcmd->command_id  = COMMAND_LOGLEVEL_CHANGE;
        settingcmd->magic[0]    = 0x4c;    // L
        settingcmd->magic[1]    = 0x4c;    // L
        memcpy( settingcmd->data, &(*itr), sizeof( struct l7ag_settingcommand_message) );
        payloadheader = (struct l7ag_payload_header*) (settingcmd + 1);
        settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);
    }

    // register iom (send)
    retval = send_message();
    if( retval != 0 ){
        free(send_buffer);
        send_buffer      = NULL;
        send_buffer_size = 0;
    }
#endif
    return retval;
}

/*!
 * Send all MIB data
 */
int l7vs::snmpbridge::send_mibcollection(struct l7ag_mibrequest_message* payload){
    int retval = 0;
#if 0
	boost::mutex::scoped_lock lock( send_buffer_mutex );

    if (send_buffer) {
        free(send_buffer);
        send_buffer      = NULL;
        send_buffer_size = 0;
    }

    // TODO should check payload oid and return specified MIB data only.

    std::vector<l7vs_service*> serviceptr_vec;
    int real_servers = 0;
    for (unsigned int i = 0; i < l7vs_service_get_VSnum(); ++i) {
        struct l7vs_service* s = l7vs_service_get_VSInfo_byNum( i );
        serviceptr_vec.push_back( s );
        real_servers += g_list_length( s->dest_list );
    }

    int payload_vs_count = 1;
    if (serviceptr_vec.size() > 1) {
        payload_vs_count = serviceptr_vec.size();
    }
    int payload_rs_count = 1;
    if (real_servers > 1) {
        payload_rs_count = real_servers;
    }
    send_buffer_size = sizeof( struct l7ag_message_header ) +
                 ( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * payload_vs_count +
                 ( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * payload_rs_count;

    send_buffer = (char*) calloc( 1, send_buffer_size );
    struct l7ag_message_header* header = (struct l7ag_message_header*) send_buffer;

    //create message header
    header->magic[0] = 0x4d;    // M
    header->magic[1] = 0x47;    // G
    header->version = 1;
    time_t tim;
    time( &tim );
    header->time = ( unsigned long long ) tim;
    header->payload_count = payload_vs_count + payload_rs_count;
    header->size = send_buffer_size;

    //create payload header
    void* pos = (void *) (header + 1);
    int vs_counter = 1;
    int rs_counter = 1;

    for( std::vector<l7vs_service*>::iterator itr = serviceptr_vec.begin();
        itr != serviceptr_vec.end();
        ++itr ){
        struct l7vs_service* srv = *itr;
        struct l7vs_service_arg_multi* multi = l7vs_service_get_service_arg(srv);
        struct l7vs_service_arg* sarg = &multi->srv_arg;

        struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) pos;
        payloadheader->magic[0] = 0x50; // P
        payloadheader->magic[1] = 0x59; // Y
        payloadheader->message_id = MESSAGE_ID_MIBCOLLECTRESPONSE_VS;
        payloadheader->payload_datasize = sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs);
        pos = payloadheader + 1;

        struct l7ag_mibdata_payload_vs* vs = (struct l7ag_mibdata_payload_vs*) pos;
        vs->magic[0]    = 0x56; // V
        vs->magic[1]    = 0x53; // S
        vs->index               = vs_counter;
        vs->protocol            = srv->lsock->proto;
        vs->ipAddress           = srv->lsock->addr.sin_addr.s_addr;
        vs->portNumber          = srv->lsock->addr.sin_port;
        vs->reschedule          = sarg->reschedule;
        vs->sorryIpAddress      = srv->sorry_dest->addr.sin_addr.s_addr;
        vs->sorryPortNumber     = srv->sorry_dest->addr.sin_port;
        vs->sorryThreshold      = srv->sorry_cc;
        vs->sorryForceFlag      = srv->sorry_flag;
        vs->QoSThresholdUp      = srv->qos_threshold_up;
        vs->QoSThresholdDown    = srv->qos_threshold_down;
        vs->throughputUp        = srv->throughput_to_server;
        vs->throughputDown      = srv->throughput_to_client;
        vs->vs_count            = payload_vs_count;
        strncpy(vs->scheduleModule, srv->scheduler->modname, L7VS_MODNAME_LEN);
        strncpy(vs->protocolModule, srv->pm->modname, L7VS_MODNAME_LEN);
        strncpy(vs->protocolModuleOption, sarg->protomod_opt_string, L7VS_PROTOMOD_OPT_LEN);
        pos = vs + 1;

        int rs_count = g_list_length( srv->dest_list );
        if (rs_count > 0) {
            std::vector<l7vs_dest*> destptr_vec;
            for (GList* l = g_list_first(srv->dest_list); l != NULL; l = g_list_next(l)) {
                struct l7vs_dest* dest = (struct l7vs_dest *)l->data;

                payloadheader = (struct l7ag_payload_header*) pos;
                payloadheader->magic[0] = 0x50; // P
                payloadheader->magic[1] = 0x59; // Y
                payloadheader->message_id = MESSAGE_ID_MIBCOLLECTRESPONSE_RS;
                payloadheader->payload_datasize = sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_rs);
                pos = payloadheader + 1;
                
                struct l7ag_mibdata_payload_rs* rs = (struct l7ag_mibdata_payload_rs*) pos;
                rs->magic[0]            = 0x52; // R
                rs->magic[1]            = 0x53; // S
                rs->index               = rs_counter;
                rs->virtualServiceIndex = vs_counter;
                rs->ipAddress           = dest->addr.sin_addr.s_addr;
                rs->portNumber          = dest->addr.sin_port;
                rs->forwardMode         = 1; // masq only
                rs->weight              = dest->weight;
                rs->activeConn          = dest->nactive;
                rs->inactiveConn        = dest->ninact;
                rs->rs_count            = real_servers;
                pos = rs + 1;

                rs_counter++;
            }
        }
        vs_counter++;
    }

    // for clear vs table
    if (serviceptr_vec.size() == 0) {
        struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) pos;
        payloadheader->magic[0] = 0x50; // P
        payloadheader->magic[1] = 0x59; // Y
        payloadheader->message_id = MESSAGE_ID_MIBCOLLECTRESPONSE_VS;
        payloadheader->payload_datasize = sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs);
        pos = payloadheader + 1;
        struct l7ag_mibdata_payload_vs* vs = (struct l7ag_mibdata_payload_vs*) pos;
        vs->magic[0]    = 0x56; // V
        vs->magic[1]    = 0x53; // S
        vs->index       = 0;
        vs->vs_count    = 0;
        pos = vs + 1;
    }
    // for clear rs table
    if (real_servers == 0) {
        struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) pos;
        payloadheader->magic[0] = 0x50; // P
        payloadheader->magic[1] = 0x59; // Y
        payloadheader->message_id = MESSAGE_ID_MIBCOLLECTRESPONSE_VS;
        payloadheader->payload_datasize = sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs);
        pos = payloadheader + 1;
        struct l7ag_mibdata_payload_rs* rs = (struct l7ag_mibdata_payload_rs*) pos;
        rs->magic[0]    = 0x52; // R
        rs->magic[1]    = 0x53; // S
        rs->index       = 0;
        rs->rs_count    = 0;
        pos = rs + 1;
    }

    // register iom (send)
    retval = send_message();
    if( retval != 0 ){
        free(send_buffer);
        send_buffer      = NULL;
        send_buffer_size = 0;
    }
#endif
    return retval;
}

/*!
 * Get connection status
 */
bool l7vs::snmpbridge::get_connectionstate(){
	return connection_state;
}

/*!
 * Get log level
 */
l7vs::LOG_LEVEL_TAG l7vs::snmpbridge::get_loglevel( const l7vs::LOG_CATEGORY_TAG snmp_log_category ){
    l7vs::LOG_LEVEL_TAG level = LOG_LV_NONE;

    if( snmp_param.loglevel.end() != snmp_param.loglevel.find( snmp_log_category ) ){
        level = snmp_param.loglevel.find( snmp_log_category )->second;
    }
    return level;
}

/*!
 * Get log level allcategory
 */
void l7vs::snmpbridge::get_loglevel_allcategory( std::map<l7vs::LOG_CATEGORY_TAG, l7vs::LOG_LEVEL_TAG>&	loglevelmap ){
	for( std::map<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>::iterator it = snmp_param.loglevel.begin();
		it != snmp_param.loglevel.end();
		++it ){
		loglevelmap.insert( std::pair<l7vs::LOG_CATEGORY_TAG, l7vs::LOG_LEVEL_TAG>( it->first, it->second ) );
	}
}

/*!
 * snmpagent socket callback
 */
#if 0
int l7vs_snmpbridge_callback( l7vs_iomux* iom ){
    int len = 0;
    char buff[READBUF_SIZE];
    struct l7ag_message_header* message_header = NULL;
    struct l7ag_payload_header* payload_header = NULL;

    if( !iom ) return -1;

    switch( iom->status ){
        case iomux_snmp_receive_ready:
            len = recv( iom->fd, buff, READBUF_SIZE -1 , 0 );
            if ( len < sizeof( l7ag_message_header ) ) return -2;
            message_header = (struct l7ag_message_header*)buff;
            if ( message_header->version != 1 ) return -3;
            if ( message_header->magic[0] != 0x4d || message_header->magic[1] != 0x47 ) return -4;
            for ( unsigned long long i = 0; i < message_header->payload_count; ++i ) {
                // TODO bug if payload_count = 2...
                payload_header = (struct l7ag_payload_header*) (message_header + 1);
                if (payload_header->magic[0] != 0x50 || payload_header->magic[1] != 0x59) {
                    l7vs_iomux_mod( l7vs_snmp_iomux, iom_read );
                    continue;
                }
                struct l7ag_mibrequest_message* payload = (struct l7ag_mibrequest_message*) (payload_header + 1);
                switch (payload_header->message_id) {
                case MESSAGE_ID_MIBCOLLECTREQUEST:
                    if (payload->magic[0] != 0x52 || payload->magic[1] != 0x51) {
                        l7vs_iomux_mod( l7vs_snmp_iomux, iom_read );
                        return -5;
                    }
                    l7vs_snmpbridge_send_mibcollection(payload);
                    break;
                default:
                    l7vs_iomux_mod( l7vs_snmp_iomux, iom_read );
                    break;
                }
            }
            break;

        case iomux_snmp_send_ready:
            len = send( iom->fd, send_buffer, send_buffer_size, 0 );
            l7vs_iomux_mod( l7vs_snmp_iomux, iom_read );
            iom->status = iomux_snmp_receive_ready;
            break;
        default:
            //error!
            return -1;
    }
    return 0;
}
#endif

/*!
 *
 */
void l7vs::snmpbridge::handle_accept(const boost::system::error_code& error){
	std::cout << "handle_accept" << std::endl;//debug
	connection_state = true;
	snmp_socket.async_receive( boost::asio::buffer( recv_buffer, READBUF_SIZE ), 
								boost::bind(&snmpbridge::handle_receive,
								this,
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred ) );
}

/*!
 *
 */
void l7vs::snmpbridge::handle_receive(const boost::system::error_code& error, size_t bytes_transferred){
#if 0
    size_t len = 0;
    struct l7ag_message_header* message_header = NULL;
    struct l7ag_payload_header* payload_header = NULL;

	len = bytes_transferred;//
	if ( len < sizeof( l7ag_message_header ) ) return;
	message_header = (struct l7ag_message_header*)recv_buffer;
	if ( message_header->version != 1 ) return;
	if ( message_header->magic[0] != 0x4d || message_header->magic[1] != 0x47 ) return;
	for ( unsigned long long i = 0; i < message_header->payload_count; ++i ) {
		// TODO bug if payload_count = 2...
		payload_header = (struct l7ag_payload_header*) (message_header + 1);
		if (payload_header->magic[0] != 0x50 || payload_header->magic[1] != 0x59) {
			continue;
		}
		struct l7ag_mibrequest_message* payload = (struct l7ag_mibrequest_message*) (payload_header + 1);
		switch (payload_header->message_id) {
		case MESSAGE_ID_MIBCOLLECTREQUEST:
			if (payload->magic[0] != 0x52 || payload->magic[1] != 0x51) {
				return;
			}
			l7vs::snmpbridge::send_mibcollection(payload);
			break;
		}
	}
#endif
	snmp_socket.async_receive( boost::asio::buffer( recv_buffer, READBUF_SIZE ), 
								boost::bind(&snmpbridge::handle_receive,
								this,
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred ) );
}

/*!
 *
 */
void l7vs::snmpbridge::handle_send(const boost::system::error_code& error, size_t bytes_transferred){
	snmp_socket.async_receive( boost::asio::buffer( recv_buffer, READBUF_SIZE ), 
								boost::bind(&snmpbridge::handle_receive,
								this,
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred ) );
}

/*!
 *
 */
int l7vs::snmpbridge::send_message(){
    if (send_buffer == NULL || send_buffer_size == 0)
        return -1;

#if 1
	snmp_socket.async_send( boost::asio::buffer( send_buffer, send_buffer_size ), 
								boost::bind(&snmpbridge::handle_send,
								this,
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred ) );
#endif
    return 0;
}

/*!
 *
 */
int l7vs::snmpbridge::load_config(){
#if 1
//    int retval = 0;
	l7vs::Parameter param;
	l7vs::error_code err;
	int tmp_int;

    // load port no from cf
	tmp_int = param.get_int( l7vs::PARAM_COMP_SNMPAGENT, "port", err );
	if( !err ){
		snmp_param.portno = tmp_int;
	}
	else{
		snmp_param.portno = PORT_DEFAULT;
	}
	err.setter(false, "");//clear
    // load interval from cf
	tmp_int = param.get_int( l7vs::PARAM_COMP_SNMPAGENT, "interval", err );
	if( !err ){
		snmp_param.interval = tmp_int;
	}
	else{
		snmp_param.interval = INTERVAL_DEFAULT;
	}
#endif
    return 0;
}

void l7vs::snmpbridge::load_loglevel(){
    //log level 取得
	l7vs::Parameter param;
	l7vs::error_code err;
	std::string tmp_str;

#if 1
    snmp_param.loglevel.clear();
    //
	tmp_str = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_start_stop", err );
	if( !err ){
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_START_STOP, levelstring_map[tmp_str] ) );
	}
	else{
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_INFO) );
	}
	err.setter(false, "");//clear
    //
	tmp_str = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_manager_receive", err );
	if( !err ){
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE, levelstring_map[tmp_str] ) );
	}
	else{
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE, LOG_LV_INFO) );
	}
	err.setter(false, "");//clear
    //
	tmp_str = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_manager_send", err );
	if( !err ){
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_MANAGER_SEND, levelstring_map[tmp_str] ) );
	}
	else{
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_MANAGER_SEND, LOG_LV_INFO) );
	}
	err.setter(false, "");//clear
    //
	tmp_str = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_l7vsd_receive", err );
	if( !err ){
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE, levelstring_map[tmp_str] ) );
	}
	else{
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE, LOG_LV_INFO) );
	}
	err.setter(false, "");//clear
    //
	tmp_str = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_l7vsd_send", err );
	if( !err ){
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_L7VSD_SEND, levelstring_map[tmp_str] ) );
	}
	else{
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_L7VSD_SEND, LOG_LV_INFO) );
	}
	err.setter(false, "");//clear
    //
	tmp_str = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_logger", err );
	if( !err ){
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_LOGGER, levelstring_map[tmp_str] ) );
	}
	else{
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_LOGGER, LOG_LV_INFO) );
	}
	err.setter(false, "");//clear
    //
	tmp_str = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_parameter", err );
	if( !err ){
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_PARAMETER, levelstring_map[tmp_str] ) );
	}
	else{
        snmp_param.loglevel.insert( std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( LOG_CAT_SNMPAGENT_PARAMETER, LOG_LV_INFO) );
	}
#endif
}
