/*
 *    @file    module_base.h
 *    @brief    shared object module abstract class
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
#ifndef    MODULE_BASE_H
#define    MODULE_BASE_H

#include <string>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#include "logger.h"

namespace l7vs{

//! @class    module_base
//! @brief    protocol module and schedule module base class
//! @brief    all module class is extened this class.
class module_base{
public:
    //! loglevel get function object type
    typedef    boost::function< LOG_LEVEL_TAG(void) >
                                getloglevel_func_type;
    //! log output function object type
    typedef    boost::function< void ( const unsigned int, const std::string&, const char*, int ) >
                                logger_func_type;
    //! replication payment memory function object type
    typedef    boost::function< void* ( const std::string&, unsigned int* ) >
                                replicationpaymemory_func_type;
protected:
    std::string                        name;            //!< module name string
    getloglevel_func_type            getloglevel;    //!< get loglevel function object
    logger_func_type                putLogFatal;    //!< fatal log output function object
    logger_func_type                putLogError;    //!< error log output function object
    logger_func_type                putLogWarn;        //!< warn log output function object
    logger_func_type                putLogInfo;        //!< info log output function object
    logger_func_type                putLogDebug;    //!< debug log output function object

    //! replication memory peyment method
    replicationpaymemory_func_type    replication_pay_memory;

    //! replication area lock function object
    boost::function< void( void ) >    replication_area_lock;
    //! replication area unlock function object
    boost::function< void( void ) >    replication_area_unlock;

    //! virtual service endpoint tcp
    boost::asio::ip::tcp::endpoint virtual_service_endpoint_tcp;
    //! virtual service endpoint udp
    boost::asio::ip::udp::endpoint virtual_service_endpoint_udp;

public:
    //! constractor
    module_base( std::string in_modulename ){
        name = in_modulename;
    }
    //! destractor
    virtual ~module_base(){}
    //! tcp protocol support check
    //! @return tcp support is true
    //! @return tcp not-support is false
    virtual    bool    is_tcp() = 0;
    //! udp protocol support check
    //! @return udp support is true
    //! @return udp not-support is false
    virtual    bool    is_udp() = 0;
    //! module name getter
    //! @return module name
    const std::string&    get_name() { return name; }

    //! logger function setter
    //! @param[in]    loglevel get function object
    //! @param[in]    fatal log output function object
    //! @param[in]    error log output function object
    //!    @param[in]    warn log output function object
    //!    @param[in]    info log output function object
    //! @param[in]    debug log output function object
    void    init_logger_functions(
                            getloglevel_func_type    ingetloglevel,
                            logger_func_type        inputLogFatal,
                            logger_func_type        inputLogError,
                            logger_func_type        inputLogWarn,
                            logger_func_type        inputLogInfo,
                            logger_func_type        inputLogDebug ){
        getloglevel = ingetloglevel;
        putLogFatal = inputLogFatal;
        putLogError = inputLogError;
        putLogWarn    = inputLogWarn;
        putLogInfo    = inputLogInfo;
        putLogDebug = inputLogDebug;
    }

    //! replication function object setter.
    //! @param[in]    replication pay memory function object
    //! @param[in]    replication lock function object
    //! @param[in]    replication unlock undontion object
    //! @param[in]    virtual service endpoint tcp
    //! @param[in]    virtual service endpoint udp
    void    init_replication_functions(
                            replicationpaymemory_func_type  inreplication_pay_memory,
                            boost::function< void( void ) > inlock_func,
                            boost::function< void( void ) > inunlock_func,
                            const boost::asio::ip::tcp::endpoint& invirtual_service_endpoint_tcp,
                            const boost::asio::ip::udp::endpoint& invirtual_service_endpoint_udp ){
        replication_pay_memory = inreplication_pay_memory;
        replication_area_lock = inlock_func;
        replication_area_unlock = inunlock_func;
        virtual_service_endpoint_tcp = invirtual_service_endpoint_tcp;
        virtual_service_endpoint_udp = invirtual_service_endpoint_udp;
    }

    // For replication interface
    // when call this function, write data to replication area.
    // Caution: Not need loop inside.
    //      Because Loop and Timer control is in virtual_service.
    //      virtual_service call replication_interrupt in specified time at once.
    //      when called, replication_interrupt write data at once.
    //! replication interval interrrupt
    //! timer thread call this function. from virtualservice.
    virtual    void    replication_interrupt() = 0;
};

}    //namespace l7vsd

#endif //MODULE_BASE_H
