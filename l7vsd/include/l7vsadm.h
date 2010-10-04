/*!
 * @file  l7vsadm.h
 * @brief l7vsd control application.
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

#ifndef L7VSADM_H
#define L7VSADM_H
#include <map>
#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include "error_code.h"
#include "l7vs_command.h"
#include "tcp_socket_option.h"

#ifndef L7VS_MODULE_PATH
#define L7VS_MODULE_PATH    "./"
#endif

#ifndef COMMAND_BUFFER_SIZE
#define COMMAND_BUFFER_SIZE (65535)
#endif

#ifndef L7VS_CONFIG_SOCK_PATH
#define L7VS_CONFIG_SOCK_PATH   "/var/run/l7vs"
#endif
#define L7VS_CONFIG_SOCKNAME    L7VS_CONFIG_SOCK_PATH   "/l7vs"

#define L7VSADM_DEFAULT_SCHEDULER       "rr"        //!< Default scheduler name
#define L7VSADM_DEFAULT_WAIT_INTERVAL   (1)         //!< Default wait interval
#define L7VSADM_DEFAULT_WAIT_COUNT      (10)        //!< Default wait count
#define L7VSADM_MAX_WAIT                (60)        //!< Max wait value
#define L7VS_MODNAME_LEN                (16)        //!< Module name length
#define L7VS_FILENAME_LEN               (256)       //!< File name length

namespace l7vs
{

//! endpoint string parse function
//! @param[in]  endpoint string
//! @param[out] error_code
//! @return     endpoint
template <class T>
typename T::endpoint string_to_endpoint(std::string &str, error_code &err)
{
        std::string::size_type pos = str.rfind(":");
        std::string hostname = str.substr(0, pos);
        std::string portname = str.substr(pos + 1, str.length());
        if (0 == hostname.length()) {
                err.setter(1, "hostname is not specified:");
                return typename T::endpoint();
        }
        boost::regex re("\\d+");
        if (boost::regex_match(portname.c_str(), re)) {
                try {
                        boost::lexical_cast<unsigned short>(portname);
                } catch (boost::bad_lexical_cast &) {
                        err.setter(1, "invalid port number:");
                        return typename T::endpoint();
                }
        }
        //remove "[","]"
        boost::algorithm::erase_first(hostname, "[");
        boost::algorithm::erase_last(hostname, "]");

        boost::asio::io_service        io_service;
        typename T::resolver           resolver(io_service);
        typename T::resolver::query    query(hostname, portname);
        typename T::resolver::iterator end;
        boost::system::error_code      ec;
        typename T::resolver::iterator itr = resolver.resolve(query, ec);
        if (ec) {
                std::stringstream buf;
                buf << "invalid endpoint:" << ec.message() << ":";
                err.setter(1, buf.str());
                return typename T::endpoint();
        }
        if (itr == end) {
                return typename T::endpoint();
        }
        return *itr;
}

//! check endpoint function
//! @param[in]  endpoint
//! @param[in]  allow any_address or not
//! @param[out] error_code
template <class T>
void check_endpoint(typename T::endpoint &ep, bool allow_any_address, error_code &err)
{
        if (!allow_any_address) {
                if ((ep.address().is_v4() &&
                     ep.address().to_v4() == boost::asio::ip::address_v4::any()) ||
                    (ep.address().is_v6() &&
                     ep.address().to_v6() == boost::asio::ip::address_v6::any())) {
                        err.setter(1, "invalid address (INADDR_ANY):");
                        return;
                }
        }
        if (ep.port() == 0) {
                err.setter(1, "invalid port number (0):");
                return;
        }
}

//! endpoint to string function
//! @param[in] endpoint
//! @param[in] return numeric expression or not
//! @return    endpoint string
template <class T>
std::string endpoint_to_string(typename T::endpoint ep, bool numeric_flag)
{
        std::stringstream buf;
        if (!numeric_flag) {
                boost::asio::io_service        io_service;
                typename T::resolver           resolver(io_service);
                boost::system::error_code      ec;
                typename T::resolver::iterator itr = resolver.resolve(ep, ec);
                if (!ec) {
                        if (itr->host_name() == "::") {
                                buf << "[::]:" << itr->service_name();
                        } else {
                                buf << itr->host_name() << ":" << itr->service_name();
                        }
                        return buf.str();
                }
        }
        if (ep.address().is_v6()) {
                buf << "[" << ep.address().to_string() << "]:" << ep.port();
        } else {
                buf << ep.address().to_string() << ":" << ep.port();
        }
        return buf.str();
}

class l7vsadm
{
protected:
        //! command parse function object.type.
        typedef boost::function<bool (int, char*[])> parse_cmd_func_type;
        //! command string - parse function object map type.
        typedef std::map<std::string, parse_cmd_func_type> parse_cmd_map_type;
        //! option parse function object type.
        typedef boost::function<bool (int &, int, char*[])> parse_opt_func_type;
        //! option string - parse function object map type
        typedef std::map<std::string, parse_opt_func_type> parse_opt_map_type;
        //! log category string -> log category enum convert map type.
        typedef std::map<std::string, LOG_CATEGORY_TAG> string_logcategory_map_type;
        //! log category enum -> log category string convert map type.
        typedef std::map<LOG_CATEGORY_TAG, std::string> logcategory_string_map_type;
        //! log level string -> log level enum convert map type
        typedef std::map<std::string, LOG_LEVEL_TAG> string_loglevel_map_type;
        //! log level enum convert map -> log level string type
        typedef std::map<LOG_LEVEL_TAG, std::string> loglevel_string_map_type;
        //! parameter category string -> parameter category enum convert map type
        typedef std::map<std::string, PARAMETER_COMPONENT_TAG> string_parameter_map_type;
        //! COMMAND_RESPONSE_CODE -> message convert map type
        typedef std::map<l7vsd_response::COMMAND_RESPONSE_CODE, std::string> response_error_message_map_type;
        //! disp result function object type.
        typedef boost::function<void ()> disp_result_func_type;
        //! command - disp result function object map type
        typedef std::map<l7vsadm_request::COMMAND_CODE_TAG, disp_result_func_type> disp_result_map_type;
        //! replication mode enum -> replication mode string convert map type
        typedef std::map<replication::REPLICATION_MODE_TAG, std::string> replication_mode_string_map_type;

        //! @class file_lock
        //! @brief l7vsadm execute file lock class
        class file_lock
        {
        protected:
                int fd;
                int lock;
        public:
                //! constructor
                file_lock(const std::string &path, error_code &err)
                        :
                        fd(-1),
                        lock(-1)
                {
                        fd = open(path.c_str(), O_RDONLY);
                        if (fd == -1) {
                                std::stringstream buf;
                                buf << boost::format("L7vsadm execute file open error. file:%s") % path;
                                err.setter(true, buf.str());
                        }
                }

                //! destructor
                ~file_lock()
                {
                        if (lock != -1) {
                                // fd unlock.
                                flock(fd, LOCK_UN);
                        }
                        if (fd != -1) {
                                // fd close.
                                close(fd);
                        }
                }

                //! try lock function
                //! @return    lock succeed(true) / lock failed(false)
                bool try_lock()
                {
                        lock = flock(fd, LOCK_EX | LOCK_NB);
                        if (lock == 0) {
                                // l7vsadm file lock OK.
                                return true;
                        }
                        return false;
                }
        };

        //
        // command parse functions.
        //
        //! list command parse function
        bool parse_list_func(l7vsadm_request::COMMAND_CODE_TAG, int, char*[]);
        //! virtualservice command parse function
        bool parse_vs_func(l7vsadm_request::COMMAND_CODE_TAG, int, char*[]);
        //! realserver_command parse function
        bool parse_rs_func(l7vsadm_request::COMMAND_CODE_TAG, int, char*[]);
        //! replication command parse function
        bool parse_replication_func(l7vsadm_request::COMMAND_CODE_TAG, int, char*[]);
        //! log command parse function
        bool parse_log_func(l7vsadm_request::COMMAND_CODE_TAG, int, char*[]);
        //! snmpagent command parse function
        bool parse_snmp_func(l7vsadm_request::COMMAND_CODE_TAG, int, char*[]);
        //! parameter parse function
        bool parse_parameter_func(l7vsadm_request::COMMAND_CODE_TAG, int, char*[]);
        //! help command parse func
        bool parse_help_func(l7vsadm_request::COMMAND_CODE_TAG, int, char*[]);

        //
        // option parse functions.
        //
        // list option functions.
        //! list numeric flag check.
        bool parse_opt_list_numeric_func(int &, int, char*[]);
        //
        // virtualservice option functions.
        //! target option check
        bool parse_opt_vs_target_func(int &, int, char*[]);
        //! module option check
        bool parse_opt_vs_module_func(int &, int, char*[]);
        //! scheduler option check.
        bool parse_opt_vs_scheduler_func(int &, int, char*[]);
        //! upper flag check
        bool parse_opt_vs_upper_func(int &, int, char*[]);
        //! bypass(SorryServer) option check
        bool parse_opt_vs_bypass_func(int &, int, char*[]);
        //! virtualservice option flag function
        bool parse_opt_vs_flag_func(int &, int, char*[]);
        //! virtualservice option sorry forward mode function
        bool parse_opt_vs_fwdmode_func(int &, int, char*[]);
        //! virtualservice option qosupstream function
        bool parse_opt_vs_qosup_func(int &, int, char*[]);
        //! virtualservice option qosdownstream function
        bool parse_opt_vs_qosdown_func(int &, int, char*[]);
        //! virtualservice option udp func.
        bool parse_opt_vs_udp_func(int &, int, char*[]);
        //! virtualservice option ssl file function
        bool parse_opt_vs_ssl_file_func(int &, int, char*[]);
        //! virtualservice option access log function
        bool parse_opt_vs_access_log_func(int &, int, char*[]);
        //! virtualservice option access log logrotate function
        bool parse_opt_vs_access_log_logrotate_func(int &, int, char*[]);
        //! virtualservice option socket option function
        bool parse_opt_vs_socket_func(int &, int, char*[]);
        // realserver option function
        //! realserver weight set
        bool parse_opt_rs_weight_func(int &, int, char*[]);
        //! realserver target set
        bool parse_opt_rs_realserver_func(int &, int, char*[]);
        //! realserver forward mode set
        bool parse_opt_rs_fwdmode_func(int &, int, char*[]);
        // replication option function
        //! replication switch function
        bool parse_opt_replication_switch_func(int &, int, char*[]);
        //! replication start function
        bool parse_opt_replication_start_func(int &, int, char*[]);
        //! replication stop function
        bool parse_opt_replication_stop_func(int &, int, char*[]);
        //! replication force function
        bool parse_opt_replication_force_func(int &, int, char*[]);
        //! replication dump function
        bool parse_opt_replication_dump_func(int &, int, char*[]);
        // log option function
        //! log category set function
        bool parse_opt_log_category_func(int &, int, char*[]);
        //! log level set function
        bool parse_opt_log_level_func(int &, int, char*[]);
        // snmp option function
        //! snmp refresh set function
        bool parse_opt_snmp_refresh_func(int &, int, char*[]);
        //! snmp parse_opt_snmp_vs_target_func
        bool parse_opt_snmp_vs_target_func(int &, int, char*[]);
        bool parse_opt_snmp_vs_module_func(int &, int, char*[]);
        bool parse_opt_snmp_flag_func(int &, int, char*[]);
        bool parse_opt_snmp_interval_func(int &, int, char*[]);
        bool parse_opt_snmp_log_trap_func(int &, int, char*[]);
        bool parse_opt_snmp_log_trap_level_func(int &, int, char*[]);
        // parameter option function
        //! parameter reload component parsing
        bool parse_opt_parameter_reload_func(int &, int, char*[]);

        //! disp_list function
        void disp_list();
        //! disp_list_key function
        void disp_list_key();
        //! disp_list_verbose function
        void disp_list_verbose();

        //! Get l7vsadm parameter data
        void set_parameter();

        //! argument dump for debug
        std::string argument_debug_dump(int, char*[]);

        //! command function map dictionary.
        parse_cmd_map_type command_dic;

        //! list option function map dictionary.
        parse_opt_map_type list_option_dic;
        //! virtualservice option function map dictionary
        parse_opt_map_type vs_option_dic;
        //! realserver option function map dictionary
        parse_opt_map_type rs_option_dic;
        //! replication option function map dictionary
        parse_opt_map_type replication_option_dic;
        //! replication switch option function map dictionary
        parse_opt_map_type replication_switch_option_dic;
        //! log option function map dictionary
        parse_opt_map_type log_option_dic;
        //! snmp option_function map dictionary
        parse_opt_map_type snmp_option_dic;
        parse_opt_map_type snmp_vs_option_dic;
        //! parameter option function map dictionary
        parse_opt_map_type parameter_option_dic;

        //! log category string to log category enum dictionary.
        string_logcategory_map_type string_logcategory_dic;

        //! log category enum to log category string dictionary.
        logcategory_string_map_type logcategory_string_dic;

        //! log level string to log level enum dictionary
        string_loglevel_map_type string_loglevel_dic;

        //! log level enum to log level string dictionary
        loglevel_string_map_type loglevel_string_dic;

        string_parameter_map_type string_parameter_dic;
        response_error_message_map_type response_error_message_dic;

        //! disp result function map dictionary.
        disp_result_map_type disp_result_dic;

        //! replication mode enum to replication mode string dictionary
        replication_mode_string_map_type replication_mode_string_dic;

        //! usage function
        std::string usage();

        // l7vsd request data
        l7vsadm_request request; //!< send_request
        l7vsd_response response; //!< recv_response

        // l7vsadm using data.
        bool numeric_flag; //!< numeric flag
        boost::asio::io_service io_service; //!< io_service

        error_code l7vsadm_err;

        //! Interval of l7vsadm command conflict check.
        int command_wait_interval;
        //! Number of times of l7vsadm command conflict check.
        int command_wait_count;

public:
        //! constructor
        l7vsadm();

        //! execute function
        bool execute(int, char*[]);
};

} //namespace l7vs

#endif //L7VSADM_H
