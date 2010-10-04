/*!
 * @file  snmpagent.h
 * @brief snmpagent module.
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
#ifndef __SNMPAGENT_H__
#define __SNMPAGENT_H__

#include "l7vsd.h"
#include "snmpagent_impl.h"

namespace l7vs
{

//! @class    snmpagent
//! @brief    this class is snmpagent class.
        class snmpagent : private boost::noncopyable
        {
        public:
                /*!
                 *  constructor.
                 *
                 * @param[in]       l7vsd pointer
                 */
                snmpagent(l7vsd *);

                /*!
                 *  destructor.
                 *
                 */
                ~snmpagent() {};

                /*!
                 *  push the trap message into trap message queue.
                 *
                 * @param[in]      in_trapmessage is trap message
                 * @param[in/out]  err is error code
                 * @retrun         void
                 */
                static void    push_trapmessage(trapmessage &in_trapmessage, error_code& err);

                /*!
                 *  set snmp function enable
                 *
                 * @retrun         void
                 */
                static void    enable();

                /*!
                 *  set snmp function disable
                 *
                 * @retrun         void
                 */
                static void    disable();

                /*!
                 *  set mib cache update interval.
                 *
                 * @param[in]      in_interval is cache update interval
                 * @retrun         void
                 */
                static void    set_interval(const unsigned int in_interval);

                /*!
                 * set log trap enable
                 * @retrun  void
                 */
                static void    logtrap_enable();

                /*!
                 * set log trap disable
                 * @retrun  void
                 */
                static void    logtrap_disable();

                /*!
                 *  set log trap level.
                 *
                 * @param[in]      in_level is log trap level
                 * @retrun         void
                 */
                static void    set_logtrap_level(const LOG_LEVEL_TAG in_level);

                /*!
                 *  get snmp information.
                 *
                 * @return         snmp information
                 */
                static snmp_info  get_snmp_info();

                /*!
                 *  start snmp function.
                 *
                 * @param[in/out]  error_code is error code
                 * @return         void
                 */
                static void start(error_code&);

                /*!
                 *  stop snmp function.
                 *
                 * @return         void
                 */
                static void stop();

                /*!
                 *  refresh all the statistics.
                 *
                 * @return         void
                 */
                static void refresh_all_statistics();

                /*!
                 *  refresh a special virtual service's statistics.
                 *
                 * @param[in]      vs_endpoint is virtual service's endpoint
                 * @param[in]      protocol is virtual service's protocol
                 * @return         void
                 */
                static void refresh_statistics(const boost::asio::ip::tcp::endpoint &vs_endpoint,
                                               const std::string &protocol);
                /*!
                 *  finalize.
                 *
                 * @return  void
                 */
                void finalize();
        protected:
                /*!
                 *  constructor.
                 *
                 */
                snmpagent(){};

        };
}

#endif //__SUBAGENT_H__
