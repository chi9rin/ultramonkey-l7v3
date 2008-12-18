/*
 * @file  logger_enum.h
 * @brief logger module constants enumeration.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2008  NTT COMWARE Corporation.
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

#ifndef LOGGER_ENUM_H
#define LOGGER_ENUM_H

#include <string>
#include <log4cxx/level.h>

namespace l7vs{

/*!
 * LogLevel enumeration.
 */
enum LOG_LEVEL_TAG
{
	LOG_LV_NONE = 0,
	LOG_LV_DEBUG,
	LOG_LV_INFO,
	LOG_LV_WARN,
	LOG_LV_ERROR,
	LOG_LV_FATAL
};

inline LOG_LEVEL_TAG& operator++(LOG_LEVEL_TAG& level) {
	level = static_cast<LOG_LEVEL_TAG>(level + 1);
	return level;
}

/*!
 * Category enumeration.
 * LOG_CAT_SSLPROXY_LOGGER should be first for logger log.
 */
enum LOG_CATEGORY_TAG
{
	LOG_CAT_NONE = 0,

	LOG_CAT_L7VSD_NETWORK,
	LOG_CAT_L7VSD_NETWORK_QOS,
	LOG_CAT_L7VSD_NETWORK_BANDWIDTH,
	LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION,
	LOG_CAT_L7VSD_MAINTHREAD,
	LOG_CAT_L7VSD_VIRTUALSERVICE,
	LOG_CAT_L7VSD_VIRTUALSERVICE_THRAD,
	LOG_CAT_L7VSD_SESSION,
	LOG_CAT_L7VSD_SESSION_THREAD,
	LOG_CAT_L7VSD_SORRYSERVER,
	LOG_CAT_L7VSD_MODULE,
	LOG_CAT_L7VSD_SCHEDULE,
	LOG_CAT_L7VSD_PROTOCOL,
	LOG_CAT_L7VSD_REPLICATION,
	LOG_CAT_L7VSD_PARAMTER,
	LOG_CAT_L7VSD_LOGGER,
	LOG_CAT_L7VSD_COMMAND,
	LOG_CAT_L7VSD_START_STOP,
	LOG_CAT_L7VSD_SYSTEM,
	LOG_CAT_L7VSD_SYSTEM_MEMORY,
	LOG_CAT_L7VSD_SYSTEM_ENDPOINT,
	LOG_CAT_L7VSD_SYSTEM_SIGNAL,
	LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT,

	LOG_CAT_L7VSADM_PARSE,
	LOG_CAT_L7VSADM_OPERATE,
	LOG_CAT_l7VSADM_CONFIG_RESULT,
	LOG_CAT_L7VSADM_COMMON,
	LOG_CAT_L7VSADM_LOGGER,
	LOG_CAT_L7VSADM_PARAMETER,
	LOG_CAT_L7VSADM_PROTOCOL,
	LOG_CAT_L7VSADM_MODULE,

	LOG_CAT_SNMPAGENT_START_STOP,
	LOG_CAT_SNMPAGENT_MANAGER_RECEIVE,
	LOG_CAT_SNMPAGENT_MANAGER_SEND,
	LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,
	LOG_CAT_SNMPAGENT_L7VSD_SEND,
	LOG_CAT_SNMPAGENT_LOGGER,
	LOG_CAT_SNMPAGENT_PARAMETER,

	LOG_CAT_SSLPROXY_LOGGER,
	LOG_CAT_SSLPROXY_PARAMETER,
	LOG_CAT_SSLPROXY_COMMON,
	LOG_CAT_SSLPROXY_SERVER,
	LOG_CAT_SSLPROXY_SESSION,
	LOG_CAT_SSLPROXY_CONNECTION,
	LOG_CAT_END
};


inline LOG_CATEGORY_TAG& operator++(LOG_CATEGORY_TAG& cat) {
	cat = static_cast<LOG_CATEGORY_TAG>(cat + 1);
	return cat;
}

/*!
 * Rotation enumeration.
 */
enum LOG_ROTATION_TAG {
	LOG_ROT_SIZE,
	LOG_ROT_DATE,
	LOG_ROT_DATESIZE
};

/*!
 * RotationTiming enumeration.
 */
enum LOG_ROTATION_TIMING_TAG {
	LOG_TIM_YEAR,
	LOG_TIM_MONTH,
	LOG_TIM_WEEK,
	LOG_TIM_DATE,
	LOG_TIM_HOUR
};


}; //namespace l7vs	
#endif	//__LOGGER_ENUM_H__
