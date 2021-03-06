/*!
 * @file  snmpfunc.h
 * @brief snmp function module.
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
/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.create-dataset.conf 17337 2009-01-01 14:28:29Z magfr $
 *        : mib2c.scalar.conf 17337 2009-01-01 14:28:29Z magfr $
 *        : mib2c.notify.conf 17455 2009-04-05 09:53:29Z magfr $
 */
#ifndef __SNMPFUNC_H__
#define __SNMPFUNC_H__

#include <error_code.h>
#include <string>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

using namespace l7vs;
/*
 * function declarations
 */

/*!
 * handle for virtual service count.
 *
 */
int        handle_get_vstable_count(netsnmp_mib_handler *handler,
                                    netsnmp_handler_registration *reginfo,
                                    netsnmp_agent_request_info *reqinfo,
                                    netsnmp_request_info *requests);

/*!
 * handle for replication state.
 *
 */
int        handle_get_rep_state(netsnmp_mib_handler *handler,
                                netsnmp_handler_registration *reginfo,
                                netsnmp_agent_request_info *reqinfo,
                                netsnmp_request_info *requests);

/*!
 * handle for virtual service table.
 *
 */
int        handle_get_vstable(netsnmp_mib_handler *handler,
                              netsnmp_handler_registration *reginfo,
                              netsnmp_agent_request_info *reqinfo,
                              netsnmp_request_info *requests);

/*!
 * handle for real server table.
 *
 */
int        handle_get_rstable(netsnmp_mib_handler *handler,
                              netsnmp_handler_registration *reginfo,
                              netsnmp_agent_request_info *reqinfo,
                              netsnmp_request_info *requests);

/*!
 * initialize snmp scaler and table data set handles.
 *
 * @param[in]      error_code& is trap error code
 * @retrun         void
 */
void    init_snmp_handles(error_code &err);

/*!
 * unregister snmp handler.
 *
 */
void    unregister_handler();

/*!
 * initialize virtual service table data set.
 *
 * @param[in]      error_code& is trap error code
 * @retrun         void
 */
void    initialize_virtual_service_table(error_code &err);

/*!
 * initialize real server table data set.
 *
 * @param[in]      error_code& is trap error code
 * @retrun         void
 */
void    initialize_real_server_table(error_code &err);

/*!
 * send service start trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_service_start(const std::string &);

/*!
 * send service stop trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_service_stop(const std::string &);

/*!
 * send virtual service add trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_vs_add(const std::string &);

/*!
 * send virtual service change trap.
 *
 * @param[in]      std::string is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_vs_change(const std::string &);

/*!
 * send virtual service remove trap.
 *
 * @param[in]      std::string is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_vs_remove(const std::string &);

/*!
 * send real server add trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_rs_add(const std::string &);

/*!
 * send real server change trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_rs_change(const std::string &);

/*!
 * send real server remove trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_rs_remove(const std::string &);

/*!
 * send upstream QoS alert on trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_up_qos_alert_on(const std::string &);

/*!
 * send upstream QoS alert off trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_up_qos_alert_off(const std::string &);

/*!
 * send downstream QoS alert on trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_down_qos_alert_on(const std::string &);

/*!
 * send downstream QoS alert off trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_down_qos_alert_off(const std::string &);

/*!
 * send session pool alert on trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_sessionpool_alert_on(const std::string &);

/*!
 * send session pool alert off trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_sessionpool_alert_off(const std::string &);

/*!
 * send fatal log trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_fatal(const std::string &);

/*!
 * send error log trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_error(const std::string &);

/*!
 * send warn log trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_warn(const std::string &);

/*!
 * send info log trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_info(const std::string &);

/*!
 * send debug log trap.
 *
 * @param[in]      const std::string& is trap message
 * @retrun         int  success:SNMP_ERR_NOERROR  failed:SNMP_ERR_GENERR
 */
int     trap_debug(const std::string &);

/*
 * column number definitions for table l7vsVsTable
 */
#define COLUMN_L7VSVSINDEX                    (1)
#define COLUMN_L7VSVSPROTOCOL                (2)
#define COLUMN_L7VSVSADDRTYPE                (3)
#define COLUMN_L7VSVSADDR                    (4)
#define COLUMN_L7VSVSPORT                    (5)
#define COLUMN_L7VSVSPROTOCOLMODULENAME        (6)
#define COLUMN_L7VSVSPROTOCOLMODULEOPTIONS    (7)
#define COLUMN_L7VSVSSCHEDULEMODULENAME        (8)
#define COLUMN_L7VSVSSCHEDULEMODULEOPTIONS    (9)
#define COLUMN_L7VSVSSORRYADDRTYPE            (10)
#define COLUMN_L7VSVSSORRYADDR                (11)
#define COLUMN_L7VSVSSORRYPORT                (12)
#define COLUMN_L7VSVSSORRYCONNLIMIT            (13)
#define COLUMN_L7VSVSSORRYFORCEENABLED        (14)
#define COLUMN_L7VSVSTHROUGHPUTUP            (15)
#define COLUMN_L7VSVSTHROUGHPUTDOWN            (16)
#define COLUMN_L7VSVSTHROUGHPUTUPQOS        (17)
#define COLUMN_L7VSVSTHROUGHPUTDOWNQOS        (18)
#define COLUMN_L7VSVSSSLFLAG                (19)
#define COLUMN_L7VSVSSESSIONPOOLCOUNT        (20)
#define COLUMN_L7VSVSSESSIONACTIVECOUNT        (21)
#define COLUMN_L7VSVSSESSIONWAITCOUNT        (22)
#define COLUMN_L7VSVSHTTPTOTALCOUNT            (23)
#define COLUMN_L7VSVSHTTPGETCOUNT            (24)
#define COLUMN_L7VSVSHTTPPOSTCOUNT            (25)
#define COLUMN_L7VSVSRSNUMBER                (26)

/*
 * column number definitions for table l7vsRsTable
 */
#define COLUMN_L7VSRSINDEX                    (1)
#define COLUMN_L7VSRSVSINDEX                (2)
#define COLUMN_L7VSRSADDRTYPE                (3)
#define COLUMN_L7VSRSADDR                    (4)
#define COLUMN_L7VSRSPORT                    (5)
#define COLUMN_L7VSRSFORWARD                (6)
#define COLUMN_L7VSRSWEIGHT                    (7)
#define COLUMN_L7VSRSACTIVECONN                (8)
#define COLUMN_L7VSRSINACTIVECONN            (9)

#endif  //__SNMPFUNC_H__
