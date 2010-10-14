/*!
 * @file  snmpagent_impl.cpp
 * @brief snmpagent implement module.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2010  NTT COMWARE Corporation.
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

#include "l7vsd.h"
#include "snmpagent_impl.h"
#include "mibdata.h"
#include "protocol_module_base.h"
#include "snmpfunc.h"

#define SNMP_SLEEP_NANO_TIME                 (1000000)
#define SNMP_MAX_TRAP_POLLING_INTEVAL        (1000000000)
#define SNMP_DEFAULT_TRAP_POLLING_INTEVAL    (100000000)
#define SNMP_DEFAULT_TRAP_QUEUE_MAX_SIZE     (1000)
#define TRAP_TIME_STRING_MAX_SIZE            (20)

#ifndef FALSE
#define FALSE                                (0)
#endif
#ifndef TRUE
#define TRUE                                 (1)
#endif

namespace l7vs
{

        /*!
         * process mib get item thread function.
         * @retrun  void
         */
        void snmpagent_impl::process_mib_run()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 19, "snmpagent_impl::process_mib_run", __FILE__, __LINE__);

                //create wait time
                timespec    wait_val;
                wait_val.tv_sec     = 0;
                wait_val.tv_nsec    = SNMP_SLEEP_NANO_TIME;

                //main loop
                while (enabled.get() != FALSE) {
                        //agent check and process
                        agent_check_and_process(0);

                        //collect mib data
                        error_code err;
                        collect_mibdata(err);

                        if (err) {
                                std::string msg("collect mib data failed.");
                                Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 67, msg, __FILE__, __LINE__);
                        }

                        //sleep a little time
                        nanosleep(&wait_val, NULL);
                }
        }

        /*!
         * process trap thread function.
         * @retrun  void
         */
        void snmpagent_impl::trap_run()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 20, "snmpagent_impl::trap_run", __FILE__, __LINE__);

                //create wait time
                timespec wait_val;
                wait_val.tv_sec     = 0;
                wait_val.tv_nsec    = trap_polling_interval.get();

                //main loop
                while (enabled.get() != FALSE) {
                        //create trap message
                        trapmessage trapmsg;
                        {
                                boost::mutex::scoped_lock lock(trap_msg_queue_condition_mutex);

                                //loop until the trap message is not none
                                while (trap_msg_queue.size() == 0) {
                                        trap_msg_queue_condition.wait(lock);
                                }

                                //get the first message in trap message
                                trapmsg = trap_msg_queue[0];

                                //pop the first message in trap message
                                trap_msg_queue.pop_front();
                        }

                        //find mapping in trap message map
                        trapfunc_type::iterator it = trapfunc_map.find(trapmsg.type);

                        if (it != trapfunc_map.end()) {
                                //send trap message
                                if (it->second(trapmsg.message) == SNMP_ERR_NOERROR)
                                {
                                        //set trap last date to current time
                                        trap_last_date = time(NULL);

                                        //incrememt snmp trap count
                                        snmp_trap_count++;
                                }
                        }

                        //sleep a little time
                        nanosleep(&wait_val, NULL);
                }
        }

        /*!
         * check mib data timeout.
         * @retrun  true: time not out
         *          false: time out
         */
        bool snmpagent_impl::check_timeout()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 21, "snmpagent::check_timeout", __FILE__, __LINE__);

                if (interval.get() == 0) {
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                std::string  debugstr =  "function : snmpagent_impl::check_timeout : timeout.";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 22, debugstr, __FILE__, __LINE__);
                        }
                        /*------ DEBUG LOG END ------*/

                        //when interval is 0, return false(timeout)
                        return false;
                } else {
                        //get current time
                        boost::posix_time::ptime  now_time(boost::posix_time::second_clock::local_time());

                        //calc time interval
                        boost::posix_time::time_duration td = boost::posix_time::seconds(interval.get());

                        if ((now_time - mib_collect_last_time) > td) {
                                /*-------- DEBUG LOG --------*/
                                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                        std::string  debugstr =  "function : snmpagent_impl::check_timeout : timeout.";
                                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 23, debugstr, __FILE__, __LINE__);
                                }
                                /*------ DEBUG LOG END ------*/

                                //timeout
                                return false;
                        } else {
                                /*-------- DEBUG LOG --------*/
                                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                        std::string  debugstr =  "function : snmpagent_impl::check_timeout : not timeout.";
                                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 24, debugstr, __FILE__, __LINE__);
                                }
                                /*------ DEBUG LOG END ------*/
                                //not timeout
                                return true;
                        }
                }
        }

        /*!
         * create and return a snmpagent_impl instance.
         *
         * @retrun  snmpagent_impl & snmpagent_impl' instance
         */
        snmpagent_impl& snmpagent_impl::get_instance()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 25, "snmpagent::get_instance", __FILE__, __LINE__);

                static snmpagent_impl agent_impl;
                return agent_impl;
        }

        /*!
         * snmpagent_impl class initialize.
         *
         * @param   error_code & error code
         * @retrun  void
         */
        void snmpagent_impl::init(error_code& err)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 26, "snmpagent_impl::init", __FILE__, __LINE__);

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream    debugstr;
                        debugstr << "snmpagent_impl::init arguments:";
                        debugstr << boost::format("err=%d") % (err ? true : false);
                        debugstr << boost::format(", err.message=%s") % err.get_message();
                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 27, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/

                try {
                        if (initialized == false) {
                                //set snmp start flag false
                                start_flag = false;

                                //mapping trap type and trap function
                                trapfunc_map[trapmessage::SERVICE_START]         = &trap_service_start;
                                trapfunc_map[trapmessage::SERVICE_STOP]          = &trap_service_stop;
                                trapfunc_map[trapmessage::VIRTUALSERVICE_ADD]    = &trap_vs_add;
                                trapfunc_map[trapmessage::VIRTUALSERVICE_CHANGE] = &trap_vs_change;
                                trapfunc_map[trapmessage::VIRTUALSERVICE_REMOVE] = &trap_vs_remove;
                                trapfunc_map[trapmessage::REALSERVER_ADD]        = &trap_rs_add;
                                trapfunc_map[trapmessage::REALSERVER_CHANGE]     = &trap_rs_change;
                                trapfunc_map[trapmessage::REALSERVER_REMOVE]     = &trap_rs_remove;
                                trapfunc_map[trapmessage::UPQOS_ALERT_ON]        = &trap_up_qos_alert_on;
                                trapfunc_map[trapmessage::UPQOS_ALERT_OFF]       = &trap_up_qos_alert_off;
                                trapfunc_map[trapmessage::DOWNQOS_ALERT_ON]      = &trap_down_qos_alert_on;
                                trapfunc_map[trapmessage::DOWNQOS_ALERT_OFF]     = &trap_down_qos_alert_off;
                                trapfunc_map[trapmessage::SESSIONPOOL_ALERT_ON]  = &trap_sessionpool_alert_on;
                                trapfunc_map[trapmessage::SESSIONPOOL_ALERT_OFF] = &trap_sessionpool_alert_off;
                                trapfunc_map[trapmessage::FATAL_LOG]             = &trap_fatal;
                                trapfunc_map[trapmessage::ERROR_LOG]             = &trap_error;
                                trapfunc_map[trapmessage::WARN_LOG]              = &trap_warn;
                                trapfunc_map[trapmessage::INFO_LOG]              = &trap_info;
                                trapfunc_map[trapmessage::DEBUG_LOG]             = &trap_debug;

                                //init member variables
                                mib_collect_last_time = boost::posix_time::ptime(boost::posix_time::min_date_time);
                                start_date = 0;
                                request_last_date = 0;
                                trap_last_date = 0;
                                snmp_get_requests = 0;
                                snmp_set_requests = 0;
                                snmp_trap_count = 0;

                                //mapping log string and log level
                                string_loglevel_map["debug"] = LOG_LV_DEBUG;
                                string_loglevel_map["info"]  = LOG_LV_INFO;
                                string_loglevel_map["warn"]  = LOG_LV_WARN;
                                string_loglevel_map["error"] = LOG_LV_ERROR;
                                string_loglevel_map["fatal"] = LOG_LV_FATAL;

                                //load config file setting
                                load_config();

                                /*-------- DEBUG LOG --------*/
                                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                        std::string  debugstr =  "function : snmpagent_impl::init : call load_config().";
                                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 28, debugstr, __FILE__, __LINE__);
                                }
                                /*------ DEBUG LOG END ------*/

                                netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_ROLE, 1);

                                //socket startup
                                SOCK_STARTUP;

                                //init l7vsAgent
                                int ret = init_agent("l7vsAgent");
                                if (ret) {
                                        std::string msg("init_agent failed.");
                                        Logger::putLogFatal(LOG_CAT_L7VSD_SNMPAGENT, 3, msg, __FILE__, __LINE__);
                                        //set error code
                                        err.setter(true, msg);
                                        return;
                                }

                                init_snmp("l7vsAgent");

                                //register snmp get item' handle
                                init_snmp_handles(err);

                                if (err) {
                                        return;
                                }

                                //set initialize flag true
                                initialized = true;

                                /*-------- DEBUG LOG --------*/
                                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                        std::string  debugstr =  "function : snmpagent_impl::init : initialize success.";
                                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 29, debugstr, __FILE__, __LINE__);
                                }
                                /*------ DEBUG LOG END ------*/
                        }
                } catch (const std::exception &e) {
                        std::stringstream msg;
                        msg << "init failed : " << e.what() << ".";
                        Logger::putLogFatal(LOG_CAT_L7VSD_SNMPAGENT, 2, msg.str(), __FILE__, __LINE__);
                }
        }

        /*!
         *  push the trap message into trap message queue.
         *
         * @param[in]      in_trapmessage is trap message
         * @param[in/out]  err is error code
         * @retrun         void
         */
        void snmpagent_impl::push_trapmessage(trapmessage& trapmessage, error_code& err)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 30, "snmpagent_impl::push_trapmessage", __FILE__, __LINE__);

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream    debugstr;
                        debugstr << "snmpagent_impl::push_trapmessage arguments : ";
                        debugstr << boost::format("trapmessage.type=%d") % trapmessage.type;
                        debugstr << boost::format(", trapmessage.message=%s") % trapmessage.message;
                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 31, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/
                try {
                        //snmp function is enable
                        if ((start_flag == true) && (enabled.get() == TRUE)) {
                                //get local time string
                                char time_buf[TRAP_TIME_STRING_MAX_SIZE] = {0};
                                time_t now = time(NULL);
                                struct tm *t = localtime(&now);
                                if (t)     strftime(time_buf, sizeof(time_buf), "%Y/%m/%d %H:%M:%S", t);
                                //time string added in trap message
                                std::stringstream   trap_buf;
                                trap_buf << time_buf << "," << trapmessage.message;

                                if (trapmessage.type < trapmessage::DEBUG_LOG) {
                                        char buff[HOST_NAME_MAX] = {0};
                                        gethostname(buff, HOST_NAME_MAX);
                                        trap_buf << "," << buff;
                                }

                                trapmessage.message = trap_buf.str();

                                {
                                        boost::mutex::scoped_lock lock(trap_msg_queue_condition_mutex);

                                        //drop the message when the trap message queue is too large
                                        if( trap_msg_queue.size() < trap_queue_max_size.get())
                                        {
                                                //push the message into trap message queue
                                                trap_msg_queue.push_back(trapmessage);
                                                /*-------- DEBUG LOG --------*/
                                                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                                        std::stringstream    debugstr;
                                                        debugstr << "function : snmpagent_impl::push_trapmessage : trap message push back.";
                                                        debugstr << boost::format("trapmessage.type=%d") % trapmessage.type;
                                                        debugstr << boost::format(", trapmessage.message=%s") % trapmessage.message;
                                                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 32, debugstr.str(), __FILE__, __LINE__);
                                                }
                                                /*------ DEBUG LOG END ------*/
                                        }
                                        else{
                                                /*-------- DEBUG LOG --------*/
                                                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                                        std::string  debugstr =  "function : snmpagent_impl::push_trapmessage : trap message queue is overflow.";
                                                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 33, debugstr, __FILE__, __LINE__);
                                                }
                                                /*------ DEBUG LOG END ------*/
                                        }
                                }

                                //notify the trap main thread to process the message
                                trap_msg_queue_condition.notify_one();

                        }
                }
                catch (const std::exception &e) {
                        std::stringstream msg;
                        msg << "Push trap message failed : " << e.what() << ".";

                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 1, msg.str(), __FILE__, __LINE__);
                        //set error code
                        err.setter(true, msg.str());
                }

        }

        /*!
         * snmpagent function start.
         *
         * @param[in/out]  error_code is error code
         * @retrun  void
         */
        void snmpagent_impl::start(error_code& err)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 34, "snmpagent_impl::start", __FILE__, __LINE__);

                if (vsd == NULL) {
                        //vsd pointer is NULL
                        std::string msg("vsd pointer is NULL.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 2, msg, __FILE__, __LINE__);
                        err.setter(false, msg);
                        return;
                }

                //check snmp enable state
                if (enabled.get() == FALSE) {
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                std::string  debugstr =  "function : snmpagent_impl::start : snmp function was disabled.";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 35, debugstr, __FILE__, __LINE__);
                        }
                        /*------ DEBUG LOG END ------*/
                        return;
                }

                //check start state
                if (start_flag) {
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                std::string  debugstr =  "function : snmpagent_impl::start : snmp function has already started.";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 36, debugstr, __FILE__, __LINE__);
                        }
                        /*------ DEBUG LOG END ------*/
                        return;
                }

                try {
                        //collect mib data first time
                        mibdata::get_instance().collect_mibdata(vsd, err);

                        if (err) {
                                //mib collect failed
                                std::string msg("collect_mibdata failed,thread start failed.");
                                Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 3, msg, __FILE__, __LINE__);
                                //set error code
                                err.setter(true, msg);
                                return;
                        }

                        //start process mib run thread
                        process_mib_thread = boost::thread(&snmpagent_impl::process_mib_run, this);

                        //start trap run
                        trap_thread = boost::thread(&snmpagent_impl::trap_run, this);

                        //set start time and start flag
                        start_date = time(NULL);
                        start_flag = true;

                        std::string  str =  "snmp function start.";
                        Logger::putLogInfo(LOG_CAT_L7VSD_SNMPAGENT, 1, str, __FILE__, __LINE__);

                } catch (const boost::thread_resource_error &e) {
                        std::stringstream msg;
                        msg << "snmp function start failed : " << e.what() << ".";

                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 4, msg.str(), __FILE__, __LINE__);
                        //set error code
                        err.setter(true, msg.str());
                }
        }

        /*!
         * snmpagent function stop.
         *
         * @retrun  void
         */
        void snmpagent_impl::stop()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 37, "snmpagent_impl::stop", __FILE__, __LINE__);

                enabled = FALSE;

                if (!start_flag) {
                        std::string  str =  "snmp function has already stopped.";
                        Logger::putLogInfo(LOG_CAT_L7VSD_SNMPAGENT, 2, str, __FILE__, __LINE__);
                        return;
                }

                start_flag = false;

                try {
                        trap_thread.interrupt();
                        process_mib_thread.join();
                        trap_thread.join();

                        std::string  str =  "snmp function stop.";
                        Logger::putLogInfo(LOG_CAT_L7VSD_SNMPAGENT, 3, str, __FILE__, __LINE__);

                } catch (const boost::thread_interrupted&) {
                        std::stringstream msg;
                        msg << "thread interrupted exception.";
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 5, msg.str(), __FILE__, __LINE__);
                }
        }

        /*!
         *  set snmp function enable
         *
         * @retrun         void
         */
        void snmpagent_impl::enable() {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 38, "snmpagent_impl::enable", __FILE__, __LINE__);

                //set snmp function enable flag
                enabled = TRUE;
                if (start_flag == false) {
                        error_code err;
                        start(err);

                        if (err) {
                                std::stringstream msg;
                                msg << "snmp function start failed.";
                                Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 6, msg.str(), __FILE__, __LINE__);
                        }
                }
                else {
                        std::string  str =  "snmp function has already started.";
                        Logger::putLogInfo(LOG_CAT_L7VSD_SNMPAGENT, 4, str, __FILE__, __LINE__);
                }
        }

        /*!
         *  set snmp function disable
         *
         * @retrun         void
         */
        void snmpagent_impl::disable() {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 39, "snmpagent_impl::disable", __FILE__, __LINE__);

                //set snmp function enable flag
                enabled = FALSE;
                if (start_flag == true) {
                        stop();
                }
                else {
                        std::string  str =  "snmp function has already stopped.";
                        Logger::putLogInfo(LOG_CAT_L7VSD_SNMPAGENT, 5, str, __FILE__, __LINE__);
                }
        }

        /*!
         * set log trap enable
         * @retrun  void
         */
        void snmpagent_impl::logtrap_enable() {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 40, "snmpagent_impl::logtrap_enable", __FILE__, __LINE__);

                logtrap_enabled = TRUE;
                Logger::logtrap_enable();
        }

        /*!
         * set log trap disable
         * @retrun  void
         */
        void snmpagent_impl::logtrap_disable() {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 41, "snmpagent_impl::logtrap_disable", __FILE__, __LINE__);

                logtrap_enabled = FALSE;
                Logger::logtrap_disable();
        }

        /*!
         *  set log trap level.
         *
         * @param[in]      in_level is log trap level
         * @retrun         void
         */
        void snmpagent_impl::set_logtrap_level(const LOG_LEVEL_TAG  in_level)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 42, "snmpagent_impl::set_logtrap_level", __FILE__, __LINE__);

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream    debugstr;
                        debugstr << "snmpagent_impl::set_logtrap_level arguments:";
                        debugstr << boost::format("in_level=%d") % in_level;
                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 43, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/

                logtrap_level = in_level;
                Logger::set_logtrap_level(in_level);
        }

        /*!
         *  set mib cache update interval.
         *
         * @param[in]      in_interval is cache update interval
         * @retrun         void
         */
        void snmpagent_impl::set_interval(const unsigned int in_interval)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 44, "snmpagent_impl::set_interval", __FILE__, __LINE__);

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream    debugstr;
                        debugstr << "snmpagent_impl::set_interval arguments:";
                        debugstr << boost::format("in_interval=%d") % in_interval;
                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 45, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/
                interval = in_interval;
        }

        /*!
         *  refresh all the statistics.
         *
         * @return         void
         */
        void snmpagent_impl::refresh_all_statistics()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 46, "snmpagent_impl::refresh_all_statistics", __FILE__, __LINE__);

                if (vsd == NULL) {
                        //mib collect failed
                        std::string msg("vsd pointer is NULL.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 7, msg, __FILE__, __LINE__);
                        return;
                }

                boost::mutex  &virtualservice_list_mutex = vsd->get_virtualservice_list_mutex();

                boost::mutex::scoped_lock lock(virtualservice_list_mutex);

                l7vsd::vslist_type  &virtualservice_list = vsd->get_virtualservice_list();

                for (l7vsd::vslist_type::iterator it = virtualservice_list.begin();
                                it != virtualservice_list.end(); it++) {

                        //clear real server inactive count
                        (*it)->clear_inact();
                        protocol_module_base   *protocol_module_ptr = (*it)->get_protocol_module();
                        stats_base& bstats = protocol_module_ptr->get_stats();
                        if ( bstats.get_mode() == stats_base::MODE_HTTP ) {
                                /*-------- DEBUG LOG --------*/
                                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                        std::string  debugstr =  "function : snmpagent_impl::refresh_all_statistics :"
                                                                 "stats_base::MODE_HTTP mode virtual service.";
                                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 47, debugstr, __FILE__, __LINE__);
                                }
                                /*------ DEBUG LOG END ------*/

                                http_stats &hstats = static_cast<http_stats&>(bstats);
                                hstats.http_requests = 0ULL;
                                hstats.http_get_requests = 0ULL;
                                hstats.http_post_requests = 0ULL;
                        }
                }
        }

        /*!
         *  refresh a special virtual service's statistics.
         *
         * @param[in]      vs_endpoint is virtual service's endpoint
         * @param[in]      protocol is virtual service's protocol
         * @return         void
         */
        void snmpagent_impl::refresh_statistics(const boost::asio::ip::tcp::endpoint&  vs_endpoint, const std::string&  protocol)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 48, "snmpagent_impl::refresh_statistics", __FILE__, __LINE__);

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream    debugstr;
                        debugstr << "snmpagent_impl::refresh_statistics arguments:";
                        if (vs_endpoint.address().is_v6()) {
                                debugstr << boost::format("vs_endpoint=[%s]:%d") % vs_endpoint.address().to_string() % vs_endpoint.port();
                        } else {
                                debugstr << boost::format("vs_endpoint=%s:%d") % vs_endpoint.address().to_string() % vs_endpoint.port();
                        }
                        debugstr << boost::format(", protocol=%s") % protocol;
                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 49, debugstr.str(), __FILE__, __LINE__);
                }

                if (vsd == NULL) {
                        //mib collect failed
                        std::string msg("vsd pointer is NULL.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 8, msg, __FILE__, __LINE__);
                        return;
                }

                boost::mutex  &virtualservice_list_mutex = vsd->get_virtualservice_list_mutex();

                boost::mutex::scoped_lock lock(virtualservice_list_mutex);
                l7vsd::vslist_type  &virtualservice_list = vsd->get_virtualservice_list();

                virtualservice_element  in_vselement;
                in_vselement.udpmode              = 0;
                in_vselement.protocol_module_name = protocol;
                in_vselement.tcp_accept_endpoint  = vs_endpoint;

                l7vsd::vslist_type::iterator   it = vsd->search_vslist(in_vselement, true);

                if (it != virtualservice_list.end()) {
                        /*-------- DEBUG LOG --------*/
                        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                std::string  debugstr =  "function : snmpagent_impl::refresh_statistics :"
                                                         "input virtual service has found.";
                                Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 50, debugstr, __FILE__, __LINE__);
                        }
                        /*------ DEBUG LOG END ------*/

                        //clear real server inactive count
                        (*it)->clear_inact();

                        protocol_module_base   *protocol_module_ptr = (*it)->get_protocol_module();
                        stats_base& base_stats = protocol_module_ptr->get_stats();
                        if ( base_stats.get_mode() == stats_base::MODE_HTTP ) {
                                /*-------- DEBUG LOG --------*/
                                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                        std::string  debugstr =  "function : snmpagent_impl::refresh_statistics :"
                                                                 "stats_base::MODE_HTTP mode virtual service.";
                                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 51, debugstr, __FILE__, __LINE__);
                                }
                                /*------ DEBUG LOG END ------*/

                                http_stats &httpstats = static_cast<http_stats&>(base_stats);
                                //clear http statistics
                                httpstats.http_requests = 0ULL;
                                httpstats.http_get_requests = 0ULL;
                                httpstats.http_post_requests = 0ULL;

                        }
                }
        }

        /*!
         *  set vsd information.
         *
         * @param[in]      vsd is vsd pointer.
         * @retrun         void
         */
        void snmpagent_impl::set_vsd_info(l7vsd* in_vsd)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 52, "snmpagent_impl::set_vsd_info", __FILE__, __LINE__);

                vsd = in_vsd;
        }

        /*!
         *  get snmp information.
         *
         * @retrun         snmp_info is snmp information.
         */
        snmp_info snmpagent_impl::get_snmp_info()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 53, "snmpagent_impl::get_snmp_info", __FILE__, __LINE__);
                snmp_info snmpinfo;

                //push the local variable into snmp info
                snmpinfo.enabled           = enabled.get();
                snmpinfo.interval          = interval.get();
                snmpinfo.logtrap_enabled   = logtrap_enabled;
                snmpinfo.logtrap_level     = logtrap_level;
                snmpinfo.request_last_date = request_last_date.get();
                snmpinfo.snmp_get_requests = snmp_get_requests.get();
                snmpinfo.snmp_set_requests = snmp_set_requests.get();
                snmpinfo.snmp_trap_count   = snmp_trap_count.get();
                snmpinfo.start_date        = start_date.get();
                snmpinfo.trap_last_date    = trap_last_date.get();
                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::ostringstream  debugstr;
                        debugstr << "function : snmpagent_impl::get_snmp_info : snmpinfo : ";
                        debugstr << snmpinfo << ".";

                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 54, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/
                return snmpinfo;
        }

        /*!
         * load configure file setting.
         *
         * @retrun  void
         */
        void snmpagent_impl::load_config()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 55, "snmpagent_impl::load_config", __FILE__, __LINE__);
                l7vs::Parameter        param;
                l7vs::error_code    err;

                std::string  tmp_str = param.get_string(l7vs::PARAM_COMP_SNMPAGENT, "enable", err);
                if (!err) {
                        if (tmp_str == "true") {
                                enabled = TRUE;
                        } else if (tmp_str == "false") {
                                enabled = FALSE;
                        } else {
                                enabled = FALSE;
                                std::string msg("enable parameter value is invalid. Use default value.");
                                Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 1, msg, __FILE__, __LINE__);
                        }
                } else {
                        enabled = FALSE;
                        std::string msg("enable parameter not found. Use default value.");
                        Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 2, msg, __FILE__, __LINE__);
                }

                int tmp_int = param.get_int(l7vs::PARAM_COMP_SNMPAGENT, "trap_queue_polling_interval", err);

                if (!err) {
                        if ((tmp_int > 0) && (tmp_int < SNMP_MAX_TRAP_POLLING_INTEVAL)) {
                                trap_polling_interval = tmp_int;
                        } else {
                                trap_polling_interval = SNMP_DEFAULT_TRAP_POLLING_INTEVAL;
                                std::string msg("trap_queue_polling_interval parameter value is invalid. Use default value.");
                                Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 3, msg, __FILE__, __LINE__);
                        }
                } else {
                        trap_polling_interval = SNMP_DEFAULT_TRAP_POLLING_INTEVAL;
                        std::string msg("trap_queue_polling_interval parameter not found. Use default value.");
                        Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 4, msg, __FILE__, __LINE__);
                }

                tmp_int = param.get_int(l7vs::PARAM_COMP_SNMPAGENT, "trap_queue_max_size", err);
                if (!err) {
                        if (tmp_int > 0) {
                                trap_queue_max_size = tmp_int;
                        } else {
                                trap_queue_max_size = SNMP_DEFAULT_TRAP_QUEUE_MAX_SIZE;
                                std::string msg("trap_queue_max_size parameter value is invalid. Use default value.");
                                Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 5, msg, __FILE__, __LINE__);
                        }
                } else {
                        trap_queue_max_size = SNMP_DEFAULT_TRAP_QUEUE_MAX_SIZE;
                        std::string msg("trap_queue_max_size parameter not found. Use default value.");
                        Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 6, msg, __FILE__, __LINE__);
                }

                tmp_int = param.get_int(l7vs::PARAM_COMP_SNMPAGENT, "cache_update_interval", err);
                if (!err) {
                        if (tmp_int >= 0) {
                                interval = tmp_int;
                        } else {
                                interval = 1;
                                std::string msg("cache_update_interval parameter value is invalid. Use default value.");
                                Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 7, msg, __FILE__, __LINE__);
                        }
                } else {
                        interval = 1;
                        std::string msg("cache_update_interval parameter not found. Use default value.");
                        Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 8, msg, __FILE__, __LINE__);
                }

                tmp_str = param.get_string(l7vs::PARAM_COMP_SNMPAGENT, "logtrap", err);
                if (!err) {
                        if (tmp_str == "true") {
                                logtrap_enable();
                        } else if (tmp_str == "false") {
                                logtrap_disable();
                        } else {
                                logtrap_disable();
                                std::string msg("logtrap parameter value is invalid. Use default value.");
                                Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 9, msg, __FILE__, __LINE__);
                        }
                } else {
                        logtrap_disable();
                        std::string msg("logtrap parameter not found. Use default value.");
                        Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 10, msg, __FILE__, __LINE__);
                }

                tmp_str = param.get_string(l7vs::PARAM_COMP_SNMPAGENT, "logtrap_level", err);
                if (!err) {
                        string_loglevel_map_type::iterator itr = string_loglevel_map.find(tmp_str);
                        if (itr != string_loglevel_map.end()) {
                                set_logtrap_level(itr->second);
                        } else {
                                set_logtrap_level( LOG_LV_WARN);
                                std::string msg("logtrap_level parameter value is invalid. Use default value.");
                                Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 11, msg, __FILE__, __LINE__);
                        }
                } else {
                        set_logtrap_level(LOG_LV_WARN);
                        std::string msg("logtrap_level parameter not found. Use default value.");
                        Logger::putLogWarn(LOG_CAT_L7VSD_SNMPAGENT, 12, msg, __FILE__, __LINE__);
                }

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream debugstr;
                        debugstr << "function : snmpagent_impl::load_config : ";
                        debugstr << "enabled = ["                   << enabled.get()                     << "], ";
                        debugstr << "interval = ["                  << interval.get()                    << "], ";
                        debugstr << "logtrap_enabled = ["           << logtrap_enabled                   << "], ";
                        debugstr << "logtrap_level = ["             << static_cast<int>(logtrap_level)   << "], ";
                        debugstr << "trap_polling_interval = ["     << trap_polling_interval.get()       << "], ";
                        debugstr << "trap_queue_max_size = ["       << trap_queue_max_size.get()         << "]. ";

                        Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_SNMPAGENT, 56, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/
        }

        /*!
         *  collect mib data.
         *
         * @param[in]      error_code is err code.
         * @retrun         void
         */
        void snmpagent_impl::collect_mibdata(error_code&  err)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 57, "snmpagent_impl::collect_mibdata", __FILE__, __LINE__);

                //check vsd pointer
                if (vsd == NULL) {
                        std::string msg("vsd pointer is NULL.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 9, msg, __FILE__, __LINE__);
                        //set error code
                        err.setter(true, msg);
                        return;
                }

                //check timeout
                if (!check_timeout()) {

                        //collect mib data
                        mibdata::get_instance().collect_mibdata(vsd, err);

                        if (err) {
                                std::string msg("collect mib data failed.");
                                Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 10, msg, __FILE__, __LINE__);
                                //set error code
                                err.setter(true, msg);
                                return;
                        }

                        boost::posix_time::ptime now_time(boost::posix_time::second_clock::local_time());
                        mib_collect_last_time = now_time;

                }
        }

        /*!
         *  finalize.
         *
         * @retrun         void
         */
        void snmpagent_impl::finalize()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 58, "snmpagent_impl::finalize", __FILE__, __LINE__);

                if (start_flag == true) {
                        stop();
                }
                snmp_shutdown("l7vsAgent");
                SOCK_CLEANUP;
        }

        /*!
         *  increment get request count.
         *
         * @retrun         void
         */
        void snmpagent_impl::increment_getrequest_count()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 59, "snmpagent_impl::increment_getrequest_count", __FILE__, __LINE__);
                snmp_get_requests++;

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::ostringstream  debugstr;
                        debugstr << "function : snmpagent_impl::increment_getrequest_count : snmp_get_requests = ";
                        debugstr << snmp_get_requests.get() << ".";

                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 60, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/
        }

        /*!
         *  increment set request count.
         *
         * @retrun         void
         */
        void snmpagent_impl::increment_setrequest_count()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 61, "snmpagent_impl::increment_setrequest_count", __FILE__, __LINE__);
                snmp_set_requests++;

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::ostringstream  debugstr;
                        debugstr << "function : snmpagent_impl::increment_getrequest_count : snmp_set_requests = ";
                        debugstr << snmp_set_requests.get() << ".";

                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 62, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/
        }

        /*!
         *  update last request date.
         *
         * @retrun         void
         */
        void snmpagent_impl::update_last_request_date()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 59, "snmpagent_impl::update_last_request_date", __FILE__, __LINE__);
                //update last request date
                request_last_date = time(NULL);

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::ostringstream  debugstr;
                        debugstr << "function : snmpagent_impl::update_last_request_date : request_last_date = ";
                        debugstr << request_last_date.get() << ".";

                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 60, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/
        }
}
