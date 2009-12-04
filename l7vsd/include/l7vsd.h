/*!
 *    @file    l7vsd.h
 *    @brief    l7vsd main class
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

#ifndef    L7VSD_H
#define L7VSD_H

#include <string>
#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "error_code.h"
#include "l7vs_command.h"
#include "command_receiver.h"
#include "virtualservice_element.h"
#include "virtualservice.h"
#include "realserver_element.h"
#include "realserver.h"
#include "replication.h"
#include "snmpbridge.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"
#include "logger.h"
#include "parameter.h"

#ifndef    L7VS_MODULE_PATH
    #define L7VS_MODULE_PATH        "."
#endif    //L7VS_MODULE_PATH

#ifndef    L7VS_CONFIG_SOCK_PATH
    #define L7VS_CONFIG_SOCK_PATH    "/var/run/l7vs"
#endif
#define L7VS_CONFIG_SOCKNAME        L7VS_CONFIG_SOCK_PATH "/l7vs"

namespace l7vs{
class virtual_service;
class command_receiver;
class replication;
class snmpbridge;
class protocol_module_control;
class schedule_module_control;

//! l7vsd main class
class l7vsd : private boost::noncopyable{
public:
    typedef boost::shared_ptr< command_receiver >    command_receiver_ptr;    //!< shared_ptr command_receiver typedef
    typedef boost::shared_ptr< replication >        replication_ptr;        //!< shared_ptr replication typedef
    typedef boost::shared_ptr< snmpbridge >            snmpbridge_ptr;            //!< shared_ptr snmp_bridge typedef
    typedef    boost::shared_ptr< virtual_service >    virtualservice_ptr;        //!< shared_ptr    virtualservice typedef

    typedef std::list< virtualservice_ptr >            vslist_type;            //!< virtual service list typedef
    typedef std::list< virtualservice_element >        vselist_type;            //!< virtual service element list typedef

    typedef    std::list< std::pair<LOG_CATEGORY_TAG, LOG_LEVEL_TAG> >    logstatus_list_type;    //!< logstatus list typedef

    l7vsd();                //!< constructor
    virtual    ~l7vsd();        //!< destructor

protected:
    boost::thread_group            vs_threads;            //!< virtual_service thread group
    boost::asio::io_service        dispatcher;            //!< dispatcher

    mutable    vslist_type            vslist;                //!< virtual_service list

    command_receiver_ptr        receiver;            //!< command_receiver ptr
    replication_ptr                rep;                //!< replication ptr
    snmpbridge_ptr                bridge;                //!< snmp_bridge ptr

    boost::mutex                command_mutex;        //!< command execute mutex
    boost::mutex                vslist_mutex;        //!< virtual service list mutex

    virtual    vslist_type::iterator
                                search_vslist( const virtualservice_element& )    const;    //!< vs_list search function

    bool                        help;                //!< help mode

    boost::posix_time::ptime    starttime;            //!< l7vsd startup time

    volatile    sig_atomic_t    exit_requested;        //!< signal exit flag
    volatile    sig_atomic_t    received_sig;        //!< received signal

    void                        sig_exit_handler();    //!< signal handler

    //! option parse function object type.
    typedef    boost::function< bool ( int&, int, char*[] ) >
            parse_opt_func_type;
    //! option string - parse function object map type
    typedef    std::map< std::string, parse_opt_func_type >
            parse_opt_map_type;
    //! list option function map dictionary.
    parse_opt_map_type    option_dic;

public:
    void    list_virtual_service( vselist_type*, error_code&  );                //!< virtual_service list command
    void    list_virtual_service_verbose( l7vsd_response* response, error_code&  );        //!< virtual_service verbose list command
    void    add_virtual_service( const virtualservice_element*, error_code& );    //!< virtual_service add command
    void    del_virtual_service( const virtualservice_element*, error_code& );    //!< virtual_service del command
    void    edit_virtual_service( const virtualservice_element*, error_code& );    //!< virtual_service edit command

    void    add_real_server( const virtualservice_element*, error_code& );    //!< real_server add command
    void    del_real_server( const virtualservice_element*, error_code& );    //!< real_server del command
    void    edit_real_server( const virtualservice_element*, error_code& );    //!< real_server edit command

    void    flush_virtual_service( error_code& );    //!< all virtual_service delete command

    void    replication_command( const l7vsadm_request::REPLICATION_COMMAND_TAG*, error_code& );    //!< replication command
    void    set_loglevel( const LOG_CATEGORY_TAG*, const LOG_LEVEL_TAG*, error_code& );        //!< set loglevel command
    void    snmp_set_loglevel( const LOG_CATEGORY_TAG*, const LOG_LEVEL_TAG*, error_code& );    //!< snmp set loglevel command
    void    reload_parameter( const PARAMETER_COMPONENT_TAG*, error_code& );    //!< reload component parameter command

    int        run( int, char*[] );        //!< l7vsd run method

    void    release_virtual_service( const virtualservice_element& )    const;        //!< virtualservice release from vslist

    vslist_type&    get_virtualservice_list();            //!< virtualservice_list getter
    boost::mutex&    get_virtualservice_list_mutex();    //!< virtualservice_list mutex getter

protected:
    bool    check_options( int, char*[] );            //!< check option func
    bool    parse_help( int&, int, char*[] );        //!< parse help func

    std::string    usage();                            //!< make usage message
    
    std::string    argument_debug_dump( int, char*[] );    //!< argument dump for debug

};

}    //namespace l7vs
#endif    //L7VSD_H
