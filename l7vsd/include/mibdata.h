/*!
 * @file  mibdata.h
 * @brief mibdata module.
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
#ifndef __MIBDATA_H__
#define __MIBDATA_H__

#include "error_code.h"
#include "l7vsd.h"
#include "snmp_info.h"
#include <vector>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#define   L7VS_IPADDR_LEN  (128)

namespace l7vs
{

//! @class    mibdata
//! @brief    this class is mib data cache class.
        class mibdata : private boost::noncopyable
        {
        protected:
                netsnmp_table_data_set*  vs_table;
                netsnmp_table_data_set*  rs_table;
                int  vs_table_size;
                int  rs_table_size;
                int  replication_state;

                mibdata() {};
                ~mibdata() {};
                mibdata(const mibdata &);
                mibdata& operator=(const mibdata &);

                /*!
                 * update virtual service table infomation.
                 *
                 * @param   l7vsd * l7vsd pointer
                 * @param   error_code & error code
                 * @retrun  void
                 */
                void  update_vs_table(l7vsd *, error_code &);

                /*!
                 * update real server table infomation.
                 *
                 * @param   const std::vector<rsdata> & temporary real server list
                 * @param   error_code & error code
                 * @retrun  void
                 */
                void  update_rs_table(const std::vector<rsdata> &, error_code &);

                /*!
                 * update replication state.
                 *
                 * @param   l7vsd * l7vsd pointer
                 * @param   error_code & error code
                 * @retrun  void
                 */
                void  update_replication_state(l7vsd *, error_code &);

        public:
                /*!
                 * get mibdata instance.
                 *
                 * @retrun  mibdata instance
                 */
                static mibdata&  get_instance();

                /*!
                 * collect mibdata in l7vsd.
                 *
                 * @param   l7vsd * l7vsd pointer
                 * @param   error_code & error code
                 * @retrun  void
                 */
                void  collect_mibdata(l7vsd *, error_code &);

                /*!
                 * set virtual service data set.
                 *
                 * @param   netsnmp_table_data_set * virtual service data set
                 * @retrun  void
                 */
                void  set_vs_table(netsnmp_table_data_set *);

                /*!
                 * set real server data set.
                 *
                 * @param   netsnmp_table_data_set * real server data set
                 * @retrun  void
                 */
                void  set_rs_table(netsnmp_table_data_set *);

                /*!
                 * get virtual service table size
                 *
                 * @retrun  int virtual service table size
                 */
                inline int   get_vs_table_size() {
                        return vs_table_size;
                }

                /*!
                 * get replication state.
                 *
                 * @retrun  int replication state
                 */
                inline int   get_replication_state() {
                        return replication_state;
                }
        };
}

#endif //__MIBDATA_H__
