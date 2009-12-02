/*!
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

#include <log4cxx/level.h>

namespace l7vs{

//!	@enum	LOG_LV_TAG
//!	@brief	LogLevel enumeration.
enum LOG_LEVEL_TAG	{
	LOG_LV_NONE = 0,	//!< loglevel is none
	LOG_LV_DEBUG,		//!< loglevel is debug
	LOG_LV_INFO,		//!< loglevel is info
	LOG_LV_WARN,		//!< loglevel is warn
	LOG_LV_ERROR,		//!< loglevel is error
	LOG_LV_FATAL		//!< loglevel is fatal
};

//! loglevel enum incliment function
//! @param[in]	logleveltag refarence
//!	@return		logleveltag reference
inline LOG_LEVEL_TAG& operator++(LOG_LEVEL_TAG& level) {
	level = static_cast<LOG_LEVEL_TAG>(level + 1);
	return level;
}

//!	@enum	LOG_CATEGORY_TAG
//!	@brief	Category enumeration.
//!	@brief	LOG_CAT_SSLPROXY_LOGGER should be first for logger log.
enum LOG_CATEGORY_TAG{
	LOG_CAT_NONE = 0,						//!< logcategory is none
	LOG_CAT_L7VSD_NETWORK,					//!< logcategory is network
	LOG_CAT_L7VSD_NETWORK_QOS,				//!< logcategory is QoS
	LOG_CAT_L7VSD_NETWORK_BANDWIDTH,		//!< logcategory is bps
	LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION,	//!< logcategory is connection count
	LOG_CAT_L7VSD_NETWORK_ACCESS,			//!< logcategory is access log
	LOG_CAT_L7VSD_MAINTHREAD,				//!< logcategory is mainthread
	LOG_CAT_L7VSD_VIRTUALSERVICE,			//!< logcategory is virtualservice
	LOG_CAT_L7VSD_VIRTUALSERVICE_THRAD,		//!< logcategory is virtualservice thread
	LOG_CAT_L7VSD_SESSION,					//!< logcategory is session
	LOG_CAT_L7VSD_SESSION_THREAD,			//!< logcategory is session thread
	LOG_CAT_L7VSD_REALSERVER,				//!< logcategory is realserver
	LOG_CAT_L7VSD_SORRYSERVER,				//!< logcategory is sollyserver
	LOG_CAT_L7VSD_MODULE,					//!< logcategory is module controler
	LOG_CAT_L7VSD_REPLICATION,				//!< logcategory is replication
	LOG_CAT_L7VSD_REPLICATION_SENDTHREAD,	//!< logcategory is replication send thread
	LOG_CAT_L7VSD_PARAMETER,				//!< logcategory is parameter
	LOG_CAT_L7VSD_LOGGER,					//!< logcategory is logger
	LOG_CAT_L7VSD_COMMAND,					//!< logcategory is command reciver
	LOG_CAT_L7VSD_START_STOP,				//!< logcategory is main function used
	LOG_CAT_L7VSD_SYSTEM,					//!< logcategory is systemcall other
	LOG_CAT_L7VSD_SYSTEM_MEMORY,			//!< logcategory is systemcall memory
	LOG_CAT_L7VSD_SYSTEM_ENDPOINT,			//!< logcategory is systemcall socket 
	LOG_CAT_L7VSD_SYSTEM_SIGNAL,			//!< logcategory is systemcall signal
	LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT,		//!< logcategory is systemcall env

	LOG_CAT_PROTOCOL,						//!< logcategory is protocol.
	LOG_CAT_SCHEDULE,						//!< logcategory is schedule

	LOG_CAT_L7VSADM_PARSE,					//!< logcategory is parse
	LOG_CAT_L7VSADM_OPERATE,				//!< logcategory is operate
	LOG_CAT_L7VSADM_CONFIG_RESULT,			//!< logcategory is configure
	LOG_CAT_L7VSADM_COMMON,					//!< logcategory is other
	LOG_CAT_L7VSADM_LOGGER,					//!< logcategory is logger
	LOG_CAT_L7VSADM_PARAMETER,				//!< logcategory is parameter
	LOG_CAT_L7VSADM_MODULE,					//!< logcategory is module control

	LOG_CAT_SNMPAGENT_START_STOP,			//!< logcategory is snmpagent main
	LOG_CAT_SNMPAGENT_MANAGER_RECEIVE,		//!< logcategory is manager send
	LOG_CAT_SNMPAGENT_MANAGER_SEND,			//!< logcategory is manager recv
	LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,		//!< logcategory is snmpbrigde recv
	LOG_CAT_SNMPAGENT_L7VSD_SEND,			//!< logcategory is snmpbrigre send
	LOG_CAT_SNMPAGENT_LOGGER,				//!< logcategory is manager logger
	LOG_CAT_SNMPAGENT_PARAMETER,			//!< logcategory is manager parameter
	LOG_CAT_SNMPAGENT_SYSTEM,				//!< logcategory is snmpagent systemcall other
	LOG_CAT_SNMPAGENT_SYSTEM_MEMORY,		//!< logcategory is snmpagent systemcall memory
	LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT,		//!< logcategory is snmpagent systemcall socket 
	LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL,		//!< logcategory is snmpagent systemcall signal
	LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT,	//!< logcategory is snmpagent systemcall env

	LOG_CAT_SSLPROXY_LOGGER,				//!< logcategory is snmpproxy logger
	LOG_CAT_SSLPROXY_PARAMETER,				//!< logcategory is snmpproxy parameter
	LOG_CAT_SSLPROXY_COMMON,				//!< logcategory is snmpproxy common
	LOG_CAT_SSLPROXY_SERVER,				//!< logcategory is snmpproxy server
	LOG_CAT_SSLPROXY_SESSION,				//!< logcategory is snmpproxy session
	LOG_CAT_SSLPROXY_CONNECTION,			//!< logcategory is snmpproxy connection
	LOG_CAT_END,								//!< end of logcategory
};


//! logcategory enum incliment function
//! @param[in]	logcategorytag refarence
//!	@return		logcategorytag reference
inline LOG_CATEGORY_TAG& operator++(LOG_CATEGORY_TAG& cat) {
	cat = static_cast<LOG_CATEGORY_TAG>(cat + 1);
	return cat;
}

} //namespace l7vs
#endif	//__LOGGER_ENUM_H__
