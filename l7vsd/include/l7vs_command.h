/*!
 *    @file    l7vscommand.h
 *    @brief    l7vsadm and l7vsd connection data prototype
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

#ifndef    L7VS_COMMAND_H
#define    L7VS_COMMAND_H

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/asio.hpp>
#include <list>
#include <memory>
#include "logger_enum.h"
#include "parameter_enum.h"
#include "replication.h"
#include "virtualservice_element.h"
#include "snmp_info.h"

namespace l7vs
{
//
//! @class l7vsadm_request
//! @brief    l7vsadm -> l7vsd request data class
class    l7vsadm_request
{
public:
        //! @enum COMMAND_CODE_TAG    request command code enum
        enum    COMMAND_CODE_TAG {
                CMD_NONE = 0,
                CMD_LIST,                //!< List command(-l,--list)
                CMD_LIST_VERBOSE,        //!< Verbose list command(-V,--verbose)
                CMD_LIST_KEY,            //!< Key list command(-K, --key)
                CMD_ADD_VS,                //!< Add VirtualService command(-A,--add-service)
                CMD_DEL_VS,                //!< Delete VirtualService command(-D, --delete-service)
                CMD_EDIT_VS,            //!< Edit VirtualService command(-E, --edit-service)
                CMD_FLUSH_VS,            //!< Flush VirtualService command(-C, --flush)
                CMD_ADD_RS,                //!< Add RealServer command(-a, --add-server)
                CMD_DEL_RS,                //!< Delete RealServer command(-d, --delete-server)
                CMD_EDIT_RS,            //!< Edit RealServer command(-e.--edit-server)
                CMD_REPLICATION,        //!< Replication command(-R, --replication)
                CMD_LOG,                //!< Logger command(-L, -log)
                CMD_SNMP,                //!< SNMPAgent command(-S, --snmp)
                CMD_PARAMETER,            //!< Parameter command(-P, --parameter)
                CMD_HELP,                //!< Help command(-h, --help)
        };

        //! @enum REPLICATION_COMMAND_TAG    replication request command enum
        enum    REPLICATION_COMMAND_TAG {
                REP_NONE = 0,
                REP_START,                //!< REPLICATION START COMMAND
                REP_STOP,                //!< REPLICATION STOP COMMAND
                REP_FORCE,                //!< REPLICATION FORCE COMMAND
                REP_DUMP                //!< REPLICATION DUMP COMMAND
        };

        COMMAND_CODE_TAG            command;                //!< request command
        virtualservice_element        vs_element;                //!< use VS mode and RealServer mode
        REPLICATION_COMMAND_TAG        replication_command;    //!< use replication command mode
        LOG_CATEGORY_TAG            log_category;            //!< use log change mode. target log category
        LOG_LEVEL_TAG                log_level;                //!< use log level change mode target category log level
        PARAMETER_COMPONENT_TAG        reload_param;            //!< set reload param mode
        snmp_info            snmpinfo;
        //! constructor
        l7vsadm_request() :            command(CMD_NONE),
                replication_command(REP_NONE),
                log_category(LOG_CAT_NONE),
                log_level(LOG_LV_NONE),
                reload_param(PARAM_COMP_NOCAT) {}

        template <typename Elem, typename Traits>
        friend std::basic_ostream<Elem, Traits>& operator<<(
                std::basic_ostream<Elem, Traits>& os,
                const l7vsadm_request &request) {

                os << "l7vsadm_request={";
                os << boost::format("command=%d: "
                                    "vs_element=%s: "
                                    "replication_command=%d: "
                                    "log_category=%d: "
                                    "log_level=%d: "
                                    "reload_param=%d: "
                                    "snmpinfo=%s: ")
                   % request.command
                   % request.vs_element
                   % request.replication_command
                   % request.log_category
                   % request.log_level
                   % request.reload_param
                   % request.snmpinfo;
                return os;
        }

private:
        friend class    boost::serialization::access;        //!< serializable access class is friend.
        //! serializable function
        //! @param[in]    archiver class from boost serializable
        //! @param[in]    version use boost serializable
        template <class Archive > void serialize(Archive &ar, const unsigned int version) {
                ar &command;
                ar &vs_element;
                ar &replication_command;
                ar &log_category;
                ar &log_level;
                ar &reload_param;
                ar &snmpinfo;
        }
};

//
//! @class    l7vsd_response
//! @brief    l7vsd -> l7vsadm data class
class    l7vsd_response
{
public:
        enum    COMMAND_RESPONSE_CODE {   //!<    response command code enum
                RESPONSE_NONE = 0,            //!<    none
                RESPONSE_OK,                //!<    request execute ok
                RESPONSE_ERROR,                //!<    request execute error
                RESPONSE_LIST_ERROR,        //!<    list request error
                RESPONSE_LIST_VERBOSE_ERROR,//!<    list verbose request error
                RESPONSE_LIST_KEY_ERROR,    //!<    list key request error
                RESPONSE_ADD_VS_ERROR,        //!<    virtual service add error
                RESPONSE_DEL_VS_ERROR,        //!<    virtual service delete error
                RESPONSE_EDIT_VS_ERROR,        //!<    virtual service edit error
                RESPONSE_FLUSH_VS_ERROR,    //!<    virtual service clear error
                RESPONSE_ADD_RS_ERROR,        //!<    realserver add error
                RESPONSE_DEL_RS_ERROR,        //!<    realserver delete error
                RESPONSE_EDIT_RS_ERROR,        //!<    realserver edit error
                RESPONSE_REPLICATION_ERROR,    //!<    replication error
                RESPONSE_LOG_ERROR,            //!<    logger error
                RESPONSE_SNMP_ERROR,        //!<    snmpagent error
                RESPONSE_PARAMETER_ERROR    //!<    parameter error
        };

        typedef    std::pair<LOG_CATEGORY_TAG, LOG_LEVEL_TAG> log_category_level_type;

        l7vsadm_request::COMMAND_CODE_TAG
        code;    //!<    request command.

        COMMAND_RESPONSE_CODE    status;    //!<    return status.

        std::string                message;//!<    error message

        std::list< virtualservice_element >
        virtualservice_status_list;//!< virtual service lists

        replication::REPLICATION_MODE_TAG    replication_mode_status;//!< replication status.

        std::list< log_category_level_type >
        log_status_list;    //!< log categories statuses.

        //bool                    snmp_connection_status;    //!< snmp connection status

        snmp_info   snmpinfo;

        unsigned long long        total_bps;                    //!< l7vsd's total bit par sec
        unsigned long long        total_client_recv_byte;        //!< l7vsd's total client receive bytes
        unsigned long long        total_client_send_byte;        //!< l7vsd's total client send bytes
        unsigned long long        total_realserver_recv_byte;    //!< l7vsd's total realserver receive bytes
        unsigned long long        total_realserver_send_byte;    //!< l7vsd's total realserver send bytes
        std::vector<virtualservice_element>
        virtualservice_vec;            //!< virtualservice lists
        //! constructor
        l7vsd_response() :        code(l7vsadm_request::CMD_NONE),
                status(RESPONSE_NONE),
                message(""),
                replication_mode_status(replication::REPLICATION_OUT),
                total_bps(0ULL),
                total_client_recv_byte(0ULL),
                total_client_send_byte(0ULL),
                total_realserver_recv_byte(0ULL),
                total_realserver_send_byte(0ULL) {}

        template <typename Elem, typename Traits>
        friend std::basic_ostream<Elem, Traits>& operator<<(
                std::basic_ostream<Elem, Traits>& os,
                const l7vsd_response &response) {

                os << "l7vsd_response={";
                os << boost::format("code=%d: "
                                    "status=%d: "
                                    "message=%s: ")
                   % response.code
                   % response.status
                   % response.message;
                {
                        unsigned int i = 0;
                        BOOST_FOREACH(virtualservice_element vs_elem, response.virtualservice_status_list) {
                                os << boost::format("virtualservice_status_list[%d]=") % i;
                                os << vs_elem;
                                os << ": ";
                                ++i;
                        }
                }
                os << boost::format("replication_mode_status=%d: ")
                   % response.replication_mode_status;
                {
                        unsigned int i = 0;
                        BOOST_FOREACH(log_category_level_type log_pair, response.log_status_list) {
                                os << boost::format("log_status_list[%d]={log_category=%d, log_level=%d}")
                                   % i
                                   % log_pair.first
                                   % log_pair.second;
                                os << ": ";
                                ++i;
                        }
                }
                os << "snmpinfo=" << response.snmpinfo;

                os << boost::format("total_bps=%d: "
                                    "total_client_recv_byte=%d: "
                                    "total_client_send_byte=%d: "
                                    "total_realserver_recv_byte=%d: "
                                    "total_realserver_send_byte=%d: ")
                   % response.total_bps
                   % response.total_client_recv_byte
                   % response.total_client_send_byte
                   % response.total_realserver_recv_byte
                   % response.total_realserver_send_byte;
                {
                        unsigned int i = 0;
                        BOOST_FOREACH(virtualservice_element vs_elem, response.virtualservice_vec) {
                                os << boost::format("virtualservice_vec[%d]=") % i;
                                os << vs_elem;
                                os << ": ";
                                ++i;
                        }
                }

                return os;
        }

private:
        friend class    boost::serialization::access;        //! friend boost serializable class
        //! serializable
        //! @brief using boost serializable. class serializable function.
        //! @param[in]    archive
        //! @param[in]    version
        template <class Archive > void serialize(Archive &ar, const unsigned int version) {
                ar &code;
                ar &status;
                ar &message;
                ar &virtualservice_status_list;
                ar &replication_mode_status;
                ar &log_status_list;
                ar &snmpinfo;
                ar &total_bps;
                ar &total_client_recv_byte;
                ar &total_client_send_byte;
                ar &total_realserver_recv_byte;
                ar &total_realserver_send_byte;
                ar &virtualservice_vec;
        }
};

}    // namespace l7vsd
#endif    //L7COMMAND_H
