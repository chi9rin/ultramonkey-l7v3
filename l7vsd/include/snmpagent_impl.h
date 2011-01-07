/*!
 * @file  snmpagent_impl.h
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
#ifndef __SNMPAGENT_IMPL_H__
#define __SNMPAGENT_IMPL_H__

#include "l7vsd.h"
#include "snmp_info.h"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
#include <list>
#include <time.h>
#include <deque>
#include "error_code.h"
#include "boost/date_time/posix_time/posix_time.hpp"

namespace l7vs
{

//! @class    snmpagent_impl
//! @brief    this class is snmpagent implement class.
        class snmpagent_impl : private boost::noncopyable
        {

        public:

                //! trap function type
                typedef int trapfunc(const std::string&);

                //!trap function map type
                typedef std::map<trapmessage::TRAP_TYPE_TAG, trapfunc*> trapfunc_type;

                //!string log level map type
                typedef std::map<std::string, LOG_LEVEL_TAG> string_loglevel_map_type;
        protected:
                bool                        initialized;
                bool                        start_flag;
                l7vsd                       *vsd;
                trapfunc_type               trapfunc_map;
                atomic<unsigned int>        trap_polling_interval;
                atomic<unsigned int>        trap_queue_max_size;
                atomic<unsigned int>        enabled;
                unsigned int                logtrap_enabled;
                LOG_LEVEL_TAG               logtrap_level;
                atomic<unsigned int>        interval;
                atomic<time_t>              start_date;
                atomic<time_t>              request_last_date;
                atomic<time_t>              trap_last_date;
                atomic<unsigned long long>  snmp_get_requests;
                atomic<unsigned long long>  snmp_set_requests;
                atomic<unsigned long long>  snmp_trap_count;
                std::deque<trapmessage>     trap_msg_queue;
                boost::condition            trap_msg_queue_condition;
                boost::mutex                trap_msg_queue_condition_mutex;
                string_loglevel_map_type    string_loglevel_map;
                boost::posix_time::ptime    mib_collect_last_time;
                boost::thread               trap_thread;
                boost::thread               process_mib_thread;

                snmpagent_impl() {};
                ~snmpagent_impl() {};
                snmpagent_impl(const snmpagent_impl&);
                snmpagent_impl& operator=(const snmpagent_impl &);

                /*!
                 * process mib get item thread function.
                 *
                 * @retrun  void
                 */
                void    process_mib_run();

                /*!
                 * process trap thread function.
                 *
                 * @retrun  void
                 */
                void    trap_run();

                /*!
                 * check mib data timeout.
                 *
                 * @retrun  void
                 */
                bool    check_timeout();

                /*!
                 * load configure file setting.
                 *
                 * @retrun  void
                 */
                void    load_config();

        public:

                /*!
                 * create and return a snmpagent_impl instance.
                 *
                 * @retrun  snmpagent_impl & snmpagent_impl' instance
                 */
                static  snmpagent_impl &get_instance();

                /*!
                 * snmpagent_impl class initialize.
                 *
                 * @param   error_code & error code
                 * @retrun  void
                 */
                void    init(error_code &);

                /*!
                 *  push the trap message into trap message queue.
                 *
                 * @param[in]      in_trapmessage is trap message
                 * @param[in/out]  err is error code
                 * @retrun         void
                 */
                void    push_trapmessage(trapmessage &in_trapmessage, error_code& err);

                /*!
                 * snmpagent function start.
                 *
                 * @param[in/out]  error_code is error code
                 * @retrun  void
                 */
                void    start(error_code&);

                /*!
                 * snmpagent function stop.
                 *
                 * @retrun  void
                 */
                void    stop();

                /*!
                 *  set snmp function enable
                 *
                 * @retrun         void
                 */
                void    enable();

                /*!
                 *  set snmp function disable
                 *
                 * @retrun         void
                 */
                void    disable();

                /*!
                 * set log trap enable
                 * @retrun  void
                 */
                void    logtrap_enable();

                /*!
                 * set log trap disable
                 * @retrun  void
                 */
                void    logtrap_disable();

                /*!
                 *  set log trap level.
                 *
                 * @param[in]      in_level is log trap level
                 * @retrun         void
                 */
                void    set_logtrap_level(const LOG_LEVEL_TAG in_level);

                /*!
                 *  set mib cache update interval.
                 *
                 * @param[in]      in_interval is cache update interval
                 * @retrun         void
                 */
                void    set_interval(const unsigned int in_interval);

                /*!
                 *  refresh all the statistics.
                 *
                 * @return         void
                 */
                void    refresh_all_statistics();

                /*!
                 *  refresh a special virtual service's statistics.
                 *
                 * @param[in]      vs_endpoint is virtual service's endpoint
                 * @param[in]      protocol is virtual service's protocol
                 * @return         void
                 */
                void    refresh_statistics(const boost::asio::ip::tcp::endpoint &vs_endpoint, const std::string &protocol);

                /*!
                 *  set vsd information.
                 *
                 * @param[in]      vsd is vsd pointer.
                 * @retrun         void
                 */
                void    set_vsd_info(l7vsd *vsd);

                /*!
                 *  get snmp information.
                 *
                 * @retrun         snmp_info is snmp information.
                 */
                snmp_info get_snmp_info();

                /*!
                 *  collect mib data.
                 *
                 * @param[in]      error_code is err code.
                 * @retrun         void
                 */
                void    collect_mibdata(error_code &);

                /*!
                 *  finalize.
                 *
                 * @retrun         void
                 */
                void    finalize();

                /*!
                 *  increment get request count.
                 *
                 * @retrun         void
                 */
                void    increment_getrequest_count();

                /*!
                 *  increment set request count.
                 *
                 * @retrun         void
                 */
                void    increment_setrequest_count();

                /*!
                 *  update last request date.
                 *
                 * @retrun         void
                 */
                void    update_last_request_date();
        };

}

#endif //__SNMPAGENT_IMPL_H__

