/*!
 *    @file    snmpagent.cpp
 *    @brief    snmpagent function class
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

#include "snmpagent.h"

namespace l7vs
{

/*!
 *  constructor.
 *
 * @param[in]       l7vsd pointer
 */
snmpagent::snmpagent(l7vsd *in_vsd)
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::snmpagent", __FILE__, __LINE__);

        error_code err;
        snmpagent_impl::get_instance().init(err);
        if (err) {
                std::string msg("snmpagent initialize failed.program exit.");
                Logger::putLogFatal(LOG_CAT_L7VSD_SNMPAGENT, 1, msg, __FILE__, __LINE__);
                exit(1);
        }
        snmpagent_impl::get_instance().set_vsd_info(in_vsd);
}

/*!
 *  push the trap message into trap message queue.
 *
 * @param[in]      in_trapmessage is trap message
 * @param[in/out]  err is error code
 * @retrun         void
 */
void    snmpagent::push_trapmessage(trapmessage &in_trapmessage, error_code &err)
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::push_trapmessage", __FILE__, __LINE__);

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                std::stringstream    debugstr;
                debugstr << "snmpagent::push_trapmessage arguments:";
                debugstr << boost::format("in_trapmessage.type=%d") % in_trapmessage.type;
                debugstr << boost::format("in_trapmessage.message=%s") % in_trapmessage.message;
                Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 99999, debugstr.str(), __FILE__, __LINE__);
        }
        /*------ DEBUG LOG END ------*/

        snmpagent_impl::get_instance().push_trapmessage(in_trapmessage, err);
}

/*!
 *  set snmp function enable
 *
 * @retrun         void
 */
void    snmpagent::enable()
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::enable", __FILE__, __LINE__);

        snmpagent_impl::get_instance().enable();
}

/*!
 *  set snmp function disable
 *
 * @retrun         void
 */
void    snmpagent::disable()
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::disable", __FILE__, __LINE__);

        snmpagent_impl::get_instance().disable();
}

/*!
 *  set mib cache update interval.
 *
 * @param[in]      in_interval is cache update interval
 * @retrun         void
 */
void    snmpagent::set_interval(const unsigned int in_interval)
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::set_interval", __FILE__, __LINE__);

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                std::stringstream    debugstr;
                debugstr << "snmpagent::set_interval arguments:";
                debugstr << boost::format("in_interval=%d") % in_interval;
                Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 99999, debugstr.str(), __FILE__, __LINE__);
        }
        /*------ DEBUG LOG END ------*/

        snmpagent_impl::get_instance().set_interval(in_interval);
}

/*!
 * set log trap enable
 * @retrun  void
 */
void    snmpagent::logtrap_enable()
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent_impl::logtrap_enable", __FILE__, __LINE__);

        snmpagent_impl::get_instance().logtrap_enable();
}

/*!
 * set log trap disable
 * @retrun  void
 */
void    snmpagent::logtrap_disable()
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent_impl::logtrap_disable", __FILE__, __LINE__);

        snmpagent_impl::get_instance().logtrap_disable();
}

/*!
 *  set log trap level.
 *
 * @param[in]      in_level is log trap level
 * @retrun         void
 */
void   snmpagent::set_logtrap_level(const LOG_LEVEL_TAG in_level)
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::set_logtrap_level", __FILE__, __LINE__);

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                std::stringstream    debugstr;
                debugstr << "snmpagent::set_logtrap_level arguments:";
                debugstr << boost::format("in_level=%d") % in_level;
                Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 99999, debugstr.str(), __FILE__, __LINE__);
        }
        /*------ DEBUG LOG END ------*/

        snmpagent_impl::get_instance().set_logtrap_level(in_level);
}

/*!
 *  get snmp information.
 *
 * @return         snmp information
 */
snmp_info  snmpagent::get_snmp_info()
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::get_snmp_info", __FILE__, __LINE__);

        return snmpagent_impl::get_instance().get_snmp_info();
}

/*!
 *  start snmp function.
 *
 * @param[in/out]  error_code is error code
 * @return         void
 */
void snmpagent::start(error_code &err)
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::start", __FILE__, __LINE__);
        snmpagent_impl::get_instance().start(err);
}

/*!
 *  stop snmp function.
 *
 * @return         void
 */
void snmpagent::stop()
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::stop", __FILE__, __LINE__);
        snmpagent_impl::get_instance().stop();
}

/*!
 *  refresh all the statistics.
 *
 * @return         void
 */
void snmpagent::refresh_all_statistics()
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::refresh_all_statistics", __FILE__, __LINE__);
        snmpagent_impl::get_instance().refresh_all_statistics();
}

/*!
 *  refresh a special virtual service's statistics.
 *
 * @param[in]      vs_endpoint is virtual service's endpoint
 * @param[in]      protocol is virtual service's protocol
 * @return         void
 */
void snmpagent::refresh_statistics(const boost::asio::ip::tcp::endpoint &vs_endpoint, const std::string &protocol)
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::refresh_statistics", __FILE__, __LINE__);

        /*-------- DEBUG LOG --------*/
        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                std::stringstream    debugstr;
                debugstr << "snmpagent::refresh_statistics arguments:";
                if (vs_endpoint.address().is_v6()) {
                        debugstr << boost::format("vs_endpoint=[%s]:%d") % vs_endpoint.address().to_string() % vs_endpoint.port();
                } else {
                        debugstr << boost::format("vs_endpoint=%s:%d") % vs_endpoint.address().to_string() % vs_endpoint.port();
                }
                debugstr << boost::format("protocol=%s") % protocol;
                Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 99999, debugstr.str(), __FILE__, __LINE__);
        }
        /*------ DEBUG LOG END ------*/
        snmpagent_impl::get_instance().refresh_statistics(vs_endpoint, protocol);
}

/*!
 *  finalize.
 *
 * @return  void
 */
void snmpagent::finalize()
{
        Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 99999, "snmpagent::finalize", __FILE__, __LINE__);

        snmpagent_impl::get_instance().finalize();
}

}
