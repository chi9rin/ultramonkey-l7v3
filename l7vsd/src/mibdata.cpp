/*!
 * @file  mibdata.cpp
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
#include "mibdata.h"
#include "snmpfunc.h"

#define SNMP_IPPROTO_TCP        (1)
#define SNMP_IPPROTO_UDP        (2)
#define SNMP_IPV4               (1)
#define SNMP_IPV6               (2)


namespace l7vs
{
        /*!
         * update virtual service table infomation.
         *
         * @param   l7vsd * l7vsd pointer
         * @param   error_code & error code
         * @retrun  void
         */
        void mibdata::update_vs_table(l7vsd *in_vsd, error_code&  err)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 63, "mibdata::update_vs_table", __FILE__, __LINE__);

                if (in_vsd == NULL) {
                        std::string msg("in_vsd pointer is NULL.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 11, msg, __FILE__, __LINE__);
                        //set error code
                        err.setter(true, msg);
                        return;
                }

                if (vs_table == NULL) {
                        std::string msg("vs_table pointer is NULL.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 12, msg, __FILE__, __LINE__);
                        //set error code
                        err.setter(true, msg);
                        return;
                }

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream    debugstr;
                        debugstr << "mibdata::update_vs_table arguments:";
                        debugstr << boost::format("in_vsd=%p") % in_vsd;
                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 64, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/

                oid   index[1] = {0};
                netsnmp_table_row *row = NULL;
                int vs_index = 0;
                std::vector<rsdata>  rs_vector;

                int protocol = 0;
                char vs_ipaddress[L7VS_IPADDR_LEN] = {0};
                char sorry_ipaddress[L7VS_IPADDR_LEN] = {0};
                int vs_ipaddress_type = SNMP_IPV4;
                int sorry_ipaddress_type = SNMP_IPV4;
                int vs_port = 0;
                int sorry_port = 0;
                int sorry_flag = 0;

                struct counter64 sorry_maxconnection = {
                        0
                };
                struct counter64 throughput_upstream = {
                        0
                };
                struct counter64 throughput_downstream = {
                        0
                };
                struct counter64 qos_upstream = {
                        0
                };
                struct counter64 qos_downstream = {
                        0
                };
                struct counter64 http_requests = {
                        0
                };
                struct counter64 http_get_requests = {
                        0
                };
                struct counter64 http_post_requests = {
                        0
                };

                int pool_session_count = 0;
                int active_session_count = 0;
                int waiting_session_count = 0;

                //get virtualservice list mutex
                boost::mutex &virtualservice_list_mutex = in_vsd->get_virtualservice_list_mutex();
                {
                        boost::mutex::scoped_lock lock(virtualservice_list_mutex);

                        l7vsd::vslist_type  &virtualservice_list = in_vsd->get_virtualservice_list();

                        //delete the surplus data from vs table
                        if (virtualservice_list.size() < static_cast<size_t>(vs_table_size)) {
                                for (size_t i = vs_table_size; i > virtualservice_list.size(); i--) {
                                        index[0] = i;
                                        row = netsnmp_table_data_get_from_oid(vs_table->table, index, 1);
                                        if (row == NULL) continue;
                                        netsnmp_table_dataset_remove_and_delete_row(vs_table, row);
                                }
                        }

                        //set virtual service table size
                        vs_table_size = virtualservice_list.size();

                        //update virtual service table
                        for (l7vsd::vslist_type::iterator it = virtualservice_list.begin();
                                        it != virtualservice_list.end(); it++) {
                                //increment virtual service index
                                vs_index++;
                                index[0] = vs_index;

                                //get row pointer
                                row = netsnmp_table_data_get_from_oid(vs_table->table, index, 1);
                                if (row == NULL) {
                                        row = netsnmp_create_table_data_row();
                                        if (row == NULL) {
                                                std::string msg("create virtualservice table row failed.");
                                                Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 13, msg, __FILE__, __LINE__);
                                                //set error code
                                                err.setter(true, msg);
                                                return;
                                        } else {

                                                netsnmp_table_row_add_index(row, ASN_INTEGER, (void*)index, sizeof(int));
                                                netsnmp_table_dataset_add_row(vs_table, row);
                                                /*-------- DEBUG LOG --------*/
                                                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                                        std::stringstream    debugstr;
                                                        debugstr << "function : mibdata::update_vs_table : vs_table add row.";
                                                        boost::format("vs_index=%d") % vs_index;

                                                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 65, debugstr.str(), __FILE__, __LINE__);
                                                }
                                                /*------ DEBUG LOG END ------*/
                                        }
                                }

                                virtualservice_element &srv = (*it)->get_element();

                                //get virtual service IP address infomation
                                if (srv.udpmode) {
                                        protocol = SNMP_IPPROTO_UDP;
                                        if (srv.udp_recv_endpoint.address().is_v4()) {
                                                strncpy(vs_ipaddress, srv.udp_recv_endpoint.address().to_v4().to_string().c_str(), L7VS_IPADDR_LEN);
                                        } else {
                                                strncpy(vs_ipaddress, srv.udp_recv_endpoint.address().to_v6().to_string().c_str(), L7VS_IPADDR_LEN);
                                                vs_ipaddress_type = SNMP_IPV6;
                                        }
                                        vs_port = srv.udp_recv_endpoint.port();
                                } else {
                                        protocol = SNMP_IPPROTO_TCP;
                                        if (srv.tcp_accept_endpoint.address().is_v4()) {
                                                strncpy(vs_ipaddress, srv.tcp_accept_endpoint.address().to_v4().to_string().c_str(), L7VS_IPADDR_LEN);
                                        } else {
                                                strncpy(vs_ipaddress, srv.tcp_accept_endpoint.address().to_v6().to_string().c_str(), L7VS_IPADDR_LEN);
                                                vs_ipaddress_type = SNMP_IPV6;
                                        }
                                        vs_port = srv.tcp_accept_endpoint.port();
                                }

                                //get sorry server IP address infomation
                                if (srv.sorry_endpoint.address().is_v4()) {
                                        strncpy(sorry_ipaddress, srv.sorry_endpoint.address().to_v4().to_string().c_str(), L7VS_IPADDR_LEN);
                                } else {
                                        strncpy(sorry_ipaddress, srv.sorry_endpoint.address().to_v6().to_string().c_str(), L7VS_IPADDR_LEN);
                                        sorry_ipaddress_type = SNMP_IPV6;
                                }
                                sorry_port = srv.sorry_endpoint.port();

                                sorry_flag = (srv.sorry_flag == 1) ? 1 : 2;

                                //get protocol mudule option string
                                std::stringstream protocol_module_arg;
                                BOOST_FOREACH(std::string str, srv.protocol_args) {
                                        protocol_module_arg << str << " ";
                                }

                                //set schedule module option string
                                std::string schedule_module_arg(" ");

                                //set ssl flag
                                int ssl_flag = srv.ssl_file_name == "" ? 0 : 1;

                                //set realserver size
                                int rs_size = srv.realserver_vector.size();

                                //get http statstics
                                protocol_module_base   *protocol_module_ptr = (*it)->get_protocol_module();
                                stats_base& base_stats = protocol_module_ptr->get_stats();
                                if ( base_stats.get_mode() == stats_base::MODE_HTTP ) {
                                        http_stats &httpstats = static_cast<http_stats&>(base_stats);
                                        http_requests.low = httpstats.http_requests.get();
                                        http_get_requests.low = httpstats.http_get_requests.get();
                                        http_post_requests.low = httpstats.http_post_requests.get();
                                }

                                //set sorry max connection
                                sorry_maxconnection.low = srv.sorry_maxconnection;

                                //set throughput infomation
                                throughput_upstream.low = srv.throughput_upstream;
                                throughput_downstream.low = srv.throughput_downstream;
                                qos_upstream.low = srv.qos_upstream * 8;
                                qos_downstream.low = srv.qos_downstream * 8;

                                //set session infomation
                                pool_session_count = (*it)->get_pool_sessions_count();
                                active_session_count = (*it)->get_active_sessions_count();
                                waiting_session_count = (*it)->get_waiting_sessions_count();

                                //set vs table row infomation
                                netsnmp_set_row_column(row, COLUMN_L7VSVSINDEX, ASN_INTEGER, (char *)&vs_index, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSPROTOCOL, ASN_INTEGER, (char *)&protocol, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSADDRTYPE, ASN_INTEGER, (char *)&vs_ipaddress_type, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSADDR, ASN_OCTET_STR, vs_ipaddress, strnlen(vs_ipaddress, L7VS_IPADDR_LEN));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSPORT, ASN_UNSIGNED, (char *)&vs_port, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSPROTOCOLMODULENAME, ASN_OCTET_STR, (char *)srv.protocol_module_name.c_str(), srv.protocol_module_name.length());
                                netsnmp_set_row_column(row, COLUMN_L7VSVSPROTOCOLMODULEOPTIONS, ASN_OCTET_STR, (char *)protocol_module_arg.str().c_str(), protocol_module_arg.str().length());
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSCHEDULEMODULENAME, ASN_OCTET_STR, (char *)srv.schedule_module_name.c_str(), srv.schedule_module_name.length());
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSCHEDULEMODULEOPTIONS, ASN_OCTET_STR, (char *)schedule_module_arg.c_str(), schedule_module_arg.length());
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSORRYADDRTYPE, ASN_INTEGER, (char *)&sorry_ipaddress_type,sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSORRYADDR, ASN_OCTET_STR, (char *)sorry_ipaddress, strnlen(sorry_ipaddress, L7VS_IPADDR_LEN));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSORRYPORT, ASN_UNSIGNED, (char *)&sorry_port, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSORRYCONNLIMIT, ASN_COUNTER64, (char *)&sorry_maxconnection, sizeof(struct counter64));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSORRYFORCEENABLED, ASN_INTEGER, (char *)&sorry_flag, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSTHROUGHPUTUP, ASN_COUNTER64, (char *)&throughput_upstream, sizeof(struct counter64));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSTHROUGHPUTDOWN, ASN_COUNTER64, (char *)&throughput_downstream, sizeof(struct counter64));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSTHROUGHPUTUPQOS, ASN_COUNTER64, (char *)&qos_upstream, sizeof(struct counter64));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSTHROUGHPUTDOWNQOS, ASN_COUNTER64, (char *)&qos_downstream, sizeof(struct counter64));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSSLFLAG, ASN_INTEGER, (char *)&ssl_flag, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSESSIONPOOLCOUNT, ASN_INTEGER, (char *)&pool_session_count, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSESSIONACTIVECOUNT, ASN_INTEGER, (char *)&active_session_count, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSSESSIONWAITCOUNT, ASN_INTEGER, (char *)&waiting_session_count, sizeof(int));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSHTTPTOTALCOUNT, ASN_COUNTER64, (char *)&http_requests, sizeof(struct counter64));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSHTTPGETCOUNT, ASN_COUNTER64, (char *)&http_get_requests, sizeof(struct counter64));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSHTTPPOSTCOUNT, ASN_COUNTER64, (char *)&http_post_requests, sizeof(struct counter64));
                                netsnmp_set_row_column(row, COLUMN_L7VSVSRSNUMBER, ASN_INTEGER, (char *)&rs_size, sizeof(int));

                                //create temporary realserver data list
                                int rs_index = 0;
                                for (std::vector<realserver_element>::iterator rs_it = srv.realserver_vector.begin();
                                                rs_it != srv.realserver_vector.end(); rs_it++) {
                                        //increment realserver index
                                        rs_index++;

                                        rsdata data;

                                        //set realserver infomation
                                        if (srv.udpmode) {
                                                if ((*rs_it).udp_endpoint.address().is_v4()) {
                                                        strncpy(data.address, (*rs_it).udp_endpoint.address().to_v4().to_string().c_str(), L7VS_IPADDR_LEN);
                                                        data.address_type = SNMP_IPV4;
                                                } else {
                                                        strncpy(data.address,(*rs_it).udp_endpoint.address().to_v6().to_string().c_str(), L7VS_IPADDR_LEN);
                                                        data.address_type = SNMP_IPV6;
                                                }
                                                data.port  = (*rs_it).udp_endpoint.port();
                                        } else {
                                                if ((*rs_it).tcp_endpoint.address().is_v4()) {
                                                        strncpy(data.address, (*rs_it).tcp_endpoint.address().to_v4().to_string().c_str(), L7VS_IPADDR_LEN);
                                                        data.address_type = SNMP_IPV4;
                                                } else {
                                                        strncpy(data.address, (*rs_it).tcp_endpoint.address().to_v6().to_string().c_str(), L7VS_IPADDR_LEN);
                                                        data.address_type = SNMP_IPV6;
                                                }
                                                data.port  = (*rs_it).tcp_endpoint.port();
                                        }

                                        data.index = rs_index;
                                        data.vs_index = vs_index;
                                        data.forward = 1;
                                        data.weigth = (*rs_it).weight;
                                        data.active_conn = (*rs_it).get_active();
                                        data.inactive_conn = (*rs_it).get_inact();

                                        //push into realserver data list
                                        rs_vector.push_back(data);
                                }
                        }
                }

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream    debugstr;
                        debugstr << "function : mibdata::update_vs_table : vs_table_size=";
                        debugstr << vs_table_size;

                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 66, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/

                //update realserver infomation with temporary realserver data list
                update_rs_table(rs_vector, err);
        }

        /*!
         * update real server table infomation.
         *
         * @param   const std::vector<rsdata> & temporary real server list
         * @param   error_code & error code
         * @retrun  void
         */
        void mibdata::update_rs_table(const std::vector<rsdata>&  rs_table_tmp, error_code&  err)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 67, "mibdata::update_rs_table", __FILE__, __LINE__);

                oid   index[1] = {0};
                int   rs_index = 0;
                netsnmp_table_row *row = NULL;

                //delete the surplus data from real server table
                if (rs_table_tmp.size() < static_cast<size_t>(rs_table_size)) {
                        for (size_t i = rs_table_size; i > rs_table_tmp.size(); i--) {
                                index[0] = i;
                                row = netsnmp_table_data_get_from_oid(rs_table->table, index, 1);
                                if (row == NULL) continue;
                                netsnmp_table_dataset_remove_and_delete_row(rs_table, row);
                        }
                }

                rs_table_size = rs_table_tmp.size();

                //update real server table
                for (std::vector<rsdata>::const_iterator  itr = rs_table_tmp.begin(); itr != rs_table_tmp.end(); itr++) {
                        index[0] = rs_index;
                        row = netsnmp_table_data_get_from_oid(rs_table->table, index, 1);
                        if (row == NULL) {
                                row = netsnmp_create_table_data_row();
                                if (row == NULL) {
                                        std::string msg("create realserver table row failed.");
                                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 14, msg, __FILE__, __LINE__);
                                        //set error code
                                        err.setter(true, msg);
                                        return ;
                                } else {
                                        netsnmp_table_row_add_index(row, ASN_INTEGER, (void*)index, sizeof(int));
                                        netsnmp_table_dataset_add_row(rs_table, row);
                                        /*-------- DEBUG LOG --------*/
                                        if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                                                std::stringstream    debugstr;
                                                debugstr << "function : mibdata::update_rs_table : rs_table add row.";
                                                boost::format("rs_index=%d") % (*itr).index;

                                                Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 68, debugstr.str(), __FILE__, __LINE__);
                                        }
                                        /*------ DEBUG LOG END ------*/
                                }
                        }

                        netsnmp_set_row_column(row, COLUMN_L7VSRSINDEX, ASN_INTEGER, (char *)&(*itr).index, sizeof(int));
                        netsnmp_set_row_column(row, COLUMN_L7VSRSVSINDEX, ASN_INTEGER, (char *)&(*itr).vs_index, sizeof(int));
                        netsnmp_set_row_column(row, COLUMN_L7VSRSADDRTYPE, ASN_INTEGER, (char *)&(*itr).address_type, sizeof(int));
                        netsnmp_set_row_column(row, COLUMN_L7VSRSADDR, ASN_OCTET_STR, (*itr).address, strlen((*itr).address));
                        netsnmp_set_row_column(row, COLUMN_L7VSRSPORT, ASN_UNSIGNED, (char *)&(*itr).port, sizeof(int));
                        netsnmp_set_row_column(row, COLUMN_L7VSRSFORWARD, ASN_INTEGER, (char *)&(*itr).forward, sizeof(int));
                        netsnmp_set_row_column(row, COLUMN_L7VSRSWEIGHT, ASN_INTEGER, (char *)&(*itr).weigth, sizeof(int));
                        netsnmp_set_row_column(row, COLUMN_L7VSRSACTIVECONN, ASN_INTEGER, (char *)&(*itr).active_conn, sizeof(int));
                        netsnmp_set_row_column(row, COLUMN_L7VSRSINACTIVECONN, ASN_INTEGER, (char *)&(*itr).inactive_conn, sizeof(int));

                        rs_index++;
                }

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream    debugstr;
                        debugstr << "function : mibdata::update_rs_table : rs_table_size=";
                        debugstr << rs_table_size;

                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 69, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/
        }

        /*!
         * update replication state.
         *
         * @param   l7vsd * l7vsd pointer
         * @param   error_code & error code
         * @retrun  void
         */
        void mibdata::update_replication_state(l7vsd *in_vsd , error_code&  err)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 70, "mibdata::update_replication_state", __FILE__, __LINE__);

                if (in_vsd == NULL) {
                        std::string msg("in_vsd pointer is NULL.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 15, msg, __FILE__, __LINE__);
                        //set error code
                        err.setter(true, msg);
                        return;
                }

                /*-------- DEBUG LOG --------*/
                if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SNMPAGENT)) {
                        std::stringstream    debugstr;
                        debugstr << "mibdata::update_replication_state arguments:";
                        debugstr << boost::format("in_vsd=%p") % in_vsd;
                        Logger::putLogDebug(LOG_CAT_L7VSD_SNMPAGENT, 71, debugstr.str(), __FILE__, __LINE__);
                }
                /*------ DEBUG LOG END ------*/

                replication::REPLICATION_MODE_TAG state = in_vsd->get_replication_state();
                replication_state = static_cast<int>(state);
        }

        /*!
         * get mibdata instance.
         *
         * @retrun  mibdata instance
         */
        mibdata& mibdata::get_instance()
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 72, "mibdata::get_instance", __FILE__, __LINE__);
                static mibdata data;
                return data;
        }

        /*!
         * collect mibdata in l7vsd.
         *
         * @param   l7vsd * l7vsd pointer
         * @param   error_code & error code
         * @retrun  void
         */
        void mibdata::collect_mibdata(l7vsd *in_vsd, error_code&  err)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 73, "mibdata::collect_mibdata", __FILE__, __LINE__);

                if (in_vsd == NULL) {
                        std::string msg("in_vsd pointer is NULL.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 16, msg, __FILE__, __LINE__);
                        //set error code
                        err.setter(true, msg);
                        return;
                }

                update_replication_state(in_vsd, err);

                if (err) {
                        std::string msg("update replication state failed.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 17, msg, __FILE__, __LINE__);
                        //set error code
                        err.setter(true, msg);
                        return;
                }

                update_vs_table(in_vsd, err);

                if (err) {
                        std::string msg("update virtual service table failed.");
                        Logger::putLogError(LOG_CAT_L7VSD_SNMPAGENT, 18, msg, __FILE__, __LINE__);
                        //set error code
                        err.setter(true, msg);
                        return;
                }
        }

        /*!
         * set virtual service data set.
         *
         * @param   netsnmp_table_data_set * virtual service data set
         * @retrun  void
         */
        void mibdata::set_vs_table(netsnmp_table_data_set*  in_vs_table)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 74, "mibdata::set_vs_table", __FILE__, __LINE__);
                vs_table = in_vs_table;
        }

        /*!
         * set real server data set.
         *
         * @param   netsnmp_table_data_set * real server data set
         * @retrun  void
         */
        void mibdata::set_rs_table(netsnmp_table_data_set*  in_rs_table)
        {
                Logger    logger(LOG_CAT_L7VSD_SNMPAGENT, 75, "mibdata::set_rs_table", __FILE__, __LINE__);
                rs_table = in_rs_table;
        }
}
