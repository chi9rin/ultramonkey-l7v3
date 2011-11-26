/*!
 * @file  trapmessage.h
 * @brief snmp trap message header.
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
 */

#ifndef __TRAPMESSAGE_H__
#define __TRAPMESSAGE_H__

#include "error_code.h"

namespace l7vs
{
//! snmp trap message struct
struct trapmessage {
        enum TRAP_TYPE_TAG {
                SERVICE_START = 0,
                SERVICE_STOP,
                VIRTUALSERVICE_ADD,
                VIRTUALSERVICE_CHANGE,
                VIRTUALSERVICE_REMOVE,
                REALSERVER_ADD,
                REALSERVER_CHANGE,
                REALSERVER_REMOVE,
                UPQOS_ALERT_ON,
                UPQOS_ALERT_OFF,
                DOWNQOS_ALERT_ON,
                DOWNQOS_ALERT_OFF,
                SESSIONPOOL_ALERT_ON,
                SESSIONPOOL_ALERT_OFF,
                DEBUG_LOG,
                INFO_LOG,
                WARN_LOG,
                ERROR_LOG,
                FATAL_LOG
        };

        std::string    message;
        TRAP_TYPE_TAG  type;
};

//! typedef snmp send trap func type
typedef boost::function<void(trapmessage &, error_code &)>
snmp_send_trap_func_type;
}

#endif //__TRAPMESSAGE_H__

