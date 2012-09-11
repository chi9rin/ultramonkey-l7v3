/*!
 * @file  l7vsadm.cpp
 * @brief l7vsadm command is l7vsd control application
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

#include <iostream>
#include <sstream>
#include <exception>
#include <csignal>
#include <boost/bind.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <sys/file.h>
#include "l7vsadm.h"
#include "logger.h"
#include "parameter.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"
#include "virtualservice_element.h"
#include "logger_access_manager.h"

#define VS_CONTACT_CLASS_SSL (0x00000001)

// global function prototype
static void sig_exit_handler(int sig);
static int  set_sighandler(int sig, void (*handler)(int));
static int  set_sighandlers();

// global variables
static bool signal_flag = false;
static int  received_sig = 0;

//
// command functions.
//
//! list command parsing.
//! @param[in] request command
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_list_func(l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 1, "l7vsadm::parse_list_func", __FILE__, __LINE__);

        request.command = cmd; // set command
        if (argc < 3) {
                return true; // option is none. this pattern is true
        } else if (argc > 3) {
                std::stringstream buf;
                buf << "Argument argc is illegal for " << argv[1] << " command.";
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 117, buf.str(), __FILE__, __LINE__);
                return false;
        }

        for (int pos = 2; pos < argc; ++pos) { //search option function from argv strings
                parse_opt_map_type::iterator itr = list_option_dic.find(argv[pos]);
                if (itr != list_option_dic.end()) { // option string function find.
                        if (! itr->second(pos, argc, argv)) {
                                return false; // option string function error.
                        }
                } else { //option string function not found.
                        // print option not found message.
                        std::stringstream buf;
                        buf << "list option not found: " << argv[pos];
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 1, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }
        return true;
}
//
// option list functions.
//
//! list numeric flag check.
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_list_numeric_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 2, "l7vsadm::parse_opt_list_numeric_func", __FILE__, __LINE__);

        numeric_flag = true; //numeric flag on.
        return true;
}

//! virtualservice command parsing.
//! @param[in] request command
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_vs_func(l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 3, "l7vsadm::parse_vs_func", __FILE__, __LINE__);

        request.command = cmd; // set command
        if (l7vsadm_request::CMD_FLUSH_VS == cmd) {
                if (argc != 2) {
                        std::stringstream buf;
                        buf << "Argument argc is illegal for " << argv[1] << " command.";
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 118, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        } else {
                if (argc < 6) {
                        std::stringstream buf;
                        buf << "Argument argc is illegal for " << argv[1] << " command.";
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 119, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }

        std::map<std::string, int> count_map;

        for (parse_opt_map_type::iterator itr = vs_option_dic.begin() ;
             itr != vs_option_dic.end() ; ++itr) {
                count_map[ itr->first ] = 0;
        }

        if (l7vsadm_request::CMD_EDIT_VS == cmd) {
                request.vs_element.access_log_flag = -1;
        }

        for (int pos = 2; pos < argc; ++pos) { // check options.
                parse_opt_map_type::iterator itr = vs_option_dic.find(argv[pos]);
                if (itr != vs_option_dic.end()) { // find option
                        count_map[ itr->first ]++;
                        if (! itr->second(pos, argc, argv)) {
                                return false; // option function execute.
                        }
                } else { // not found option function.
                        std::stringstream buf;
                        buf << "virtualservice option not found: " << argv[pos];
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 2, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }
        // check virtualservice on response
        if (l7vsadm_request::CMD_FLUSH_VS == cmd) {
                // flush vs required no option
                return true;
        }
        if ((l7vsadm_request::CMD_ADD_VS == cmd) && (request.vs_element.schedule_module_name.length() == 0)) {
                //scheduler module not specified
                //scheduler module check.
                std::string scheduler_name = L7VSADM_DEFAULT_SCHEDULER; //default scheduler
                schedule_module_control &ctrl = schedule_module_control::getInstance();
                ctrl.initialize(L7VS_MODULE_PATH);
                schedule_module_base *module;
                try {
                        module = ctrl.load_module(scheduler_name);
                } catch (...) {
                        std::stringstream buf;
                        buf << "scheduler module load error(--scheduler): " << scheduler_name;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 3, buf.str(), __FILE__, __LINE__);
                        return false;
                }
                if (!module) {
                        // schedule module not found
                        std::stringstream buf;
                        buf << "scheduler module not found(--scheduler): " << scheduler_name;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 4, buf.str(), __FILE__, __LINE__);
                        return false;
                }
                ctrl.unload_module(module);
                request.vs_element.schedule_module_name = scheduler_name;
        }
        if (request.vs_element.protocol_module_name.length() == 0) {
                //protocol module name error
                std::string buf("protocol module is not specified.(--proto-module)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 5, buf, __FILE__, __LINE__);
                return false;
        }
        if (request.vs_element.udpmode) {
                if (request.vs_element.udp_recv_endpoint == boost::asio::ip::udp::endpoint()) {
                        // udp mode, but no receive endpoint
                        std::string buf("udp recv endpoint is not specified.(--udp-service)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 6, buf, __FILE__, __LINE__);
                        return false;
                }
        } else {
                if (request.vs_element.tcp_accept_endpoint == boost::asio::ip::tcp::endpoint()) {
                        // tcp mode, but no acceptor endpoint
                        std::string buf("tcp accpeptor endpoint is not specified.(--tcp-service)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 7, buf, __FILE__, __LINE__);
                        return false;
                }
        }
        if (0 > request.vs_element.sorry_maxconnection) {
                std::string buf("invalid sorry max connection value.(--upper)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 8, buf, __FILE__, __LINE__);
                return false;
        }
        if ((l7vsadm_request::CMD_ADD_VS == cmd) &&
            (request.vs_element.sorry_fwdmode == virtualservice_element::FWD_NONE)) {
                request.vs_element.sorry_fwdmode = virtualservice_element::FWD_MASQ;
        }

        if ((l7vsadm_request::CMD_ADD_VS == cmd) &&
            (request.vs_element.access_log_flag == 1) && (request.vs_element.access_log_file_name.length() == 0)) {
                std::string buf("access log file is not specified.(--access-log-name)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 89, buf, __FILE__, __LINE__);
                return false;
        }

        if (l7vsadm_request::CMD_EDIT_VS == cmd) {
                // Existence check of the parameter
                if (count_map["-s"] == 0 &&
                    count_map["--scheduler"] == 0 &&
                    count_map["-u"] == 0 &&
                    count_map["--upper"] == 0 &&
                    count_map["-b"] == 0 &&
                    count_map["--bypass"] == 0 &&
                    count_map["-f"] == 0 &&
                    count_map["--flag"] == 0 &&
                    count_map["--masq"] == 0 &&
                    count_map["--tproxy"] == 0 &&
                    count_map["-Q"] == 0 &&
                    count_map["--qos-up"] == 0 &&
                    count_map["-q"] == 0 &&
                    count_map["--qos-down"] == 0 &&
                    count_map["-L"] == 0 &&
                    count_map["--access-log"] == 0) {

                        std::string buf("All option omitted for edit vs command.");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 116, buf, __FILE__, __LINE__);
                        return false;
                }
        }

        //conflict check
        std::string conflict_option_name;
        bool is_conflict = false;

        for (std::map<std::string, int>::iterator itr = count_map.begin() ;
             itr != count_map.end() ; ++itr) {
                if (itr->second > 1) {
                        conflict_option_name = itr->first;
                        is_conflict = true;
                        break;
                }
        }

        if (is_conflict == false &&
            count_map["-t"] == 1 && count_map["--tcp-service"] == 1) {
                //-t(--tcp-service)
                conflict_option_name = "--tcp-service";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-m"] == 1 && count_map["--proto-module"] == 1) {
                //-m(--proto-module)
                conflict_option_name = "--proto-module";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-s"] == 1 && count_map["--scheduler"] == 1) {
                //-s(--scheduler)
                conflict_option_name = "--scheduler";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-u"] == 1 && count_map["--upper"] == 1) {
                //-u(--upper)
                conflict_option_name = "--upper";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-b"] == 1 && count_map["--bypass"] == 1) {
                //-b(--bypass)
                conflict_option_name = "--bypass";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-f"] == 1 && count_map["--flag"] == 1) {
                //-f(--flag)
                conflict_option_name = "--flag";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["--masq"] + count_map["--tproxy"] > 1) {
                conflict_option_name = "--masq/tproxy";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-Q"] == 1 && count_map["--qos-up"] == 1) {
                //-Q(--qos-up)
                conflict_option_name = "--qos-up";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-q"] == 1 && count_map["--qos-down"] == 1) {
                //-q(--qos-down)
                conflict_option_name = "--qos-down";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-p"] == 1 && count_map["--udp"] == 1) {
                //-p(--udp)
                conflict_option_name = "--udp";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-z"] == 1 && count_map["--ssl"] == 1) {
                //-z(--ssl)
                conflict_option_name = "--ssl";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-O"] == 1 && count_map["--sockopt"] == 1) {
                //-O(--sockopt)
                conflict_option_name = "--sockopt";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-L"] == 1 && count_map["--access-log"] == 1) {
                //-L(--access-log)
                conflict_option_name = "--access-log";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-a"] == 1 && count_map["--access-log-name"] == 1) {
                //-a(--access-log-name)
                conflict_option_name = "--access-log-name";
                is_conflict = true;
        }

        if (is_conflict == true) {
                std::stringstream buf;
                buf << "Option " << conflict_option_name << " is conflict.";
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 120, buf.str(), __FILE__, __LINE__);
                return false;
        }

        if (l7vsadm_request::CMD_ADD_VS == cmd &&
            (count_map["-z"] > 0 || count_map["--ssl"] > 0)) {
                protocol_module_control &ctrl
                = protocol_module_control::getInstance();
                ctrl.initialize(L7VS_MODULE_PATH);
                protocol_module_base *module;
                try {
                        module
                        = ctrl.load_module(request.vs_element.protocol_module_name);
                } catch (...) {
                        std::stringstream buf;
                        buf << "protocol module load error(--proto-module): "
                            << request.vs_element.protocol_module_name;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(
                                LOG_CAT_L7VSADM_PARSE,
                                108,
                                buf.str(),
                                __FILE__,
                                __LINE__);
                        return false;
                }
                if (!module) {
                        // protocol module not found.
                        std::stringstream buf;
                        buf << "protocol module not found(--proto-module): "
                            << request.vs_element.protocol_module_name;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(
                                LOG_CAT_L7VSADM_PARSE,
                                109, buf.str(),
                                __FILE__,
                                __LINE__);
                        return false;
                }

                bool module_used_flag = module->is_exec_OK(VS_CONTACT_CLASS_SSL);
                if (module_used_flag == false) {
                        // protocol module not found.
                        std::stringstream buf;
                        buf << "You cannot set \"-z\" option with \"sslid\" protocol module.";
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(
                                LOG_CAT_L7VSADM_PARSE,
                                110,
                                buf.str(),
                                __FILE__,
                                __LINE__);
                        return false;
                }
        }

        return true;
}
//
// option virtualservice functions.
//
//! target option check
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_target_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 4, "l7vsadm::parse_opt_vs_target_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // target address and port are not specified
                std::string buf("target endpoint is not specified.(--tcp-service)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 9, buf, __FILE__, __LINE__);
                return false;
        }
        // get host endpoint from string
        std::string src_str = argv[pos];
        if (request.vs_element.udpmode) {
                error_code err;
                request.vs_element.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>(src_str, err);
                if (err) {
                        std::stringstream buf;
                        buf << "target endpoint parse error(--tcp-service): " << err.get_message() << src_str;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 10, buf.str(), __FILE__, __LINE__);
                        return false;
                }
                check_endpoint<boost::asio::ip::udp>(request.vs_element.udp_recv_endpoint, true, err);
                if (err) {
                        std::stringstream buf;
                        buf << "target endpoint parse error(--tcp-service): " << err.get_message() << src_str;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 11, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        } else {
                error_code err;
                request.vs_element.tcp_accept_endpoint = string_to_endpoint<boost::asio::ip::tcp>(src_str, err);
                if (err) {
                        std::stringstream buf;
                        buf << "target endpoint parse error(--tcp-service): " << err.get_message() << src_str;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 12, buf.str(), __FILE__, __LINE__);
                        return false;
                }
                check_endpoint<boost::asio::ip::tcp>(request.vs_element.tcp_accept_endpoint, true, err);
                if (err) {
                        std::stringstream buf;
                        buf << "target endpoint parse error(--tcp-service): " << err.get_message() << src_str;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 13, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }
        return true;
}
//! module option check
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_module_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 5, "l7vsadm::parse_opt_vs_module_func", __FILE__, __LINE__);
        if (++pos >= argc) {
                // target protomod name not specified.
                std::string buf("protocol module name is not specified.(--proto-module)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 14, buf, __FILE__, __LINE__);
                return false;
        }
        std::string module_name = argv[pos];
        if (L7VS_MODNAME_LEN < module_name.length()) {
                std::string buf("protocol module name is too long.(--proto-module)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 15, buf, __FILE__, __LINE__);
                return false;
        }
        protocol_module_control &ctrl = protocol_module_control::getInstance();
        ctrl.initialize(L7VS_MODULE_PATH);
        protocol_module_base *module;
        try {
                module = ctrl.load_module(module_name);
        } catch (...) {
                std::stringstream buf;
                buf << "protocol module load error(--proto-module): " << module_name;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 16, buf.str(), __FILE__, __LINE__);
                return false;
        }
        if (!module) {
                // protocol module not found.
                std::stringstream buf;
                buf << "protocol module not found(--proto-module): " << module_name;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 17, buf.str(), __FILE__, __LINE__);
                return false;
        }
        module->init_logger_functions(
                boost::bind(&l7vs::Logger::getLogLevel, l7vs::LOG_CAT_PROTOCOL),
                boost::bind(&l7vs::Logger::putLogFatal, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4),
                boost::bind(&l7vs::Logger::putLogError, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4),
                boost::bind(&l7vs::Logger::putLogWarn, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4),
                boost::bind(&l7vs::Logger::putLogInfo, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4),
                boost::bind(&l7vs::Logger::putLogDebug, l7vs::LOG_CAT_PROTOCOL, _1, _2, _3, _4));
        // create module args.
        std::vector<std::string> module_args;
        while (true) {
                if (++pos == argc) break; //module option end.
                parse_opt_map_type::iterator vsitr = vs_option_dic.find(argv[pos]);
                if (vsitr != vs_option_dic.end()) {
                        --pos; // back for next option
                        break; // module option end.
                }
                parse_opt_map_type::iterator rsitr = rs_option_dic.find(argv[pos]);
                if (rsitr != rs_option_dic.end()) {
                        --pos; // back for next option
                        break; // module option end.
                }
                module_args.push_back(argv[pos]);
        }
        protocol_module_base::check_message_result module_message = module->check_parameter(module_args);

        if (!module_message.flag) {
                // args is not supported.
                std::stringstream buf;
                buf << "protocol module argument error(--proto-module): " << module_message.message;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 18, buf.str(), __FILE__, __LINE__);
                return false;
        }
        request.vs_element.protocol_module_name = module_name;
        BOOST_FOREACH(std::string str, module_args) {
                request.vs_element.protocol_args.push_back(str);
        }
        ctrl.unload_module(module);

        return true;
}

//! scheduler option check.
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_scheduler_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 6, "l7vsadm::parse_opt_vs_scheduler_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // target scheduler name not specified.
                std::string buf("schedule module name is not specified.(--scheduler)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 19, buf, __FILE__, __LINE__);
                return false;
        }
        //schedule module check.
        std::string scheduler_name = argv[pos];
        if (L7VS_MODNAME_LEN < scheduler_name.length()) {
                std::string buf("schedule module name is too long.(--scheduler)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 20, buf, __FILE__, __LINE__);
                return false;
        }
        schedule_module_control &ctrl = schedule_module_control::getInstance();
        ctrl.initialize(L7VS_MODULE_PATH);
        schedule_module_base *module;
        try {
                module = ctrl.load_module(scheduler_name);
        } catch (...) {
                std::stringstream buf;
                buf << "scheduler module load error(--scheduler): " << scheduler_name;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 21, buf.str(), __FILE__, __LINE__);
                return false;
        }
        if (!module) {
                // schedule module not found
                std::stringstream buf;
                buf << "scheduler module not found(--scheduler): " << scheduler_name;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 22, buf.str(), __FILE__, __LINE__);
                return false;
        }
        ctrl.unload_module(module);
        request.vs_element.schedule_module_name = scheduler_name;
        return true;
}
//! upper flag check
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_upper_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 7, "l7vsadm::parse_opt_vs_upper_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // target max connection number not specified
                std::string buf("sorry max connection value is not specified.(--upper)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 23, buf, __FILE__, __LINE__);
                return false;
        }
        try {
                request.vs_element.sorry_maxconnection = boost::lexical_cast<long long>(argv[pos]);
                if ((0LL > request.vs_element.sorry_maxconnection) ||
                    (100000LL < request.vs_element.sorry_maxconnection)) {
                        std::string buf("invalid sorry max connection value.(--upper)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 24, buf, __FILE__, __LINE__);
                        return false;
                }
                if (0LL == request.vs_element.sorry_maxconnection)
                        request.vs_element.sorry_maxconnection = LLONG_MAX; // clear value
        } catch (boost::bad_lexical_cast &e) {
                std::string buf("invalid sorry max connection value.(--upper)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 25, buf, __FILE__, __LINE__);
                return false;
        }
        //check connection limit and zero
        return true;
}
//! bypass(SorryServer) option check
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_bypass_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 8, "l7vsadm::parse_opt_vs_bypass_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // sorryserver addr and port not specified
                std::string buf("sorryserver endpoint is not specified.(--bypass)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 26, buf, __FILE__, __LINE__);
                return false;
        }
        std::string sorry_endpoint = argv[pos];
        error_code err;
        request.vs_element.sorry_endpoint = string_to_endpoint<boost::asio::ip::tcp>(sorry_endpoint, err);
        if (err) {
                std::stringstream buf;
                buf << "sorryserver endpoint parse error(--bypass): " << err.get_message() << sorry_endpoint;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 27, buf.str(), __FILE__, __LINE__);
                return false;
        }
        // clear endpoint check (0.0.0.0:0)
        if (request.vs_element.sorry_endpoint == boost::asio::ip::tcp::endpoint()) {
                request.vs_element.sorry_endpoint = boost::asio::ip::tcp::endpoint(
                                boost::asio::ip::address::from_string("255.255.255.255"), 0);
        } else {
                check_endpoint<boost::asio::ip::tcp>(request.vs_element.sorry_endpoint, false, err);
                if (err) {
                        std::stringstream buf;
                        buf << "sorryserver endpoint parse error(--bypass): " << err.get_message() << sorry_endpoint;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 29, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }
        return true;
}
//! virtualservice option flag function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_flag_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 9, "l7vsadm::parse_opt_vs_flag_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // sorry flag is not specified
                std::string buf("sorry flag value is not specified.(--flag)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 30, buf, __FILE__, __LINE__);
                return false;
        }
        try {
                int tmp = boost::lexical_cast<int>(argv[pos]);
                if ((0 != tmp) && (1 != tmp)) {
                        std::string buf("invalid sorry flag value.(--flag)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 31, buf, __FILE__, __LINE__);
                        return false;
                }
                if (0 == tmp)
                        request.vs_element.sorry_flag = INT_MAX; // clear value
                else
                        request.vs_element.sorry_flag = 1;
        } catch (boost::bad_lexical_cast &e) {
                std::string buf("invalid sorry flag value.(--flag)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 32, buf, __FILE__, __LINE__);
                return false;
        }
        return true;
}
//! virtualservice option sorry forward mode function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_fwdmode_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, /*XXX*/999, "l7vsadm::parse_opt_vs_fwdmode_func", __FILE__, __LINE__);

        std::string opt(argv[pos]);
        if (opt == "--masq") {
                request.vs_element.sorry_fwdmode = virtualservice_element::FWD_MASQ;
        } else if (opt == "--tproxy") {
#ifdef IP_TRANSPARENT
                request.vs_element.sorry_fwdmode = virtualservice_element::FWD_TPROXY;
#else
                std::stringstream buf("tproxy(IP_TRANSPARENT) not supported on this platform.");
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogInfo(LOG_CAT_L7VSADM_PARSE, /*XXX*/999, buf.str(), __FILE__, __LINE__);
                return false;
#endif
        } else {
                // should never come here
                std::stringstream buf;
                buf << boost::format("unknown sorryserver forward mode specified.(%s)") % opt;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, /*XXX*/999, buf.str(), __FILE__, __LINE__);
                return false;
        }
        return true;
}
//! virtualservice option qosupstream function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_qosup_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 10, "l7vsadm::parse_opt_vs_qosup_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // QoS upstream value is not specified.
                std::string buf("QoS upstream value is not specified.(--qos-up)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 33, buf, __FILE__, __LINE__);
                return false;
        }
        try {
                virtualservice_element &elem = request.vs_element; // request virtualservice element reference get.
                std::string tmp = argv[pos];
                std::string::reverse_iterator ritr = tmp.rbegin();
                if (*ritr == 'G' || *ritr == 'g') {
                        std::string strval = tmp.substr(0, tmp.length() - 1);
                        unsigned long long ullval = boost::lexical_cast<unsigned long long> (strval);
                        if (ullval > 999) {
                                std::string buf("QoS upstream value is too big.(--qos-up)");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 34, buf, __FILE__, __LINE__);
                                return false;
                        }
                        elem.qos_upstream = ullval * 1000 * 1000 * 1000; // set qos_upstream
                } else if (*ritr == 'M' || *ritr == 'm') {
                        std::string strval = tmp.substr(0, tmp.length() - 1);
                        unsigned long long ullval = boost::lexical_cast<unsigned long long> (strval);
                        if (ullval > 999) {
                                std::string buf("QoS upstream value is too big.(--qos-up)");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 35, buf, __FILE__, __LINE__);
                                return false;
                        }
                        elem.qos_upstream = ullval * 1000 * 1000; // set qos_upstream
                } else if (*ritr == 'K' || *ritr == 'k') {
                        std::string strval = tmp.substr(0, tmp.length() - 1);
                        unsigned long long ullval = boost::lexical_cast<unsigned long long> (strval);
                        if (ullval > 999) {
                                std::string buf("QoS upstream value is too big.(--qos-up)");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 36, buf, __FILE__, __LINE__);
                                return false;
                        }
                        elem.qos_upstream = ullval * 1000; // set qos_upstream
                } else {
                        unsigned long long ullval = boost::lexical_cast<unsigned long long> (argv[pos]);
                        if (ullval > 999) {
                                std::string buf("QoS upstream value is too big.(--qos-up)");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 111, buf, __FILE__, __LINE__);
                                return false;
                        }
                        elem.qos_upstream = ullval; // set qos_upstream
                }
                if (0ULL == elem.qos_upstream) {
                        elem.qos_upstream = ULLONG_MAX; // clear value
                } else {
                        elem.qos_upstream /= 8; //qos convert to bytes per sec to bit per sec
                }

        } catch (boost::bad_lexical_cast &ex) {
                std::string buf("invalid QoS upstream value.(--qos-up)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 37, buf, __FILE__, __LINE__);
                return false;
        }
        return true;
}
//! virtualservice option qosdownstream function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_qosdown_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 11, "l7vsadm::parse_opt_vs_qosdown_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // QoS downstream value is not specified
                std::string buf("QoS downstream value is not specified.(--qos-down)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 38, buf, __FILE__, __LINE__);
                return false;
        }
        try {
                virtualservice_element &elem = request.vs_element; // request virtualservice element reference get.
                std::string tmp = argv[pos];
                std::string::reverse_iterator ritr = tmp.rbegin();
                if (*ritr == 'G' || *ritr == 'g') {
                        std::string strval = tmp.substr(0, tmp.length() - 1);
                        unsigned long long ullval = boost::lexical_cast<unsigned long long> (strval);
                        if (ullval > 999) {
                                std::string buf("QoS downstream value is too big.(--qos-down)");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 39, buf, __FILE__, __LINE__);
                                return false;
                        }
                        elem.qos_downstream = ullval * 1000 * 1000 * 1000; // set qos_upstream
                } else if (*ritr == 'M' || *ritr == 'm') {
                        std::string strval = tmp.substr(0, tmp.length() - 1);
                        unsigned long long ullval = boost::lexical_cast<unsigned long long> (strval);
                        if (ullval > 999) {
                                std::string buf("QoS downstream value is too big.(--qos-down)");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 40, buf, __FILE__, __LINE__);
                                return false;
                        }
                        elem.qos_downstream = ullval * 1000 * 1000; // set qos_upstream
                } else if (*ritr == 'K' || *ritr == 'k') {
                        std::string strval = tmp.substr(0, tmp.length() - 1);
                        unsigned long long ullval = boost::lexical_cast<unsigned long long> (strval);
                        if (ullval > 999) {
                                std::string buf("QoS downstream value is too big.(--qos-down)");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 41, buf, __FILE__, __LINE__);
                                return false;
                        }
                        elem.qos_downstream = ullval * 1000; // set qos_upstream
                } else {
                        unsigned long long ullval = boost::lexical_cast<unsigned long long> (argv[pos]);
                        if (ullval > 999) {
                                std::string buf("QoS downstream value is too big.(--qos-down)");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 115, buf, __FILE__, __LINE__);
                                return false;
                        }
                        elem.qos_downstream = boost::lexical_cast<unsigned long long> (argv[pos]); // set qos_downstream
                }
                if (0ULL == elem.qos_downstream) {
                        elem.qos_downstream = ULLONG_MAX; // clear value
                } else {
                        elem.qos_downstream /= 8; //qos convert to bytes per sec to bit per sec
                }
        } catch (boost::bad_lexical_cast &ex) {
                // don' conv qos downstream
                std::string buf("invalid QoS downstream value.(--qos-down)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 42, buf, __FILE__, __LINE__);
                return false;
        }
        return true;
}
//! virtualservice option udp func.
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_udp_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 12, "l7vsadm::parse_opt_vs_udp_func", __FILE__, __LINE__);

        virtualservice_element &elem = request.vs_element; // request virtualservice element reference get.
        elem.udpmode = true; // udpmode on.
        boost::asio::ip::tcp::endpoint zeropoint;
        if (zeropoint != elem.tcp_accept_endpoint) { // address tcp_acceptor endpoint
                std::stringstream sstream;
                sstream << elem.tcp_accept_endpoint;
                std::string endpoint = sstream.str();
                error_code err;
                elem.udp_recv_endpoint = string_to_endpoint<boost::asio::ip::udp>(endpoint, err);
                if (err) {
                        std::stringstream buf;
                        buf << "target endpoint parse error(--udp-service): " << err.get_message() << endpoint;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 43, buf.str(), __FILE__, __LINE__);
                        return false;
                }
                elem.tcp_accept_endpoint = zeropoint;
        }
        if (elem.realserver_vector.size() != 0 && elem.realserver_vector.front().tcp_endpoint != zeropoint) {
                std::stringstream sstream;
                sstream << elem.realserver_vector.front().tcp_endpoint;
                std::string endpoint = sstream.str();
                error_code err;
                elem.realserver_vector.front().udp_endpoint = string_to_endpoint<boost::asio::ip::udp> (endpoint, err);
                if (err) {
                        std::stringstream buf;
                        buf << "realserver endpoint parse error(--udp-service): " << err.get_message() << endpoint;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 44, buf.str(), __FILE__, __LINE__);
                        return false;
                }
                elem.realserver_vector.front().tcp_endpoint = zeropoint;
        }
        return true;
}
//! virtualservice option ssl_file function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_ssl_file_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 38, "l7vsadm::parse_opt_vs_ssl_file_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                std::string buf("ssl config file is not specified.(--ssl)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 90, buf, __FILE__, __LINE__);
                return false;
        }
        // ssl config file check.
        std::string conf_file_name = argv[pos];
        if (L7VS_FILENAME_LEN < conf_file_name.length()) {
                std::string buf("ssl config file name is too long.(--ssl)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 91, buf, __FILE__, __LINE__);
                return false;
        }
        FILE *fp;
        if ((fp = fopen(conf_file_name.c_str(), "r")) == NULL) {
                std::string buf("ssl config file cannot open.(--ssl)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 92, buf, __FILE__, __LINE__);
                return false;
        }
        fclose(fp);

        request.vs_element.ssl_file_name = conf_file_name;
        return true;
}

//! virtualservice option access log function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_access_log_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 39, "l7vsadm::parse_opt_vs_access_log_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // access log flag is not specified
                std::string buf("access log flag value is not specified.(--access-log)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 93, buf, __FILE__, __LINE__);
                return false;
        }
        try {
                int tmp = boost::lexical_cast<int>(argv[pos]);
                if ((0 != tmp) && (1 != tmp)) {
                        std::string buf("invalid access log flag value.(--access-log)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 94, buf, __FILE__, __LINE__);
                        return false;
                }
                request.vs_element.access_log_flag = tmp;
        } catch (boost::bad_lexical_cast &e) {
                std::string buf("invalid access log flag value.(--access-log)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 95, buf, __FILE__, __LINE__);
                return false;
        }
        return true;
}

//! virtualservice option access_log_logrotate function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_access_log_logrotate_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 40, "l7vsadm::parse_opt_vs_accesslog_logrotate_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                std::string buf("access log file is not specified.(--access-log-name)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 96, buf, __FILE__, __LINE__);
                return false;
        }
        // access log file check.
        std::string access_log_file_name = argv[pos];
        if (L7VS_FILENAME_LEN < access_log_file_name.length()) {
                std::string buf("access log file name is too long.(--access-log-name)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 97, buf, __FILE__, __LINE__);
                return false;
        }
        if ("/" != access_log_file_name.substr(0, 1)) {
                std::string buf("please specify access log filename in fullpath.(--access-log-name)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 98, buf, __FILE__, __LINE__);
                return false;
        }

        request.vs_element.access_log_rotate_key_info = "";

        // create access log args.
        std::vector<std::string> arguments_vector;
        virtualservice_element::access_log_rotate_arguments_map_type arguments_map;
        while (true) {
                if (++pos == argc) break; //access log arguments end.
                parse_opt_map_type::iterator vsitr = vs_option_dic.find(argv[pos]);
                if (vsitr != vs_option_dic.end()) {
                        --pos; // back for next option
                        break; // module option end.
                }
                arguments_vector.push_back(argv[pos]);

                request.vs_element.access_log_rotate_key_info += argv[pos];
                request.vs_element.access_log_rotate_key_info += " ";

        }
        boost::algorithm::erase_last(request.vs_element.access_log_rotate_key_info , " ");

        if (0 < arguments_vector.size()) {
                if (0 == (arguments_vector.size() % 2)) {
                        for (unsigned int i = 0; i < (arguments_vector.size() - 1); ++i) {
                                std::pair<virtualservice_element::access_log_rotate_arguments_map_type::iterator, bool> ret =
                                        arguments_map.insert(
                                                virtualservice_element::access_log_rotate_arguments_pair_type(
                                                        arguments_vector[i], arguments_vector[i+1]));
                                if (!ret.second) {
                                        std::string buf("access log rotation argument is duplicated.(--access-log-name)");
                                        l7vsadm_err.setter(true, buf);
                                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 99, buf, __FILE__, __LINE__);
                                        return false;
                                }
                                ++i;
                        }
                } else {
                        std::string buf("access log rotation argument error.(--access-log-name)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 100, buf, __FILE__, __LINE__);
                        return false;
                }
                bool ret = logger_access_manager::getInstance().access_log_logrotate_parameter_check(arguments_map);
                if (!ret) {
                        std::string buf("access log rotation argument error.(--access-log-name)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 101, buf, __FILE__, __LINE__);
                        return false;
                }
        }

        request.vs_element.access_log_file_name = access_log_file_name;
        request.vs_element.access_log_rotate_arguments.clear();
        BOOST_FOREACH(virtualservice_element::access_log_rotate_arguments_pair_type pair, arguments_map) {
                request.vs_element.access_log_rotate_arguments.insert(pair);
        }

        return true;
}

//! virtualservice option socket function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_vs_socket_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 41, "l7vsadm::parse_opt_vs_socket_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                std::string buf("socket option is not specified.(--sockopt)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 102, buf, __FILE__, __LINE__);
                return false;
        }

        bool is_set_defer_accept = false;
        bool is_set_nodelay = false;
        bool is_set_cork = false;
        bool is_set_quickack = false;

        request.vs_element.socket_option_tcp_defer_accept = 0;
        request.vs_element.socket_option_tcp_nodelay = 0;
        request.vs_element.socket_option_tcp_cork = 0;
        request.vs_element.socket_option_tcp_quickack = 0;

        std::string socket_option_string = argv[pos];
        std::vector<std::string> socket_options;
        boost::split(socket_options, socket_option_string, boost::algorithm::is_any_of(","));

        BOOST_FOREACH(std::string option, socket_options) {
                if (option == "deferaccept") {
                        if (!is_set_defer_accept) {
                                is_set_defer_accept = true;
                                request.vs_element.socket_option_tcp_defer_accept = 1;
                        } else {
                                // defer_accept is duplicated
                                std::stringstream buf;
                                buf << "socket option deferaccept is duplicated.(--sockopt)";
                                l7vsadm_err.setter(true, buf.str());
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 103, buf.str(), __FILE__, __LINE__);
                                return false;
                        }
                } else if (option == "nodelay") {
                        if (!is_set_nodelay) {
                                is_set_nodelay = true;
                                request.vs_element.socket_option_tcp_nodelay = 1;
                        } else {
                                // nodelay is duplicated
                                std::stringstream buf;
                                buf << "socket option nodelay is duplicated.(--sockopt)";
                                l7vsadm_err.setter(true, buf.str());
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 104, buf.str(), __FILE__, __LINE__);
                                return false;
                        }
                } else if (option == "cork") {
                        if (!is_set_cork) {
                                is_set_cork = true;
                                request.vs_element.socket_option_tcp_cork = 1;
                        } else {
                                // cork is duplicated
                                std::stringstream buf;
                                buf << "socket option cork is duplicated.(--sockopt)";
                                l7vsadm_err.setter(true, buf.str());
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 105, buf.str(), __FILE__, __LINE__);
                                return false;
                        }
                } else if (option == "quickackon" || option == "quickackoff") {
                        if (!is_set_quickack) {
                                is_set_quickack = true;
                                request.vs_element.socket_option_tcp_quickack = ((option == "quickackon") ? 1 : 2);
                        } else {
                                // quickack is duplicated
                                std::stringstream buf;
                                buf << "socket option quickack is duplicated.(--sockopt)";
                                l7vsadm_err.setter(true, buf.str());
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 106, buf.str(), __FILE__, __LINE__);
                                return false;
                        }
                } else {
                        // unknown socket option
                        std::stringstream buf;
                        buf << "unknown socket option.(--sockopt)";
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 107, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }

        request.vs_element.socket_option_string = socket_option_string;
        return true;

}
//! realserver command parsing.
//! @param[in] request command
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_rs_func(l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 13, "l7vsadm::parse_rs_func", __FILE__, __LINE__);

        if (argc < 8) {
                //argument num err
                std::stringstream buf;
                buf << "Argument argc is illegal for ";
                buf << argv[1];
                buf << " command.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 121, buf.str(), __FILE__, __LINE__);
                return false;
        }

        request.command = cmd;

        request.vs_element.realserver_vector.push_back(realserver_element());

        std::map<std::string, int> count_map;
        for (parse_opt_map_type::iterator itr = rs_option_dic.begin() ;
             itr != rs_option_dic.end() ; ++itr) {
                count_map[ itr->first ] = 0;
        }

        for (int pos = 2; pos < argc; ++pos) {
                parse_opt_map_type::iterator itr = rs_option_dic.find(argv[pos]);
                if (itr != rs_option_dic.end()) {
                        count_map[ itr->first ]++;
                        if (! itr->second(pos, argc, argv)) return false;
                } else {
                        std::stringstream buf;
                        buf << "realserver option not found(--real-server): " << argv[pos];
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 45, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }

        if (request.vs_element.protocol_module_name.length() == 0) {
                //protocol module name error
                std::string buf("protocol module is not specified.(--proto-module)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 46, buf, __FILE__, __LINE__);
                return false;
        }
        if (request.vs_element.udpmode) {
                if (request.vs_element.udp_recv_endpoint == boost::asio::ip::udp::endpoint()) {
                        // udp mode,but not acceptor endpoint
                        std::string buf("udp receive endpoint is not specified.(--udp-service)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 47, buf, __FILE__, __LINE__);
                        return false;
                }
                if (request.vs_element.realserver_vector.front().udp_endpoint == boost::asio::ip::udp::endpoint()) {
                        // udp mode,but not realserver endpoint
                        std::string buf("realserver udp endpoint is not specified.(--real-server)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 48, buf, __FILE__, __LINE__);
                        return false;
                }
        } else {
                if (request.vs_element.tcp_accept_endpoint == boost::asio::ip::tcp::endpoint()) {
                        // tcp mode, but not acceptor endpoint
                        std::string buf("tcp accpeptor endpoint not specified.(--tcp-service)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 49, buf, __FILE__, __LINE__);
                        return false;
                }
                if (request.vs_element.realserver_vector.front().tcp_endpoint == boost::asio::ip::tcp::endpoint()) {
                        // tcp mode,but not realserver endpoint
                        std::string buf("realserver tcp endpoint not specified.(--real-server)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 50, buf, __FILE__, __LINE__);
                        return false;
                }
        }

        if (l7vsadm_request::CMD_EDIT_RS != cmd) {
                // realserver weight default value = 1
                if (-1 == request.vs_element.realserver_vector.front().weight) {
                        request.vs_element.realserver_vector.front().weight = 1;
                }
                // realserver forward mode default value = FWD_MASQ
                if (realserver_element::FWD_NONE == request.vs_element.realserver_vector.front().fwdmode) {
                        request.vs_element.realserver_vector.front().fwdmode = realserver_element::FWD_MASQ;
                }
        } else {
                // Existence check of the parameter
                if (count_map["-w"] == 0 &&
                    count_map["--weight"] == 0 &&
                    count_map["--masq"] == 0 &&
                    count_map["--tproxy"] == 0) {
                        std::string buf("All option omitted for edit rs command.");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 112, buf, __FILE__, __LINE__);
                        return false;
                }
        }

        //conflict check
        std::string conflict_option_name;
        bool is_conflict = false;

        for (std::map<std::string, int>::iterator itr = count_map.begin() ;
             itr != count_map.end() ; ++itr) {
                if (itr->second > 1) {
                        conflict_option_name = itr->first;
                        is_conflict = true;
                        break;
                }
        }

        if (is_conflict == false &&
            count_map["-t"] == 1 && count_map["--tcp-service"] == 1) {
                //-t(--tcp-service)
                conflict_option_name = "--tcp-service";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-m"] == 1 && count_map["--proto-module"] == 1) {
                //-m(--proto-module)
                conflict_option_name = "--proto-module";
                is_conflict = true;
        }
        if (is_conflict == false &&
            count_map["-r"] == 1 && count_map["--real-server"] == 1) {
                //-r(--real-server)
                conflict_option_name = "--real-server";
                is_conflict = true;
        }
        if ((is_conflict == false) &&
            (count_map["-w"] == 1) && (count_map["--weight"] == 1) &&
            (l7vsadm_request::CMD_DEL_RS != cmd)) {
                //-w(--weight)
                conflict_option_name = "--weight";
                is_conflict = true;
        }
        if ((is_conflict == false) &&
            count_map["--masq"] + count_map["--tproxy"] > 1 &&
            (l7vsadm_request::CMD_DEL_RS != cmd)) {
                //--masq/tproxy
                conflict_option_name = "--masq/tproxy";
                is_conflict = true;
        }

        if (is_conflict == true) {
                std::stringstream buf;
                buf << "Option " << conflict_option_name << " is conflict.";
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 122, buf.str(), __FILE__, __LINE__);
                return false;
        }

        return true;
}
//
// realserver option functions.
//
//! realserver weight set
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_rs_weight_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 14, "l7vsadm::parse_opt_rs_weight_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // weight value is not specified
                std::string buf("weight value is not specified.(--weight)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 51, buf, __FILE__, __LINE__);
                return false;
        }
        try {
                request.vs_element.realserver_vector.front().weight = boost::lexical_cast<int>(argv[pos]);
                if ((0 > request.vs_element.realserver_vector.front().weight) ||
                    (100 < request.vs_element.realserver_vector.front().weight)) {
                        std::string buf("invalid weight value.(--weight)");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 52, buf, __FILE__, __LINE__);
                        return false;
                }
        } catch (boost::bad_lexical_cast &ex) {
                // lexical cast error
                std::string buf("invalid weight value.(--weight)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 53, buf, __FILE__, __LINE__);
                return false;
        }
        return true;
}
//! realserver forward mode set
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_rs_fwdmode_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, /*XXX*/999, "l7vsadm::parse_opt_rs_fwdmode_func", __FILE__, __LINE__);

        std::string opt(argv[pos]);
        if (opt == "--masq") {
                request.vs_element.realserver_vector.front().fwdmode = realserver_element::FWD_MASQ;
        } else if (opt == "--tproxy") {
#ifdef IP_TRANSPARENT
                request.vs_element.realserver_vector.front().fwdmode = realserver_element::FWD_TPROXY;
#else
                std::stringstream buf("tproxy(IP_TRANSPARENT) not supported on this platform.");
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogInfo(LOG_CAT_L7VSADM_PARSE, /*XXX*/999, buf.str(), __FILE__, __LINE__);
                return false;
#endif
        } else {
                // should never come here
                std::stringstream buf;
                buf << boost::format("unknown realserver forward mode specified.(%s)") % opt;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, /*XXX*/999, buf.str(), __FILE__, __LINE__);
                return false;
        }
        return true;

}
//! realserver target set
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_rs_realserver_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 15, "l7vsadm::parse_opt_rs_realserver_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // realserver address is not specified
                std::string buf("realserver address is not specified.(--real-server)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 54, buf, __FILE__, __LINE__);
                return false;
        }
        std::string src_str = argv[pos];
        if (request.vs_element.udpmode) {
                error_code err;
                request.vs_element.realserver_vector.front().udp_endpoint = string_to_endpoint<boost::asio::ip::udp>(src_str, err);
                if (err) {
                        // address string error.
                        std::stringstream buf;
                        buf << "realserver endpoint parse error(--real-server): " << err.get_message() << src_str;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 55, buf.str(), __FILE__, __LINE__);
                        return false;
                }
                check_endpoint<boost::asio::ip::udp>(request.vs_element.realserver_vector.front().udp_endpoint, false, err);
                if (err) {
                        std::stringstream buf;
                        buf << "realserver endpoint parse error(--real-server): " << err.get_message() << src_str;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 56, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        } else {
                error_code err;
                request.vs_element.realserver_vector.front().tcp_endpoint = string_to_endpoint<boost::asio::ip::tcp>(src_str, err);
                if (err) {
                        // address string error.
                        std::stringstream buf;
                        buf << "realserver endpoint parse error(--real-server): " << err.get_message() << src_str;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 57, buf.str(), __FILE__, __LINE__);
                        return false;
                }
                check_endpoint<boost::asio::ip::tcp>(request.vs_element.realserver_vector.front().tcp_endpoint, false, err);
                if (err) {
                        std::stringstream buf;
                        buf << "realserver endpoint parse error(--real-server): " << err.get_message() << src_str;
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 58, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }
        return true;
}

//! replication command parsing.
//! @param[in] request command
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_replication_func(l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 16, "l7vsadm::parse_replication_func", __FILE__, __LINE__);

        if (argc < 3 || argc > 4) {
                //argument num err
                std::stringstream buf;
                buf << "Argument argc is illegal for ";
                buf << argv[1];
                buf << " command.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 123, buf.str(), __FILE__, __LINE__);
                return false;
        }

        request.command = cmd;

        for (int pos = 2; pos < argc; ++pos) {
                parse_opt_map_type::iterator itr = replication_option_dic.find(argv[pos]);
                if (itr != replication_option_dic.end()) {
                        if (! itr->second(pos, argc, argv)) return false;
                } else {
                        std::stringstream buf;
                        buf << "replication option not found(--replication): " << argv[pos];
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 59, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }
        if (l7vsadm_request::REP_NONE == request.replication_command) {
                // not specified replication command
                std::string buf("replication command not specified.(--replication)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 60, buf, __FILE__, __LINE__);
                return false;
        }

        return true;
}

//
// replication option functions.
//
//! replication switch function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_replication_switch_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 17, "l7vsadm::parse_opt_replication_switch_func", __FILE__, __LINE__);

        if (request.replication_command != l7vsadm_request::REP_NONE) {
                // double command target.
                std::string buf("replication option is double specified.(--switch)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 61, buf, __FILE__, __LINE__);
                return false;
        }
        if (++pos >= argc) {
                // replication switch value is not specified
                std::string buf("replication switch option is not specified.(--switch)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 62, buf, __FILE__, __LINE__);
                return false;
        }
        parse_opt_map_type::iterator itr = replication_switch_option_dic.find(argv[pos]);
        if (itr != replication_switch_option_dic.end()) { // option string function find.
                if (! itr->second(pos, argc, argv)) return false; // option string function error.
        } else { //option string function not found.
                // print option not found message.
                std::stringstream buf;
                buf << "replication switch option not found(--switch): " << argv[pos];
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 63, buf.str(), __FILE__, __LINE__);
                return false;
        }
        return true;
}
//! replication start function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_replication_start_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 18, "l7vsadm::parse_opt_replication_start_func", __FILE__, __LINE__);

        request.replication_command = l7vsadm_request::REP_START;
        return true;
}
//! replication stop function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_replication_stop_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 19, "l7vsadm::parse_opt_replication_stop_func", __FILE__, __LINE__);

        request.replication_command = l7vsadm_request::REP_STOP;
        return true;
}
//! replication force function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_replication_force_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 20, "l7vsadm::parse_opt_replication_force_func", __FILE__, __LINE__);

        if (request.replication_command != l7vsadm_request::REP_NONE) {
                // double command target.
                std::string buf("replication option is double specified.(--force)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 64, buf, __FILE__, __LINE__);
                return false;
        }
        request.replication_command = l7vsadm_request::REP_FORCE;
        return true;
}
//! replication dump function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_replication_dump_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 21, "l7vsadm::parse_opt_replication_dump_func", __FILE__, __LINE__);

        if (request.replication_command != l7vsadm_request::REP_NONE) {
                // double command target.
                std::string buf("replication option is double specified.(--dump)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 65, buf, __FILE__, __LINE__);
                return false;
        }
        request.replication_command = l7vsadm_request::REP_DUMP;
        return true;
}

//! log command parsing.
//! @param[in] request command
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_log_func(l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 22, "l7vsadm::parse_log_func", __FILE__, __LINE__);

        if (argc != 6) {
                //argument num err
                std::stringstream buf;
                buf << "Argument argc is illegal for ";
                buf << argv[1];
                buf << " command.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 124, buf.str(), __FILE__, __LINE__);
                return false;
        }

        request.command = cmd;

        for (int pos = 2; pos < argc; ++pos) {
                parse_opt_map_type::iterator itr = log_option_dic.find(argv[pos]);
                if (itr != log_option_dic.end()) { // option string function find.
                        if (! itr->second(pos, argc, argv)) return false; // option string function error.
                } else { //option string function not found.
                        // print option not found message.
                        std::stringstream buf;
                        buf << "log option not found(--log): " << argv[pos];
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 66, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }
        if (LOG_CAT_NONE == request.log_category) {
                // not specified logcategory
                std::string buf("log category is not specified(--category).");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 67, buf, __FILE__, __LINE__);
                return false;
        }
        if (LOG_LV_NONE == request.log_level) {
                // not specified loglevel
                std::string buf("log level is not specified.(--level)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 68, buf, __FILE__, __LINE__);
                return false;
        }

        return true;
}
//
// log option function
//
//! log category set function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_log_category_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 23, "l7vsadm::parse_opt_log_category_func", __FILE__, __LINE__);

        if (request.log_category != LOG_CAT_NONE) {
                // double target commands.
                std::stringstream buf;
                buf << "Option ";
                buf << argv[pos];
                buf << " conflict.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 69, buf.str(), __FILE__, __LINE__);
                return false;
        }
        if (++pos >= argc) {
                // log category is not specified.
                std::string buf("log category is not specified.(--category)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 70, buf, __FILE__, __LINE__);
                return false;
        }
        string_logcategory_map_type::iterator itr = string_logcategory_dic.find(argv[pos]);
        if (itr != string_logcategory_dic.end()) {
                request.log_category = itr->second;
                return true;
        }
        std::stringstream buf;
        buf << "log category not found(--category): " << argv[pos];
        l7vsadm_err.setter(true, buf.str());
        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 71, buf.str(), __FILE__, __LINE__);
        return false;
}
//! log level set function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_log_level_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 24, "l7vsadm::parse_opt_log_level_func", __FILE__, __LINE__);

        if (request.log_level != LOG_LV_NONE) {
                // double target commands.
                std::stringstream buf;
                buf << "Option ";
                buf << argv[pos];
                buf << " conflict.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 72, buf.str(), __FILE__, __LINE__);
                return false;
        }
        if (++pos >= argc) {
                // log level is not specified.
                std::string buf("log level is not specified.(--level)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 73, buf, __FILE__, __LINE__);
                return false;
        }
        string_loglevel_map_type::iterator itr = string_loglevel_dic.find(argv[pos]);
        if (itr != string_loglevel_dic.end()) {
                request.log_level = itr->second;
                return true;
        }
        std::stringstream buf;
        buf << "log level not found(--level): " << argv[pos];
        l7vsadm_err.setter(true, buf.str());
        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 74, buf.str(), __FILE__, __LINE__);
        return false;
}

//! snmp command parsing
//! @param[in] request command
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_snmp_func(l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 25, "l7vsadm::parse_snmp_func", __FILE__, __LINE__);

        //Argument argc is illegal
        if (argc < 3) {
                //argument num err
                std::stringstream buf;
                buf << "Argument argc is illegal for ";
                buf << argv[1];
                buf << " command.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 127, buf.str(), __FILE__, __LINE__);
                return false;
        }

        request.command = cmd;

        for (int pos = 2; pos < argc; ++pos) {
                parse_opt_map_type::iterator itr = snmp_option_dic.find(argv[pos]);
                if (itr != snmp_option_dic.end()) { // option string function find.
                        if (! itr->second(pos, argc, argv)) return false; // option string function error.
                } else { //option string function not found.
                        // print option not found message.
                        std::string buf = "Invalid option for -S command.";
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 128, buf, __FILE__, __LINE__);
                        return false;
                }
        }

        return true;

}
//! snmp refresh set function
//! @param[in]    argument position
//! @param[in]    argument count
//! @param[in]    argument value
bool    l7vs::l7vsadm::parse_opt_snmp_refresh_func(int &pos, int argc, char *argv[])
{
        Logger    logger(LOG_CAT_L7VSADM_COMMON, 42, "l7vsadm::parse_opt_snmp_refresh_func", __FILE__, __LINE__);
        if (request.snmpinfo.option_set_flag & snmp_info::SNMP_REFRESH_ALL_OPTION_FLAG
            || request.snmpinfo.option_set_flag & snmp_info::SNMP_REFRESH_OPTION_FLAG) {
                // double target commands.
                std::stringstream buf;
                buf << "Option ";
                buf << argv[pos];
                buf << " is conflict.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 129, buf.str(), __FILE__, __LINE__);
                return false;
        }

        if (pos != 2) {
                // don't target logcategory
                std::string    buf("Invalid option for -S command.");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 130, buf, __FILE__, __LINE__);
                return false;
        }

        if (argc == 3) {
                request.snmpinfo.option_set_flag |= snmp_info::SNMP_REFRESH_ALL_OPTION_FLAG;
                return true;
        } else if (argc == 7) {
                for (pos = 3; pos < argc; ++pos) {
                        parse_opt_map_type::iterator itr = snmp_vs_option_dic.find(argv[pos]);
                        if (itr != snmp_vs_option_dic.end()) {
                                if (!itr->second(pos, argc, argv)) {
                                        return false;
                                }
                        } else {
                                std::string    buf("Invalid option value for -r option.");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 78, buf, __FILE__, __LINE__);
                                return false;
                        }
                }
                request.snmpinfo.option_set_flag |= snmp_info::SNMP_REFRESH_OPTION_FLAG;

        } else {
                std::string    buf("Argument argc is illegal for -S command.");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 79, buf, __FILE__, __LINE__);
                return false;
        }

        return true;
}

//! snmp virtualservice set function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool    l7vs::l7vsadm::parse_opt_snmp_vs_target_func(int &pos, int argc, char *argv[])
{
        Logger    logger(LOG_CAT_L7VSADM_COMMON, 43, "l7vsadm::parse_opt_vs_target_func", __FILE__, __LINE__);
        if (request.snmpinfo.option_set_flag & snmp_info::SNMP_TCP_SERVICE_OPTION_FLAG) {
                std::stringstream buf;
                buf << "Option ";
                buf << argv[pos];
                buf << " is conflict.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 131, buf.str(), __FILE__, __LINE__);
                return false;
        }

        if (++pos >= argc) {
                //don't target recvaddress:port
                std::string    buf("target endpoint is not specified.");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 132, buf, __FILE__, __LINE__);
                return false;
        }

        // get host endpoint from string
        std::string
        src_str = argv[pos];
        error_code    err;
        boost::asio::ip::tcp::endpoint tmp_endpoint;
        tmp_endpoint = string_to_endpoint<boost::asio::ip::tcp>(src_str, err);
        if (err) {
                std::stringstream buf;
                buf << "target endpoint parse error:" << err.get_message() << src_str;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 133, buf.str(), __FILE__, __LINE__);
                return false;
        }

        request.snmpinfo.vs_endpoint = tmp_endpoint;
        request.snmpinfo.option_set_flag |= snmp_info::SNMP_TCP_SERVICE_OPTION_FLAG;


        return true;
}
//! snmp module set function
//! @param[in]    argument position
//! @param[in]    argument count
//! @param[in]    argument value
bool    l7vs::l7vsadm::parse_opt_snmp_vs_module_func(int &pos, int argc, char *argv[])
{
        Logger    logger(LOG_CAT_L7VSADM_COMMON, 44, "l7vsadm::parse_opt_snmp_vs_module_func", __FILE__, __LINE__);

        if (request.snmpinfo.option_set_flag & snmp_info::SNMP_PROTOCOL_MODULE_OPTION_FLAG) {
                std::stringstream buf;
                buf << "Option ";
                buf << argv[pos];
                buf << " is conflict.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 134, buf.str(), __FILE__, __LINE__);
                return false;
        }

        if (++pos >= argc) {
                //don't target protomod name.
                std::string    buf("protomod name is not specified.");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 135, buf, __FILE__, __LINE__);
                return false;
        }

        std::string    module_name = argv[pos];
        if (L7VS_MODNAME_LEN < module_name.length()) {
                std::string    buf("protomod name is too long.");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 136, buf, __FILE__, __LINE__);
                return false;
        }

        protocol_module_control    &ctrl = protocol_module_control::getInstance();
        ctrl.initialize(L7VS_MODULE_PATH);
        protocol_module_base *module;

        module = ctrl.load_module(module_name);
        if (!module) {
                //don't find protocol module.
                std::stringstream buf;
                buf << "protocol module not found:" << module_name;
                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 137, buf.str(), __FILE__, __LINE__);
                return false;
        }


        ctrl.unload_module(module);

        request.snmpinfo.option_set_flag |= snmp_info::SNMP_PROTOCOL_MODULE_OPTION_FLAG ;
        request.snmpinfo.protocol = module_name;


        return true;
}
//! snmp flag set function
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool    l7vs::l7vsadm::parse_opt_snmp_flag_func(int &pos, int argc, char *argv[])
{
        int tmp = 0;
        Logger    logger(LOG_CAT_L7VSADM_COMMON, 45, "l7vsadm::parse_opt_snmp_flag_func", __FILE__, __LINE__);
        if (request.snmpinfo.option_set_flag & snmp_info::SNMP_ENABLE_OPTION_FLAG) {
                std::stringstream buf;
                buf << "Option ";
                buf << argv[pos];
                buf << " is conflict.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 138, buf.str(), __FILE__, __LINE__);
                return false;
        }
        if (++pos < argc) {
                try {
                        tmp = boost::lexical_cast< int >(argv[pos]);
                } catch (const boost::bad_lexical_cast &ex) {
                        std::string    buf("Invalid option value for -f option.");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 139, buf, __FILE__, __LINE__);
                        return false;
                }

                if (tmp == 0 || tmp == 1) {

                        request.snmpinfo.enabled = tmp;
                        request.snmpinfo.option_set_flag |= snmp_info::SNMP_ENABLE_OPTION_FLAG;

                        return true;
                } else {
                        std::string    buf("Invalid option value for -f option.");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 140, buf, __FILE__, __LINE__);
                        return false;
                }
        } else {
                std::string    buf("Need option value for -f option.");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 141, buf, __FILE__, __LINE__);
                return false;
        }
}
//! snmp interval set function
//! @param[in]    argument position
//! @param[in]    argument count
//! @param[in]    argument value
bool    l7vs::l7vsadm::parse_opt_snmp_interval_func(int &pos, int argc, char *argv[])
{
        int tmp = 0;
        Logger    logger(LOG_CAT_L7VSADM_COMMON, 46, "l7vs::l7vsadm::parse_opt_snmp_interval_func", __FILE__, __LINE__);

        if (request.snmpinfo.option_set_flag & snmp_info::SNMP_INTERVAL_OPTION_FLAG) {
                std::stringstream buf;
                buf << "Option ";
                buf << argv[pos];
                buf << " is conflict.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 142, buf.str(), __FILE__, __LINE__);
                return false;
        }

        if (++pos < argc) {
                try {
                        tmp = boost::lexical_cast< int >(argv[pos]);
                } catch (const boost::bad_lexical_cast &) {
                        std::string    buf("Invalid option value for -i option.");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 143, buf, __FILE__, __LINE__);
                        return false;
                }

                if (tmp >= 0) {
                        request.snmpinfo.interval = tmp;
                        request.snmpinfo.option_set_flag |= snmp_info::SNMP_INTERVAL_OPTION_FLAG;

                        return true;
                } else {
                        std::string    buf("Invalid option value for -i option.");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 144, buf, __FILE__, __LINE__);
                        return false;
                }
        } else    {
                std::string    buf("Need option value for -i option.");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 145, buf, __FILE__, __LINE__);
                return false;
        }
}

//! snmp logtrap set function
//! @param[in]    argument position
//! @param[in]    argument count
//! @param[in]    argument value
bool    l7vs::l7vsadm::parse_opt_snmp_log_trap_func(int &pos, int argc, char *argv[])
{
        int tmp = 0;
        Logger    logger(LOG_CAT_L7VSADM_COMMON, 47, "l7vs::l7vsadm::parse_opt_snmp_log_trap_func", __FILE__, __LINE__);

        if (request.snmpinfo.option_set_flag & snmp_info::SNMP_LOGTRAP_OPTION_FLAG) {
                std::stringstream buf;
                buf << "Option ";
                buf << argv[pos];
                buf << " is conflict.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 146, buf.str(), __FILE__, __LINE__);
                return false;
        }
        if (++pos < argc) {
                try {
                        tmp = boost::lexical_cast< int >(argv[pos]);
                } catch (const boost::bad_lexical_cast &) {
                        std::string    buf("Invalid option value for -t option.");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 147, buf, __FILE__, __LINE__);
                        return false;
                }
                if (tmp == 0 || tmp == 1) {

                        request.snmpinfo.logtrap_enabled = tmp;
                        request.snmpinfo.option_set_flag |= snmp_info::SNMP_LOGTRAP_OPTION_FLAG;


                        return true;
                } else {
                        std::string    buf("Invalid option value for -t option.");
                        l7vsadm_err.setter(true, buf);
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 148, buf, __FILE__, __LINE__);
                        return false;
                }
        } else    {
                std::string    buf("Need option value for -t option.");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 149, buf, __FILE__, __LINE__);
                return false;
        }

}

//! snmp logtrap_level set function
//! @param[in]    argument position
//! @param[in]    argument count
//! @param[in]    argument value
bool    l7vs::l7vsadm::parse_opt_snmp_log_trap_level_func(int &pos, int argc, char *argv[])
{
        Logger    logger(LOG_CAT_L7VSADM_COMMON, 48, "l7vs::l7vsadm::parse_opt_snmp_log_trap_level_func", __FILE__, __LINE__);

        if (request.snmpinfo.option_set_flag & snmp_info::SNMP_LOGTRAP_LEVEL_OPTION_FLAG) {
                std::stringstream buf;
                buf << "Option ";
                buf << argv[pos];
                buf << " is conflict.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 150, buf.str(), __FILE__, __LINE__);
                return false;
        }

        if (++pos < argc) {
                string_loglevel_map_type::iterator itr = string_loglevel_dic.find(argv[pos]);
                if (itr != string_loglevel_dic.end()) {
                        request.snmpinfo.logtrap_level = itr->second;
                        request.snmpinfo.option_set_flag |= snmp_info::SNMP_LOGTRAP_LEVEL_OPTION_FLAG;

                        return true;
                } else {
                        std::stringstream    buf;
                        buf << "No such Log-Level " << argv[pos] << ".";
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 151, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        } else {
                std::string    buf("Need option value for -l option.");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 152, buf, __FILE__, __LINE__);
                return false;

        }

}

//! parameter command parsing
//! @param[in] request command
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_parameter_func(l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 28, "l7vsadm::parse_parameter_func", __FILE__, __LINE__);

        if (argc != 4) {
                //argument num err
                std::stringstream buf;
                buf << "Argument argc is illegal for ";
                buf << argv[1];
                buf << " command.";

                l7vsadm_err.setter(true, buf.str());
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 126, buf.str(), __FILE__, __LINE__);
                return false;
        }

        request.command = cmd;

        for (int pos = 2; pos < argc; ++pos) {
                parse_opt_map_type::iterator itr = parameter_option_dic.find(argv[pos]);
                if (itr != parameter_option_dic.end()) { // option string function find.
                        if (! itr->second(pos, argc, argv)) return false; // option string function error.
                } else { //option string function not found.
                        // print option not found message.
                        std::stringstream buf;
                        buf << "parameter option not found(--parameter): " << argv[pos];
                        l7vsadm_err.setter(true, buf.str());
                        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 84, buf.str(), __FILE__, __LINE__);
                        return false;
                }
        }

        if (PARAM_COMP_NOCAT == request.reload_param) {
                // not specified reload_param
                std::string buf("reload component is not specified.(--reload)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 85, buf, __FILE__, __LINE__);
                return false;
        }

        return true;
}
//
// parameter command
//
//! parameter reload component parsing
//! @param[in] argument position
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_opt_parameter_reload_func(int &pos, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 29, "l7vsadm::parse_opt_parameter_reload_func", __FILE__, __LINE__);

        if (++pos >= argc) {
                // reload component is not specified
                std::string buf("reload component is not specified.(--reload)");
                l7vsadm_err.setter(true, buf);
                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 86, buf, __FILE__, __LINE__);
                return false;
        }
        string_parameter_map_type::iterator itr = string_parameter_dic.find(argv[pos]);
        if (itr != string_parameter_dic.end()) {
                request.reload_param = itr->second;
                return true;
        }
        std::stringstream buf;
        buf << "reload component not found(--reload): " << argv[pos];
        l7vsadm_err.setter(true, buf.str());
        Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 87, buf.str(), __FILE__, __LINE__);
        return false;
}

//! help command parsing
//! @param[in] request command
//! @param[in] argument count
//! @param[in] argument value
bool l7vs::l7vsadm::parse_help_func(l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd, int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 30, "l7vsadm::parse_help_func", __FILE__, __LINE__);

        request.command = cmd;

        std::cout << usage();

        std::cout <<
                  "Commands:\n"
                  "  --add-service      -A        add virtual service with options\n"
                  "  --edit-service     -E        edit virtual service with options\n"
                  "  --delete-service   -D        delete virtual service with options\n"
                  "  --flush            -C        flush virtual service\n"
                  "  --add-server       -a        add real server with options\n"
                  "  --edit-server      -e        edit real server with options\n"
                  "  --delete-server    -d        delete real server with options\n"
                  "  --replication      -R        control replication-function\n"
                  "  --log              -L        control logger-function\n"
                  "  --snmp             -S        control SNMP Agent-function\n"
                  "  --parameter        -P        control parameter-function\n"
                  "  --list             -l        list the table\n"
                  "  --verbose          -V        list the table in verbose format\n"
                  "  --key              -K        list the table in key setting format\n"
                  "  --help             -h        show usage\n"
                  << std::endl;

        std::cout <<
                  "Options:\n"
                  "  --tcp-service      -t service-address     service-address is host:port\n"
                  "  --proto-module     -m proto-module        protocol module name and module argument\n"
                  "                        [module-args]\n"
                  "  --scheduler        -s scheduler           one of rr,lc,wrr\n"
                  "  --upper            -u connection-count    maximum number of connections\n"
                  "  --bypass           -b sorry-server        sorry server address is host:port\n"
                  "  --tproxy                                  set sorry server connection to IP transparent mode.\n"
                  "  --masq                                    set sorry server connection to IP masquerade mode.\n"

                  "  --flag             -f sorry-flag          sorry status set to virtual service\n"
                  "  --qos-up           -Q QoSval-up           QoS Threshold(bps) set to real server direction\n"
                  "  --qos-down         -q QoSval-down         QoS Threshold(bps) set to client direction\n"
                  "  --ssl              -z ssl-config-file     SSL configuration file(Use SSL)\n"
                  "  --sockopt          -O socket-option       deferaccept,nodelay,cork,quickackon or quickackoff set to socket option\n"
                  "  --access-log       -L access-log-flag     access log flag 0(none) or 1(output)\n"
                  "  --access-log-name  -a access-log-file     access log file\n"
                  "                        [logrotate-args]\n"
                  "  --real-server      -r server-address      server-address is host:port\n"
                  "  --weight           -w weight              scheduling weight set to real server\n"
                  "  --tproxy                                  set real server connection to IP transparent mode.\n"
                  "  --masq                                    set real server connection to IP masquerade mode.\n"
                  "  --switch           -s replication-switch  start or stop replication\n"
                  "  --force            -f                     force replication start\n"
                  "  --dump             -d                     dump replication memory\n"
                  "  --category         -c log-category        set log category for l7vsd or SNMP Agent\n"
                  "  --level            -l log-level           set log level for l7vsd or SNMP Agent\n"
                  "  --reload           -r reload-parameter    reload specified config parameter\n"
                  "  --numeric          -n                     list the table in numeric\n"
                  "  --flag             -f snmp-flag           start or stop snmp function 0(off) 1(on)\n"
                  "  --interval         -i update-interval     set collect snmp cache collect interval(s)\n"
                  "  --logtrap          -t log-trap-flag       start or stop log trap function 0(off) 1(on)\n"
                  "  --logtraplevel     -l log-trap-level      set log trap level for snmp\n"
                  "  --refresh          -r                     clear statistic info for snmp\n"
                  << std::endl;

        return true;
}

//! usage function.
std::string l7vs::l7vsadm::usage()
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 31, "l7vsadm::usage", __FILE__, __LINE__);

        std::stringstream stream;
        stream <<
               "Usage: \n"
               "  l7vsadm -A -t service-address -m proto-module [module-args]\n"
               "          [-s scheduler] [-u connection-count] [-b sorry-server] [--masq|tproxy]\n"
               "          [-f sorry-flag] [-Q QoSval-up] [-q QoSval-down] [-z ssl-config-file]\n"
               "          [-O socket-option] [-L access-log-flag] [-a access-log-file [logrotate-args]]\n"
               "  l7vsadm -E -t service-address -m proto-module [module-args]\n"
               "          [-s scheduler] [-u connection-count] [-b sorry-server] [--masq|tproxy]\n"
               "          [-f sorry-flag] [-Q QoSval-up] [-q QoSval-down] [-L access-log-flag]\n"
               "  l7vsadm -D -t service-address -m proto-module [module-args]\n"
               "  l7vsadm -C\n"
               "  l7vsadm -a|e -t service-address -m proto-module [module-args]\n"
               "          -r server-address [-w weight] [--masq|tproxy]\n"
               "  l7vsadm -d -t service-address -m proto-module [module-args]\n"
               "          -r server-address\n"
               "  l7vsadm -R -s replication-switch\n"
               "  l7vsadm -R -f\n"
               "  l7vsadm -R -d\n"
               "  l7vsadm -L -c log-category -l log-level\n"
               "  l7vsadm -S [-f snmp-flag] [-i update-interval] [-t log-trap-flag] [-l log-trap-level]\n"
               "  l7vsadm -S -r [-t service-address -m proto-module]\n"
               "  l7vsadm -P -r reload-parameter\n"
               "  l7vsadm -l [-n]\n"
               "  l7vsadm -V [-n]\n"
               "  l7vsadm -K [-n]\n"
               "  l7vsadm -h\n"
               << std::endl;
        return stream.str();
}

//! disp_list function
void l7vs::l7vsadm::disp_list()
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 32, "l7vsadm::disp_list", __FILE__, __LINE__);

        std::stringstream buf;
        buf << boost::format("Layer-7 Virtual Server version %s\n") % VERSION;
        buf << "Prot LocalAddress:Port ProtoMod Scheduler\n";
        buf << "  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn\n";
        BOOST_FOREACH(virtualservice_element vse, response.virtualservice_status_list) {
                std::string vsepstr;
                if (vse.udpmode) {
                        vsepstr = endpoint_to_string<boost::asio::ip::udp>(vse.udp_recv_endpoint, numeric_flag);
                } else {
                        vsepstr = endpoint_to_string<boost::asio::ip::tcp>(vse.tcp_accept_endpoint, numeric_flag);
                }
                buf << boost::format("%s %s %s %s\n")
                    % (vse.udpmode ? "UDP" : "TCP")
                    % vsepstr
                    % vse.protocol_module_name
                    % vse.schedule_module_name;
                BOOST_FOREACH(realserver_element rse, vse.realserver_vector) {
                        std::string rsepstr;
                        if (vse.udpmode) {
                                rsepstr = endpoint_to_string<boost::asio::ip::udp>(rse.udp_endpoint, numeric_flag);
                        } else {
                                rsepstr = endpoint_to_string<boost::asio::ip::tcp>(rse.tcp_endpoint, numeric_flag);
                        }
                        buf << boost::format("  -> %-28s %-7s %-6d %-10d %-10d\n")
                            % rsepstr
                            % rse.get_fwdmode_str()
                            % rse.weight
                            % rse.get_active()
                            % rse.get_inact();
                }
        }
        std::cout << buf.str();
}

//! disp_list_key function
void l7vs::l7vsadm::disp_list_key()
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 33, "l7vsadm::disp_list_key", __FILE__, __LINE__);

        std::stringstream buf;
        buf << boost::format("Layer-7 Virtual Server version %s\n") % VERSION;
        buf << "Prot LocalAddress:Port ProtoMod Scheduler\n";
        buf << "     SSL_config_file\n";
        buf << "     Socket option\n";
        buf << "     Access_log_flag\n";
        buf << "     Access_log_file\n";
        buf << "     Access_log_rotate option\n";
        buf << "  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn\n";
        BOOST_FOREACH(virtualservice_element vse, response.virtualservice_status_list) {
                std::string vsepstr;
                if (vse.udpmode) {
                        vsepstr = endpoint_to_string<boost::asio::ip::udp>(vse.udp_recv_endpoint, numeric_flag);
                } else {
                        vsepstr = endpoint_to_string<boost::asio::ip::tcp>(vse.tcp_accept_endpoint, numeric_flag);
                }
                buf << boost::format("%s %s %s %s\n")
                    % (vse.udpmode ? "UDP" : "TCP")
                    % vsepstr
                    % vse.protocol_module_name
                    % vse.schedule_module_name;
                buf << boost::format("    %s\n")
                    % ((0 == vse.ssl_file_name.length()) ? "none" : vse.ssl_file_name);
                buf << boost::format("    %s\n")
                    % ((0 == vse.socket_option_string.length()) ? "none" : vse.socket_option_string);
                buf << boost::format("    %d\n") % vse.access_log_flag;
                buf << boost::format("    %s\n")
                    % ((0 == vse.access_log_file_name.length()) ? "none" : vse.access_log_file_name);
                buf << boost::format("    %s\n")
                    % ((0 == vse.access_log_rotate_key_info.length()) ? "none" :  vse.access_log_rotate_key_info);

                BOOST_FOREACH(realserver_element rse, vse.realserver_vector) {
                        std::string    rsepstr;
                        if (vse.udpmode)
                                rsepstr = endpoint_to_string<boost::asio::ip::udp>(rse.udp_endpoint, numeric_flag);
                        else
                                rsepstr = endpoint_to_string<boost::asio::ip::tcp>(rse.tcp_endpoint, numeric_flag);
                        buf << boost::format("  -> %-28s %-7s %-6d %-10d %-10d\n")
                            % rsepstr
                            % rse.get_fwdmode_str()
                            % rse.weight
                            % rse.get_active()
                            % rse.get_inact();
                }
        }
        std::cout << buf.str();
}

//! disp_list_verbose function
void l7vs::l7vsadm::disp_list_verbose()
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 34, "l7vsadm::disp_list_verbose", __FILE__, __LINE__);

        unsigned long long output_qos_upstream_value;
        unsigned long long output_qos_downstream_value;
        const int MAX_TIME_FORMAT_LEN = 20;
        char snmp_start_date[MAX_TIME_FORMAT_LEN] = {0};
        char snmp_last_request_date[MAX_TIME_FORMAT_LEN] = {0};
        char snmp_last_trap_date[MAX_TIME_FORMAT_LEN] = {0};

        std::stringstream buf;
        buf << boost::format("Layer-7 Virtual Server version %s\n") % VERSION;

        //disp loglevel
        buf << "L7vsd Log Level:\n";
        buf << "Category                       Level\n";
        typedef std::pair<LOG_CATEGORY_TAG, LOG_LEVEL_TAG> logstatus_type;
        BOOST_FOREACH(logstatus_type logstatus, response.log_status_list) {
                buf << boost::format("%-30s %s\n")
                    % logcategory_string_dic[logstatus.first]
                    % loglevel_string_dic[logstatus.second];
        }
        buf << "\n";

        //disp replication
        buf << "Replication Mode:\n";
        buf << boost::format("%s\n") % replication_mode_string_dic[response.replication_mode_status];
        buf << "\n";

        //disp snmp agent status
        buf << "SNMPAgent:\n";
        if (!response.snmpinfo.enabled) {
                boost::format fmtter("%-30s inactive\n"
                                     "%-30s none\n"
                                     "%-30s none\n"
                                     "%-30s none\n"
                                     "%-30s none\n"
                                     "%-30s none\n"
                                     "%-30s none\n"
                                     "%-30s none\n"
                                     "%-30s none\n"
                                     "%-30s none\n");

                fmtter % "Agent Status";
                fmtter % "log trap status";
                fmtter % "log trap level";
                fmtter % "cache update interval";
                fmtter % "start date";
                fmtter % "last request date";
                fmtter % "last trap date";
                fmtter % "total GET requests";
                fmtter % "total SET requests";
                fmtter % "total trap counts";

                buf << fmtter.str();
        } else {
                strftime(snmp_start_date, sizeof(snmp_start_date), "%Y-%m-%d %H:%M:%S", localtime(&response.snmpinfo.start_date));
                strftime(snmp_last_request_date, sizeof(snmp_start_date), "%Y-%m-%d %H:%M:%S", localtime(&response.snmpinfo.request_last_date));
                strftime(snmp_last_trap_date, sizeof(snmp_start_date), "%Y-%m-%d %H:%M:%S", localtime(&response.snmpinfo.trap_last_date));

                boost::format fmtter("%-30s active\n"
                                     "%-30s %s\n"
                                     "%-30s %s\n"
                                     "%-30s %d\n"
                                     "%-30s %s\n"
                                     "%-30s %s\n"
                                     "%-30s %s\n"
                                     "%-30s %u\n"
                                     "%-30s %u\n"
                                     "%-30s %u\n");

                fmtter % "Agent Status";
                fmtter % "log trap status";
                fmtter % (response.snmpinfo.logtrap_enabled ? "on" : "off");
                fmtter % "log trap level";
                fmtter % (response.snmpinfo.logtrap_enabled ? loglevel_string_dic[response.snmpinfo.logtrap_level] : "none");
                fmtter % "cache update interval" % response.snmpinfo.interval;
                fmtter % "start date" % (response.snmpinfo.start_date == 0 ? "none" : snmp_start_date);
                fmtter % "last request date" % (response.snmpinfo.request_last_date == 0 ? "none" : snmp_last_request_date);
                fmtter % "last trap date" % (response.snmpinfo.trap_last_date == 0 ? "none" : snmp_last_trap_date);
                fmtter % "total GET requests" % response.snmpinfo.snmp_get_requests;
                fmtter % "total SET requests" % response.snmpinfo.snmp_set_requests;
                fmtter % "total trap counts" % response.snmpinfo.snmp_trap_count;
                buf << fmtter.str();
        }


        buf << "\n";

        // disp vs
        buf << "Prot LocalAddress:Port ProtoMod Scheduler Protomod_opt_string\n";
        buf << "  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn\n";
        BOOST_FOREACH(virtualservice_element vse, response.virtualservice_status_list) {
                std::string vsepstr;
                if (vse.udpmode) {
                        vsepstr = endpoint_to_string<boost::asio::ip::udp>(vse.udp_recv_endpoint, numeric_flag);
                } else {
                        vsepstr = endpoint_to_string<boost::asio::ip::tcp>(vse.tcp_accept_endpoint, numeric_flag);
                }

                if (vse.qos_upstream == ULLONG_MAX) {
                        output_qos_upstream_value = 0;
                } else {
                        output_qos_upstream_value = vse.qos_upstream * 8;
                }

                if (vse.qos_downstream == ULLONG_MAX) {
                        output_qos_downstream_value = 0;
                } else {
                        output_qos_downstream_value = vse.qos_downstream * 8;
                }

                buf << boost::format("%s %s %s %s %s\n")
                    % (vse.udpmode ? "UDP" : "TCP")
                    % vsepstr
                    % vse.protocol_module_name
                    % vse.schedule_module_name
                    % vse.protocol_module_for_indication_options;

                BOOST_FOREACH(realserver_element rse, vse.realserver_vector) {
                        std::string rsepstr;
                        if (vse.udpmode) {
                                rsepstr = endpoint_to_string<boost::asio::ip::udp>(rse.udp_endpoint, numeric_flag);
                        } else {
                                rsepstr = endpoint_to_string<boost::asio::ip::tcp>(rse.tcp_endpoint, numeric_flag);
                        }
                        buf << boost::format("  -> %-28s %-7s %-6d %-10d %-10d\n")
                            % rsepstr
                            % rse.get_fwdmode_str()
                            % rse.weight
                            % rse.get_active()
                            % rse.get_inact();
                }

                if (!vse.udpmode) {
                        std::string    sorryepstr;
                        std::string    sorry_flag_str;
                        boost::asio::ip::tcp::endpoint    zeropoint;
                        if (zeropoint == vse.sorry_endpoint) {
                                sorryepstr = "none";
                        } else {
                                sorryepstr = endpoint_to_string<boost::asio::ip::tcp>(vse.sorry_endpoint,
                                                numeric_flag) + " " + vse.get_fwdmode_str();
                        }

                        if (vse.sorry_flag) {
                                sorry_flag_str = "on";
                        } else {
                                sorry_flag_str = "off";
                        }

                        buf << boost::format("  Bypass Settings:\n"
                                             "    Sorry Server                  %s\n"
                                             "    Max Connection                %lld\n"
                                             "    Sorry Flag                    %s\n")
                            % sorryepstr
                            % vse.sorry_maxconnection
                            % sorry_flag_str;
                }

                buf << boost::format("  SSL Settings:\n"
                                     "    SSL Config File               %s\n")
                    % ((0 == vse.ssl_file_name.length()) ? "none" : vse.ssl_file_name);

                buf << boost::format("  Logging Settings:\n"
                                     "    Access Log                    %s\n"
                                     "    Access Log File               %s\n"
                                     "    Access Log Rotate             %s\n")
                    % ((0 == vse.access_log_flag) ? "off" : "on")
                    % ((0 == vse.access_log_file_name.length()) ? "none" : vse.access_log_file_name)
                    % ((0 == vse.access_log_rotate_verbose_info.length()) ? "none" :  vse.access_log_rotate_verbose_info);

                buf << boost::format("  Socket Settings:\n"
                                     "    TCP_DEFER_ACCEPT              %s\n"
                                     "    TCP_NODELAY                   %s\n"
                                     "    TCP_CORK                      %s\n"
                                     "    TCP_QUICKACK                  %s\n")
                    % ((0 == vse.socket_option_tcp_defer_accept) ? "disable" : "enable")
                    % ((0 == vse.socket_option_tcp_nodelay) ? "disable" : "enable")
                    % ((0 == vse.socket_option_tcp_cork) ? "disable" : "enable")
                    % ((0 == vse.socket_option_tcp_quickack) ? "auto" : ((1 == vse.socket_option_tcp_quickack) ? "enable" : "disable"));


                buf << boost::format("  Throughput:\n"
                                     "    Current Upload / Limit        %f Mbps / %f Mbps\n"
                                     "    Current Download / Limit      %f Mbps / %f Mbps\n")
                    % ((double)vse.throughput_upstream * 8 / (1000 * 1000)) % ((double)output_qos_upstream_value / (1000 * 1000))
                    % ((double)vse.throughput_downstream * 8 / (1000 * 1000)) % ((double)output_qos_downstream_value / (1000 * 1000));

                buf << boost::format("  Statistics:\n"
                                     "    HTTP Total Requests           %lld\n"
                                     "    HTTP GET Requests             %lld\n"
                                     "    HTTP POST Requests            %lld\n")
                    % vse.http_total_count % vse.http_get_count % vse.http_post_count;


        }
        std::cout << buf.str();
}
//! l7vsadm constructor.
//! create including all dictionary.
l7vs::l7vsadm::l7vsadm()
        :   numeric_flag(false),
            command_wait_interval(L7VSADM_DEFAULT_WAIT_INTERVAL),
            command_wait_count(L7VSADM_DEFAULT_WAIT_COUNT)
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 35, "l7vsadm::l7vsadm(constructor)", __FILE__, __LINE__);

        // create command dictionary.
        command_dic["-l"]               = boost::bind(&l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST, _1, _2);
        command_dic["--list"]           = boost::bind(&l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST, _1, _2);
        command_dic["-V"]               = boost::bind(&l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST_VERBOSE, _1, _2);
        command_dic["--verbose"]        = boost::bind(&l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST_VERBOSE, _1, _2);
        command_dic["-K"]               = boost::bind(&l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST_KEY, _1, _2);
        command_dic["--key"]            = boost::bind(&l7vsadm::parse_list_func, this, l7vsadm_request::CMD_LIST_KEY, _1, _2);
        command_dic["-A"]               = boost::bind(&l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_ADD_VS, _1, _2);
        command_dic["--add-service"]    = boost::bind(&l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_ADD_VS, _1, _2);
        command_dic["-D"]               = boost::bind(&l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_DEL_VS, _1, _2);
        command_dic["--delete-service"] = boost::bind(&l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_DEL_VS, _1, _2);
        command_dic["-E"]               = boost::bind(&l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_EDIT_VS, _1, _2);
        command_dic["--edit-service"]   = boost::bind(&l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_EDIT_VS, _1, _2);
        command_dic["-C"]               = boost::bind(&l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_FLUSH_VS, _1, _2);
        command_dic["--flush"]          = boost::bind(&l7vsadm::parse_vs_func, this, l7vsadm_request::CMD_FLUSH_VS, _1, _2);
        command_dic["-a"]               = boost::bind(&l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_ADD_RS, _1, _2);
        command_dic["--add-server"]     = boost::bind(&l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_ADD_RS, _1, _2);
        command_dic["-d"]               = boost::bind(&l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_DEL_RS, _1, _2);
        command_dic["--delete-server"]  = boost::bind(&l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_DEL_RS, _1, _2);
        command_dic["-e"]               = boost::bind(&l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_EDIT_RS, _1, _2);
        command_dic["--edit-server"]    = boost::bind(&l7vsadm::parse_rs_func, this, l7vsadm_request::CMD_EDIT_RS, _1, _2);
        command_dic["-R"]               = boost::bind(&l7vsadm::parse_replication_func, this, l7vsadm_request::CMD_REPLICATION, _1, _2);
        command_dic["--replication"]    = boost::bind(&l7vsadm::parse_replication_func, this, l7vsadm_request::CMD_REPLICATION, _1, _2);
        command_dic["-L"]               = boost::bind(&l7vsadm::parse_log_func, this, l7vsadm_request::CMD_LOG, _1, _2);
        command_dic["--log"]            = boost::bind(&l7vsadm::parse_log_func, this, l7vsadm_request::CMD_LOG, _1, _2);
        command_dic["-S"]               = boost::bind(&l7vsadm::parse_snmp_func, this, l7vsadm_request::CMD_SNMP, _1, _2);
        command_dic["--snmp"]           = boost::bind(&l7vsadm::parse_snmp_func, this, l7vsadm_request::CMD_SNMP, _1, _2);
        command_dic["-P"]               = boost::bind(&l7vsadm::parse_parameter_func, this, l7vsadm_request::CMD_PARAMETER, _1, _2);
        command_dic["--parameter"]      = boost::bind(&l7vsadm::parse_parameter_func, this, l7vsadm_request::CMD_PARAMETER, _1, _2);
        command_dic["-h"]               = boost::bind(&l7vsadm::parse_help_func, this, l7vsadm_request::CMD_HELP, _1, _2);
        command_dic["--help"]           = boost::bind(&l7vsadm::parse_help_func, this, l7vsadm_request::CMD_HELP, _1, _2);

        // create list option dictionary.
        list_option_dic["-n"]        = boost::bind(&l7vsadm::parse_opt_list_numeric_func, this, _1, _2, _3);
        list_option_dic["--numeric"] = boost::bind(&l7vsadm::parse_opt_list_numeric_func, this, _1, _2, _3);

        // create virtualservice option dictionary
        vs_option_dic["-t"]                = boost::bind(&l7vsadm::parse_opt_vs_target_func, this, _1, _2, _3);
        vs_option_dic["--tcp-service"]     = boost::bind(&l7vsadm::parse_opt_vs_target_func, this, _1, _2, _3);
        vs_option_dic["-m"]                = boost::bind(&l7vsadm::parse_opt_vs_module_func, this, _1, _2, _3);
        vs_option_dic["--proto-module"]    = boost::bind(&l7vsadm::parse_opt_vs_module_func, this, _1, _2, _3);
        vs_option_dic["-s"]                = boost::bind(&l7vsadm::parse_opt_vs_scheduler_func, this, _1, _2, _3);
        vs_option_dic["--scheduler"]       = boost::bind(&l7vsadm::parse_opt_vs_scheduler_func, this, _1, _2, _3);
        vs_option_dic["-u"]                = boost::bind(&l7vsadm::parse_opt_vs_upper_func, this, _1, _2, _3);
        vs_option_dic["--upper"]           = boost::bind(&l7vsadm::parse_opt_vs_upper_func, this, _1, _2, _3);
        vs_option_dic["-b"]                = boost::bind(&l7vsadm::parse_opt_vs_bypass_func, this, _1, _2, _3);
        vs_option_dic["--bypass"]          = boost::bind(&l7vsadm::parse_opt_vs_bypass_func, this, _1, _2, _3);
        vs_option_dic["--tproxy"]          = boost::bind(&l7vsadm::parse_opt_vs_fwdmode_func, this, _1, _2, _3);
        vs_option_dic["--masq"]            = boost::bind(&l7vsadm::parse_opt_vs_fwdmode_func, this, _1, _2, _3);
        vs_option_dic["-f"]                = boost::bind(&l7vsadm::parse_opt_vs_flag_func, this, _1, _2, _3);
        vs_option_dic["--flag"]            = boost::bind(&l7vsadm::parse_opt_vs_flag_func, this, _1, _2, _3);
        vs_option_dic["-Q"]                = boost::bind(&l7vsadm::parse_opt_vs_qosup_func, this, _1, _2, _3);
        vs_option_dic["--qos-up"]          = boost::bind(&l7vsadm::parse_opt_vs_qosup_func, this, _1, _2, _3);
        vs_option_dic["-q"]                = boost::bind(&l7vsadm::parse_opt_vs_qosdown_func, this, _1, _2, _3);
        vs_option_dic["--qos-down"]        = boost::bind(&l7vsadm::parse_opt_vs_qosdown_func, this, _1, _2, _3);
        vs_option_dic["-p"]                = boost::bind(&l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3);
        vs_option_dic["--udp"]             = boost::bind(&l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3);
        vs_option_dic["-z"]                = boost::bind(&l7vsadm::parse_opt_vs_ssl_file_func, this, _1, _2, _3);
        vs_option_dic["--ssl"]             = boost::bind(&l7vsadm::parse_opt_vs_ssl_file_func, this, _1, _2, _3);
        vs_option_dic["-O"]                = boost::bind(&l7vsadm::parse_opt_vs_socket_func, this, _1, _2, _3);
        vs_option_dic["--sockopt"]         = boost::bind(&l7vsadm::parse_opt_vs_socket_func, this, _1, _2, _3);
        vs_option_dic["-L"]                = boost::bind(&l7vsadm::parse_opt_vs_access_log_func, this, _1, _2, _3);
        vs_option_dic["--access-log"]      = boost::bind(&l7vsadm::parse_opt_vs_access_log_func, this, _1, _2, _3);
        vs_option_dic["-a"]                = boost::bind(&l7vsadm::parse_opt_vs_access_log_logrotate_func, this, _1, _2, _3);
        vs_option_dic["--access-log-name"] = boost::bind(&l7vsadm::parse_opt_vs_access_log_logrotate_func, this, _1, _2, _3);

        // create realserver option dictionary
        rs_option_dic["-t"]             = boost::bind(&l7vsadm::parse_opt_vs_target_func, this, _1, _2, _3);
        rs_option_dic["--tcp-service"]  = boost::bind(&l7vsadm::parse_opt_vs_target_func, this, _1, _2, _3);
        rs_option_dic["-w"]             = boost::bind(&l7vsadm::parse_opt_rs_weight_func, this, _1, _2, _3);
        rs_option_dic["--weight"]       = boost::bind(&l7vsadm::parse_opt_rs_weight_func, this, _1, _2, _3);
        rs_option_dic["--tproxy"]       = boost::bind(&l7vsadm::parse_opt_rs_fwdmode_func, this, _1, _2, _3);
        rs_option_dic["--masq"]         = boost::bind(&l7vsadm::parse_opt_rs_fwdmode_func, this, _1, _2, _3);
        rs_option_dic["-m"]             = boost::bind(&l7vsadm::parse_opt_vs_module_func, this, _1, _2, _3);
        rs_option_dic["--proto-module"] = boost::bind(&l7vsadm::parse_opt_vs_module_func, this, _1, _2, _3);
        rs_option_dic["-p"]             = boost::bind(&l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3);
        rs_option_dic["--udp"]          = boost::bind(&l7vsadm::parse_opt_vs_udp_func, this, _1, _2, _3);
        rs_option_dic["-r"]             = boost::bind(&l7vsadm::parse_opt_rs_realserver_func, this, _1, _2, _3);
        rs_option_dic["--real-server"]  = boost::bind(&l7vsadm::parse_opt_rs_realserver_func, this, _1, _2, _3);

        // create replication option dictionary
        replication_option_dic["-s"]           = boost::bind(&l7vsadm::parse_opt_replication_switch_func, this, _1, _2, _3);
        replication_option_dic["--switch"]     = boost::bind(&l7vsadm::parse_opt_replication_switch_func, this, _1, _2, _3);
        replication_switch_option_dic["start"] = boost::bind(&l7vsadm::parse_opt_replication_start_func, this, _1, _2, _3);
        replication_switch_option_dic["stop"]  = boost::bind(&l7vsadm::parse_opt_replication_stop_func, this, _1, _2, _3);
        replication_option_dic["-f"]           = boost::bind(&l7vsadm::parse_opt_replication_force_func, this, _1, _2, _3);
        replication_option_dic["--force"]      = boost::bind(&l7vsadm::parse_opt_replication_force_func, this, _1, _2, _3);
        replication_option_dic["-d"]           = boost::bind(&l7vsadm::parse_opt_replication_dump_func, this, _1, _2, _3);
        replication_option_dic["--dump"]       = boost::bind(&l7vsadm::parse_opt_replication_dump_func, this, _1, _2, _3);

        // create log option function dictionary create
        log_option_dic["-c"]         = boost::bind(&l7vsadm::parse_opt_log_category_func, this, _1, _2, _3);
        log_option_dic["--category"] = boost::bind(&l7vsadm::parse_opt_log_category_func, this, _1, _2, _3);
        log_option_dic["-l"]         = boost::bind(&l7vsadm::parse_opt_log_level_func, this, _1, _2, _3);
        log_option_dic["--level"]    = boost::bind(&l7vsadm::parse_opt_log_level_func, this, _1, _2, _3);

        // snmp agent option function dictionary create
        snmp_option_dic["-r"]               = boost::bind(&l7vsadm::parse_opt_snmp_refresh_func, this, _1, _2, _3);
        snmp_option_dic["--refresh"]        = boost::bind(&l7vsadm::parse_opt_snmp_refresh_func, this, _1, _2, _3);
        snmp_option_dic["-f"]            = boost::bind(&l7vsadm::parse_opt_snmp_flag_func, this, _1, _2, _3);
        snmp_option_dic["--flag"]        = boost::bind(&l7vsadm::parse_opt_snmp_flag_func, this, _1, _2, _3);
        snmp_option_dic["-i"]            = boost::bind(&l7vsadm::parse_opt_snmp_interval_func, this, _1, _2, _3);
        snmp_option_dic["--interval"]        = boost::bind(&l7vsadm::parse_opt_snmp_interval_func, this, _1, _2, _3);
        snmp_option_dic["-t"]            = boost::bind(&l7vsadm::parse_opt_snmp_log_trap_func, this, _1, _2, _3);
        snmp_option_dic["--logtrap"]        = boost::bind(&l7vsadm::parse_opt_snmp_log_trap_func, this, _1, _2, _3);
        snmp_option_dic["-l"]            = boost::bind(&l7vsadm::parse_opt_snmp_log_trap_level_func, this, _1, _2, _3);
        snmp_option_dic["--logtraplevel"]     = boost::bind(&l7vsadm::parse_opt_snmp_log_trap_level_func, this, _1, _2, _3);
        snmp_vs_option_dic["-t"]        = boost::bind(&l7vsadm::parse_opt_snmp_vs_target_func, this, _1, _2, _3);
        snmp_vs_option_dic["--tcp-service"]    = boost::bind(&l7vsadm::parse_opt_snmp_vs_target_func, this, _1, _2, _3);
        snmp_vs_option_dic["-m"]        = boost::bind(&l7vsadm::parse_opt_snmp_vs_module_func, this, _1, _2, _3);
        snmp_vs_option_dic["--proto-module"]    = boost::bind(&l7vsadm::parse_opt_snmp_vs_module_func, this, _1, _2, _3);
        // parameter option function dictionary create
        parameter_option_dic["-r"]       = boost::bind(&l7vsadm::parse_opt_parameter_reload_func, this, _1, _2, _3);
        parameter_option_dic["--reload"] = boost::bind(&l7vsadm::parse_opt_parameter_reload_func, this, _1, _2, _3);

        // string logcategory dictionary create
        string_logcategory_dic["l7vsd_network"]                      = LOG_CAT_L7VSD_NETWORK;
        string_logcategory_dic["nw"]                                 = LOG_CAT_L7VSD_NETWORK;
        logcategory_string_dic[LOG_CAT_L7VSD_NETWORK]                = "l7vsd_network";
        string_logcategory_dic["l7vsd_network_qos"]                  = LOG_CAT_L7VSD_NETWORK_QOS;
        string_logcategory_dic["nw_qos"]                             = LOG_CAT_L7VSD_NETWORK_QOS;
        logcategory_string_dic[LOG_CAT_L7VSD_NETWORK_QOS]            = "l7vsd_network_qos";
        string_logcategory_dic["l7vsd_network_bandwidth"]            = LOG_CAT_L7VSD_NETWORK_BANDWIDTH;
        string_logcategory_dic["nw_bw"]                              = LOG_CAT_L7VSD_NETWORK_BANDWIDTH;
        logcategory_string_dic[LOG_CAT_L7VSD_NETWORK_BANDWIDTH]      = "l7vsd_network_bandwidth";
        string_logcategory_dic["l7vsd_network_num_connection"]       = LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION;
        string_logcategory_dic["nw_conn"]                            = LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION;
        logcategory_string_dic[LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION] = "l7vsd_network_num_connection";
        string_logcategory_dic["l7vsd_network_access"]               = LOG_CAT_L7VSD_NETWORK_ACCESS;
        string_logcategory_dic["nw_acc"]                             = LOG_CAT_L7VSD_NETWORK_ACCESS;
        logcategory_string_dic[LOG_CAT_L7VSD_NETWORK_ACCESS]         = "l7vsd_network_access";
        string_logcategory_dic["l7vsd_mainthread"]                   = LOG_CAT_L7VSD_MAINTHREAD;
        string_logcategory_dic["mth"]                                = LOG_CAT_L7VSD_MAINTHREAD;
        logcategory_string_dic[LOG_CAT_L7VSD_MAINTHREAD]             = "l7vsd_mainthread";
        string_logcategory_dic["l7vsd_virtualservice"]               = LOG_CAT_L7VSD_VIRTUALSERVICE;
        string_logcategory_dic["vs"]                                 = LOG_CAT_L7VSD_VIRTUALSERVICE;
        logcategory_string_dic[LOG_CAT_L7VSD_VIRTUALSERVICE]         = "l7vsd_virtualservice";
        string_logcategory_dic["l7vsd_virtualservice_thread"]        = LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD;
        string_logcategory_dic["vs_th"]                              = LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD;
        logcategory_string_dic[LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD]  = "l7vsd_virtualservice_thread";
        string_logcategory_dic["l7vsd_session"]                      = LOG_CAT_L7VSD_SESSION;
        string_logcategory_dic["ss"]                                 = LOG_CAT_L7VSD_SESSION;
        logcategory_string_dic[LOG_CAT_L7VSD_SESSION]                = "l7vsd_session";
        string_logcategory_dic["l7vsd_session_thread"]               = LOG_CAT_L7VSD_SESSION_THREAD;
        string_logcategory_dic["ss_th"]                              = LOG_CAT_L7VSD_SESSION_THREAD;
        logcategory_string_dic[LOG_CAT_L7VSD_SESSION_THREAD]         = "l7vsd_session_thread";
        string_logcategory_dic["l7vsd_realserver"]                   = LOG_CAT_L7VSD_REALSERVER;
        string_logcategory_dic["rs"]                                 = LOG_CAT_L7VSD_REALSERVER;
        logcategory_string_dic[LOG_CAT_L7VSD_REALSERVER]             = "l7vsd_realserver";
        string_logcategory_dic["l7vsd_sorryserver"]                  = LOG_CAT_L7VSD_SORRYSERVER;
        string_logcategory_dic["sorry"]                              = LOG_CAT_L7VSD_SORRYSERVER;
        logcategory_string_dic[LOG_CAT_L7VSD_SORRYSERVER]            = "l7vsd_sorryserver";
        string_logcategory_dic["l7vsd_module"]                       = LOG_CAT_L7VSD_MODULE;
        string_logcategory_dic["mod"]                                = LOG_CAT_L7VSD_MODULE;
        logcategory_string_dic[LOG_CAT_L7VSD_MODULE]                 = "l7vsd_module";
        string_logcategory_dic["l7vsd_replication"]                  = LOG_CAT_L7VSD_REPLICATION;
        string_logcategory_dic["rep"]                                = LOG_CAT_L7VSD_REPLICATION;
        logcategory_string_dic[LOG_CAT_L7VSD_REPLICATION]            = "l7vsd_replication";
        string_logcategory_dic["l7vsd_replication_sendthread"]       = LOG_CAT_L7VSD_REPLICATION_SENDTHREAD;
        string_logcategory_dic["rep_sth"]                            = LOG_CAT_L7VSD_REPLICATION_SENDTHREAD;
        logcategory_string_dic[LOG_CAT_L7VSD_REPLICATION_SENDTHREAD] = "l7vsd_replication_sendthread";
        string_logcategory_dic["l7vsd_parameter"]                    = LOG_CAT_L7VSD_PARAMETER;
        string_logcategory_dic["para"]                               = LOG_CAT_L7VSD_PARAMETER;
        logcategory_string_dic[LOG_CAT_L7VSD_PARAMETER]              = "l7vsd_parameter";
        string_logcategory_dic["l7vsd_logger"]                       = LOG_CAT_L7VSD_LOGGER;
        string_logcategory_dic["logger"]                             = LOG_CAT_L7VSD_LOGGER;
        logcategory_string_dic[LOG_CAT_L7VSD_LOGGER]                 = "l7vsd_logger";
        string_logcategory_dic["l7vsd_command"]                      = LOG_CAT_L7VSD_COMMAND;
        string_logcategory_dic["cmd"]                                = LOG_CAT_L7VSD_COMMAND;
        logcategory_string_dic[LOG_CAT_L7VSD_COMMAND]                = "l7vsd_command";
        string_logcategory_dic["l7vsd_start_stop"]                   = LOG_CAT_L7VSD_START_STOP;
        string_logcategory_dic["stastp"]                             = LOG_CAT_L7VSD_START_STOP;
        logcategory_string_dic[LOG_CAT_L7VSD_START_STOP]             = "l7vsd_start_stop";
        string_logcategory_dic["l7vsd_system"]                       = LOG_CAT_L7VSD_SYSTEM;
        string_logcategory_dic["sys"]                                = LOG_CAT_L7VSD_SYSTEM;
        logcategory_string_dic[LOG_CAT_L7VSD_SYSTEM]                 = "l7vsd_system";
        string_logcategory_dic["l7vsd_system_memory"]                = LOG_CAT_L7VSD_SYSTEM_MEMORY;
        string_logcategory_dic["sys_mem"]                            = LOG_CAT_L7VSD_SYSTEM_MEMORY;
        logcategory_string_dic[LOG_CAT_L7VSD_SYSTEM_MEMORY]          = "l7vsd_system_memory";
        string_logcategory_dic["l7vsd_system_endpoint"]              = LOG_CAT_L7VSD_SYSTEM_ENDPOINT;
        string_logcategory_dic["sys_ep"]                             = LOG_CAT_L7VSD_SYSTEM_ENDPOINT;
        logcategory_string_dic[LOG_CAT_L7VSD_SYSTEM_ENDPOINT]        = "l7vsd_system_endpoint";
        string_logcategory_dic["l7vsd_system_signal"]                = LOG_CAT_L7VSD_SYSTEM_SIGNAL;
        string_logcategory_dic["sys_sig"]                            = LOG_CAT_L7VSD_SYSTEM_SIGNAL;
        logcategory_string_dic[LOG_CAT_L7VSD_SYSTEM_SIGNAL]          = "l7vsd_system_signal";
        string_logcategory_dic["l7vsd_system_environment"]           = LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT;
        string_logcategory_dic["sys_env"]                            = LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT;
        logcategory_string_dic[LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT]     = "l7vsd_system_environment";
        string_logcategory_dic["l7vsd_snmpagent"]                    = LOG_CAT_L7VSD_SNMPAGENT;
        string_logcategory_dic["agent"]                              = LOG_CAT_L7VSD_SNMPAGENT;
        logcategory_string_dic[LOG_CAT_L7VSD_SNMPAGENT]              = "l7vsd_snmpagent";
        string_logcategory_dic["l7vsd_protocol"]                     = LOG_CAT_PROTOCOL;
        string_logcategory_dic["prot"]                               = LOG_CAT_PROTOCOL;
        logcategory_string_dic[LOG_CAT_PROTOCOL]                     = "l7vsd_protocol";
        string_logcategory_dic["l7vsd_schedule"]                     = LOG_CAT_SCHEDULE;
        string_logcategory_dic["sched"]                              = LOG_CAT_SCHEDULE;
        logcategory_string_dic[LOG_CAT_SCHEDULE]                     = "l7vsd_schedule";
        string_logcategory_dic["all"]                                = LOG_CAT_END;

        // string log level dictionary create.
        string_loglevel_dic["debug"]      = LOG_LV_DEBUG;
        loglevel_string_dic[LOG_LV_DEBUG] = "debug";
        string_loglevel_dic["info"]       = LOG_LV_INFO;
        loglevel_string_dic[LOG_LV_INFO]  = "info";
        string_loglevel_dic["warn"]       = LOG_LV_WARN;
        loglevel_string_dic[LOG_LV_WARN]  = "warn";
        string_loglevel_dic["error"]      = LOG_LV_ERROR;
        loglevel_string_dic[LOG_LV_ERROR] = "error";
        string_loglevel_dic["fatal"]      = LOG_LV_FATAL;
        loglevel_string_dic[LOG_LV_FATAL] = "fatal";

        // parameter category dictionary create
        string_parameter_dic["all"]            = PARAM_COMP_ALL;
        string_parameter_dic["l7vsd"]          = PARAM_COMP_L7VSD;
        string_parameter_dic["command"]        = PARAM_COMP_COMMAND;
        string_parameter_dic["session"]        = PARAM_COMP_SESSION;
        string_parameter_dic["virtualservice"] = PARAM_COMP_VIRTUALSERVICE;
        string_parameter_dic["module"]         = PARAM_COMP_MODULE;
        string_parameter_dic["replication"]    = PARAM_COMP_REPLICATION;
        string_parameter_dic["logger"]         = PARAM_COMP_LOGGER;
        string_parameter_dic["l7vsadm"]        = PARAM_COMP_L7VSADM;
        string_parameter_dic["snmpagent"]      = PARAM_COMP_SNMPAGENT;
        string_parameter_dic["ssl"]            = PARAM_COMP_SSL;

        // create disp_result dictionary.
        disp_result_dic[l7vsadm_request::CMD_LIST]         = boost::bind(&l7vsadm::disp_list, this);
        disp_result_dic[l7vsadm_request::CMD_LIST_KEY]     = boost::bind(&l7vsadm::disp_list_key, this);
        disp_result_dic[l7vsadm_request::CMD_LIST_VERBOSE] = boost::bind(&l7vsadm::disp_list_verbose, this);

        // response_message_dic create
        response_error_message_dic[l7vsd_response::RESPONSE_ERROR]              = "command error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_LIST_ERROR]         = "list command error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_LIST_VERBOSE_ERROR] = "list verbose error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_LIST_KEY_ERROR]     = "list key error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_ADD_VS_ERROR]       = "add vs error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_DEL_VS_ERROR]       = "del vs error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_EDIT_VS_ERROR]      = "edit vs error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_FLUSH_VS_ERROR]     = "flush vs error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_ADD_RS_ERROR]       = "add rs error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_DEL_RS_ERROR]       = "del rs error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_EDIT_RS_ERROR]      = "edit rs error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_REPLICATION_ERROR]  = "replication command error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_LOG_ERROR]          = "log command error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_SNMP_ERROR]         = "snmp command error : ";
        response_error_message_dic[l7vsd_response::RESPONSE_PARAMETER_ERROR]    = "parameter error : ";

        replication_mode_string_dic[replication::REPLICATION_OUT]         = "OUT";
        replication_mode_string_dic[replication::REPLICATION_SINGLE]      = "SINGLE";
        replication_mode_string_dic[replication::REPLICATION_MASTER]      = "MASTER";
        replication_mode_string_dic[replication::REPLICATION_SLAVE]       = "SLAVE";
        replication_mode_string_dic[replication::REPLICATION_MASTER_STOP] = "MASTER_STOP";
        replication_mode_string_dic[replication::REPLICATION_SLAVE_STOP]  = "SLAVE_STOP";
}

//! Get l7vsadm parameter data
void l7vs::l7vsadm::set_parameter()
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 36, "l7vsadm::set_parameter", __FILE__, __LINE__);

        // Get and Set l7vsadm all parameter value.
        Parameter param;
        error_code err;

        // command_wait_interval
        command_wait_interval = param.get_int(PARAM_COMP_L7VSADM, "cmd_interval", err);
        if (!err) {
                if (command_wait_interval < 0 ||
                    command_wait_interval > L7VSADM_MAX_WAIT) {
                        // When illegal parameter value, use default parameter value.
                        command_wait_interval = L7VSADM_DEFAULT_WAIT_INTERVAL;
                        std::stringstream msg;
                        msg << boost::format("Illegal cmd_interval parameter value. Use default value(%s).")
                            % L7VSADM_DEFAULT_WAIT_INTERVAL;
                        Logger::putLogWarn(LOG_CAT_L7VSADM_COMMON, 1, msg.str(), __FILE__, __LINE__);
                }
        } else {
                command_wait_interval = L7VSADM_DEFAULT_WAIT_INTERVAL;
                std::stringstream msg;
                msg << boost::format("Get cmd_interval parameter error. Use default value(%s).")
                    % L7VSADM_DEFAULT_WAIT_INTERVAL;
                Logger::putLogWarn(LOG_CAT_L7VSADM_COMMON, 2, msg.str(), __FILE__, __LINE__);
        }

        //command_wait_count
        command_wait_count = param.get_int(PARAM_COMP_L7VSADM, "cmd_count", err);
        if (!err) {
                if (command_wait_count < 0 ||
                    command_wait_count > L7VSADM_MAX_WAIT) {
                        // When illegal parameter value, use default parameter value.
                        command_wait_count = L7VSADM_DEFAULT_WAIT_COUNT;
                        std::stringstream msg;
                        msg << boost::format("Illegal cmd_count parameter value. Use default value(%s).")
                            % L7VSADM_DEFAULT_WAIT_COUNT;
                        Logger::putLogWarn(LOG_CAT_L7VSADM_COMMON, 3, msg.str(), __FILE__, __LINE__);
                }
        } else {
                command_wait_count = L7VSADM_DEFAULT_WAIT_COUNT;
                std::stringstream msg;
                msg << boost::format("Get cmd_count parameter error. Use default value(%s).")
                    % L7VSADM_DEFAULT_WAIT_COUNT;
                Logger::putLogWarn(LOG_CAT_L7VSADM_COMMON, 4, msg.str(), __FILE__, __LINE__);
        }

        if ((command_wait_interval * command_wait_count) > L7VSADM_MAX_WAIT) {
                // When wait value too long, use default parameter value.
                command_wait_interval = L7VSADM_DEFAULT_WAIT_INTERVAL;
                command_wait_count = L7VSADM_DEFAULT_WAIT_COUNT;
                std::stringstream msg;
                msg << boost::format("Command wait value too long. Use default value(%s).")
                    % L7VSADM_DEFAULT_WAIT_COUNT;
                Logger::putLogWarn(LOG_CAT_L7VSADM_COMMON, 9, msg.str(), __FILE__, __LINE__);
        }
}

//! l7vsadm command execute
bool l7vs::l7vsadm::execute(int argc, char *argv[])
{
        Logger logger(LOG_CAT_L7VSADM_COMMON, 37, "l7vsadm::execute", __FILE__, __LINE__);

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSADM_COMMON)) {
                std::stringstream debugstr;
                debugstr << boost::format("l7vsadm::execute arguments: %s") % argument_debug_dump(argc, argv);
                Logger::putLogDebug(LOG_CAT_L7VSADM_COMMON, 38, debugstr.str(), __FILE__, __LINE__);
        }
        /*------ DEBUG LOG END ------*/

        // set sighandler
        if (0 > set_sighandlers()) {
                std::string buf("set_sighandlers failed.");
                std::cerr << "COMMON ERROR: " << buf << std::endl;
                Logger::putLogError(LOG_CAT_L7VSADM_COMMON, 1, buf, __FILE__, __LINE__);
                return false;
        }

        // read param
        set_parameter();

        // Get l7vsadm execute file path from /proc/(pid)/exe (symbolic link)
        char l7vsadm_file_path[256];
        ssize_t retsize;
        memset(l7vsadm_file_path, 0, sizeof(l7vsadm_file_path));
        retsize = readlink("/proc/self/exe", l7vsadm_file_path, sizeof(l7vsadm_file_path));

        // L7vsadm command conflict check. (Try l7vsadm execute file lock)
        file_lock lock(l7vsadm_file_path, l7vsadm_err);
        if (l7vsadm_err) {
                std::cerr << "COMMON ERROR: " << l7vsadm_err.get_message() << std::endl;
                Logger::putLogError(LOG_CAT_L7VSADM_COMMON, 2, l7vsadm_err.get_message(), __FILE__, __LINE__);
                return false;
        }

        try {
                // l7vsadm file lock wait
                int command_retry_count = 0;
                while (true) {
                        // Check signal.
                        if (signal_flag) {
                                std::stringstream buf;
                                buf << boost::format("Signal (%d) Received.") % received_sig;
                                l7vsadm_err.setter(true, buf.str());
                                Logger::putLogError(LOG_CAT_L7VSADM_COMMON, 3, buf.str(), __FILE__, __LINE__);
                                break;
                        }

                        // Try lock l7vsadm file.
                        if (lock.try_lock()) {
                                break;
                        }

                        ++command_retry_count;
                        if (command_retry_count > command_wait_count) {
                                // L7vsadm file lock error. (l7vsadm is executing)
                                std::string buf("L7vsadm file lock timeout. (l7vsadm is already executing)");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_COMMON, 4, buf, __FILE__, __LINE__);
                                break;
                        }

                        std::stringstream buf;
                        buf << boost::format("L7vsadm file lock error. (l7vsadm is already executing) (retry %d)") % command_retry_count;
                        Logger::putLogWarn(LOG_CAT_L7VSADM_COMMON, 11, buf.str(), __FILE__, __LINE__);

                        // Lock retrying.
                        boost::xtime xt;
                        xtime_get(&xt, boost::TIME_UTC);
                        xt.sec += command_wait_interval;
                        boost::thread::sleep(xt);
                }

                // display err
                if (l7vsadm_err) {
                        std::cerr << "COMMON ERROR: " << l7vsadm_err.get_message() << std::endl;
                        return false;
                }

                // no argument, assume list command
                if (1 == argc) {
                        request.command = l7vsadm_request::CMD_LIST;
                } else {
                        // parse command line
                        int pos = 1;
                        parse_cmd_map_type::iterator itr = command_dic.find(argv[pos]);
                        if (itr != command_dic.end()) {
                                itr->second(argc, argv);
                        } else {
                                std::string buf("command not found.");
                                l7vsadm_err.setter(true, buf);
                                Logger::putLogError(LOG_CAT_L7VSADM_PARSE, 88, buf, __FILE__, __LINE__);
                        }
                }

                // display command parse result
                if (l7vsadm_err) {
                        std::cerr << "PARSE ERROR: " << l7vsadm_err.get_message() << std::endl;
                        std::cerr << usage() << std::endl;
                        return false;
                }

                if (l7vsadm_request::CMD_HELP != request.command) {
                        // communicate to l7vsd
                        using boost::asio::local::stream_protocol;
                        boost::array<char, COMMAND_BUFFER_SIZE> response_buffer;
                        response_buffer.assign(0x00);

                        // connect
                        boost::asio::io_service io;
                        stream_protocol::socket s(io);

                        // Check signal.
                        if (signal_flag) {
                                std::stringstream buf;
                                buf << boost::format("Signal (%d) Received.") % received_sig;
                                l7vsadm_err.setter(true, buf.str());
                                Logger::putLogError(LOG_CAT_L7VSADM_COMMON, 5, buf.str(), __FILE__, __LINE__);
                        } else {
                                // Try connect to config socket.
                                boost::system::error_code err;
                                s.connect(stream_protocol::endpoint(L7VS_CONFIG_SOCKNAME), err);
                                if (err) {
                                        std::stringstream buf;
                                        buf << boost::format("connect() failed: %s.") % err.message();
                                        l7vsadm_err.setter(true, buf.str());
                                        Logger::putLogError(LOG_CAT_L7VSADM_COMMON, 9, buf.str(), __FILE__, __LINE__);
                                }
                        }

                        // display err
                        if (l7vsadm_err) {
                                std::cerr << "COMMON ERROR: " << l7vsadm_err.get_message() << std::endl;
                                return false;
                        }

                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSADM_COMMON)) {
                                std::stringstream debugstr;
                                debugstr << boost::format("l7vsadm_send_request:%s") % request;
                                Logger::putLogDebug(LOG_CAT_L7VSADM_COMMON, 39, debugstr.str(), __FILE__, __LINE__);
                        }
                        /*------ DEBUG LOG END ------*/

                        // write sockfile
                        std::stringstream send_stream;
                        boost::archive::text_oarchive oa(send_stream);
                        oa << (const l7vs::l7vsadm_request &) request;
                        boost::asio::write(s, boost::asio::buffer(send_stream.str()));

                        // read sockfile
                        s.read_some(boost::asio::buffer(response_buffer));

                        std::stringstream recv_stream;
                        recv_stream << &(response_buffer[0]);
                        boost::archive::text_iarchive ia(recv_stream);
                        ia >> response;

                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSADM_COMMON)) {
                                std::stringstream debugstr;
                                debugstr << boost::format("l7vsadm_recv_response:%s") % response;
                                Logger::putLogDebug(LOG_CAT_L7VSADM_COMMON, 40, debugstr.str(), __FILE__, __LINE__);
                        }
                        /*------ DEBUG LOG END ------*/

                        // close socket
                        s.close();

                        // display result
                        if (l7vsd_response::RESPONSE_OK == response.status) {
                                disp_result_map_type::iterator itr = disp_result_dic.find(request.command);
                                if (itr != disp_result_dic.end())
                                        itr->second();
                        } else {
                                std::stringstream buf;
                                response_error_message_map_type::iterator itr = response_error_message_dic.find(response.status);
                                if (itr != response_error_message_dic.end())
                                        buf << itr->second << response.message;
                                else
                                        buf << "COMMAND ERROR: " << response.message;
                                std::cerr << buf.str() << std::endl;
                                Logger::putLogError(LOG_CAT_L7VSADM_COMMON, 7, buf.str(), __FILE__, __LINE__);
                                return false;
                        }
                } //if help_mode
        } //try
        catch (std::exception &e) {
                std::stringstream buf;
                buf << "COMMON ERROR: " << e.what();
                std::cerr << buf.str() << std::endl;
                Logger::putLogError(LOG_CAT_L7VSADM_COMMON, 8, buf.str(), __FILE__, __LINE__);
                return false;
        }
        return true;
}

//! argument dump for debug
//! @param[in] argument count
//! @param[in] argument value
std::string l7vs::l7vsadm::argument_debug_dump(int argc, char *argv[])
{
        std::stringstream buf;
        if (!argv) {
                buf << "argument=(null)";
        } else {
                buf << boost::format("argument={argc=%d") % argc;
                for (int i = 0; i < argc; ++i) {
                        buf << boost::format(", argv[%d]=%s") % i % argv[i];
                }
                buf << "}";
        }
        return buf.str();
}

//! signal handler function
//! @param[in] signal
static void sig_exit_handler(int sig)
{
        received_sig = sig;
        signal_flag = true;
}

//! set signal handler function
//! @param[in] signal
//! @param[in] handler function pointer
//! @return 0/success, -1/fail
static int set_sighandler(int sig, void (*handler)(int))
{
        struct sigaction act;
        int ret;

        ret = sigaction(sig, NULL, &act);
        if (0 > ret) {
                return ret;
        }

        act.sa_flags &= ~SA_RESETHAND;
        act.sa_handler = handler;

        ret = sigaction(sig, &act, NULL);
        if (0 > ret) {
                return ret;
        }

        return 0;
}

//! set all signal handler function
//! @return 0/success, -1/fail
static int set_sighandlers()
{
        int ret;

#define SET_SIGHANDLER(sig, handler)            \
    do {                                        \
        ret = set_sighandler((sig), (handler)); \
        if (ret < 0) {                          \
            return ret;                         \
        }                                       \
    } while (0)

        SET_SIGHANDLER(SIGHUP,  sig_exit_handler);
        SET_SIGHANDLER(SIGINT,  sig_exit_handler);
        SET_SIGHANDLER(SIGQUIT, sig_exit_handler);
        SET_SIGHANDLER(SIGPIPE, sig_exit_handler);
        SET_SIGHANDLER(SIGTERM, sig_exit_handler);
        SET_SIGHANDLER(SIGUSR1, sig_exit_handler);
        SET_SIGHANDLER(SIGUSR2, sig_exit_handler);

#undef SET_SIGHANDLER

        return 0;
}

#ifndef UNIT_TEST
int main(int argc, char *argv[])
{
        try {
                l7vs::Logger logger;
                l7vs::Parameter param;
                logger.loadConf();
        } catch (...) {
        }

        l7vs::l7vsadm adm;
        if (!adm.execute(argc, argv)) {
                return -1;
        }
        return 0;
}
#endif //UNIT_TEST
