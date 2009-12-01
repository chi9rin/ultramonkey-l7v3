//
//!	@file	store_mibdata.cpp
//!	@brief	net-snmp mibdata stored source
//
//	copyright(c) sdy corporation.2008
//	mail: h.okada at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "store_mibdata.h"

vsdata*	l7ag_store_mibdata::getVS_first_data_point(){
    return &vsdatalist.front();
}

std::vector<vsdata>::iterator	l7ag_store_mibdata::getVS_first_data_iterator(){
    return vsdatalist.begin();
}

vsdata*	l7ag_store_mibdata::getVS_data_point(std::vector<vsdata>::iterator in_it){
    if (vsdatalist.end() == in_it) {
        return NULL;
    }
    for (unsigned int i = 0; i < vsdatalist.size(); i++) {
        if (vsdatalist[i].index == in_it->index) {
            return &vsdatalist[i];
        }
    }
    return NULL;
}

std::size_t	l7ag_store_mibdata::getVSdatacount(){
    return vsdatalist.size();
}

vsdata*	l7ag_store_mibdata::getVSmibdata(std::size_t n){
    if (vsdatalist.size() <= n) {
        return NULL;
    }
    return &vsdatalist[n];
}

vsdata*	l7ag_store_mibdata::updateVSmibdata(std::size_t n, l7ag_mibdata_payload_vs* in_data){
    oid   index[1];
    netsnmp_table_row *row;

    if (vsdatalist.size() <= n) {
        return NULL;
    }
    if (in_data == NULL) {
        for (size_t i = vsdatalist.size(); i > n; i--) {
            index[0] = i;
            row = netsnmp_table_data_get_from_oid(vs_table_set->table, index, 1);
            netsnmp_table_dataset_remove_and_delete_row(vs_table_set, row);
            vsdatalist.pop_back();
        }
        return NULL;
    }

    vsdatalist[n].index = in_data->index;
    switch (in_data->protocol) {
    case IPPROTO_TCP:
        vsdatalist[n].protocol = 1;
        break;
    case IPPROTO_UDP:
        vsdatalist[n].protocol = 2;
        break;
    default:
        vsdatalist[n].protocol = 0;
        break;
    }
    struct in_addr vip_in = { in_data->ipAddress };
    char* vip = inet_ntoa(vip_in);
    strncpy(vsdatalist[n].ipAddress, vip, L7VS_IPADDR_LEN);
    vsdatalist[n].portNumber    = ntohs(in_data->portNumber);
    strncpy(vsdatalist[n].scheduleModule, in_data->scheduleModule, L7VS_MODNAME_LEN);
    strncpy(vsdatalist[n].protocolModule, in_data->protocolModule, L7VS_MODNAME_LEN);
    strncpy(vsdatalist[n].protocolModuleOption, in_data->protocolModuleOption, L7VS_PROTOMOD_OPT_LEN);
    switch (in_data->reschedule) {
    case 0:
        vsdatalist[n].reschedule = 2;
        break;
    case 1:
        vsdatalist[n].reschedule = 1;
        break;
    default:
        vsdatalist[n].reschedule = 0;
        break;
    }
    struct in_addr sorry_in = { in_data->sorryIpAddress };
    char* sorry = inet_ntoa(sorry_in);
    strncpy(vsdatalist[n].sorryIpAddress, sorry, L7VS_IPADDR_LEN);
    vsdatalist[n].sorryPortNumber   = ntohs(in_data->sorryPortNumber);
    vsdatalist[n].sorryThreshold    = in_data->sorryThreshold;
    switch (in_data->sorryForceFlag) {
    case 0:
        vsdatalist[n].sorryForceFlag = 2;
        break;
    case 1:
        vsdatalist[n].sorryForceFlag = 1;
        break;
    default:
        vsdatalist[n].sorryForceFlag = 0;
        break;
    }
    vsdatalist[n].QoSThresholdUp    = in_data->QoSThresholdUp   * 8;
    vsdatalist[n].QoSThresholdDown  = in_data->QoSThresholdDown * 8;
    vsdatalist[n].throughputUp      = in_data->throughputUp     * 8;
    vsdatalist[n].throughputDown    = in_data->throughputDown   * 8;

    if (vs_table_set == NULL) {
        return false;
    }
    index[0] = vsdatalist[n].index;
    row = netsnmp_table_data_get_from_oid(vs_table_set->table, index, 1);
    if( row == NULL ) {
//        row = netsnmp_table_data_set_create_row_from_defaults(vs_table_set->default_row);
        row = netsnmp_create_table_data_row();
        if( row == NULL ) return false;
        netsnmp_table_row_add_index(row, ASN_INTEGER, (char*)&vsdatalist[n].index, sizeof(unsigned short));
        netsnmp_table_dataset_add_row(vs_table_set, row);
    }

    netsnmp_set_row_column(row, COLUMN_UM7VSINDEX, ASN_INTEGER, (char*) &vsdatalist[n].index, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7VSPROTOCOL, ASN_INTEGER, (char*) &vsdatalist[n].protocol, sizeof(unsigned char));
    netsnmp_set_row_column(row, COLUMN_UM7VSIPADDRESS, ASN_OCTET_STR, vsdatalist[n].ipAddress, strnlen(vsdatalist[n].ipAddress, L7VS_IPADDR_LEN));
    netsnmp_set_row_column(row, COLUMN_UM7VSPORTNUMBER, ASN_UNSIGNED, (char*) &vsdatalist[n].portNumber, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7VSSCHEDULEMODULE, ASN_OCTET_STR, (char*) &vsdatalist[n].scheduleModule, L7VS_MODNAME_LEN);
    netsnmp_set_row_column(row, COLUMN_UM7VSPROTOCOLMODULE, ASN_OCTET_STR, (char*) &vsdatalist[n].protocolModule, L7VS_MODNAME_LEN);
    netsnmp_set_row_column(row, COLUMN_UM7VSPROTOCOLMODULEOPTION, ASN_OCTET_STR, (char*) &vsdatalist[n].protocolModuleOption, L7VS_PROTOMOD_OPT_LEN);
    netsnmp_set_row_column(row, COLUMN_UM7VSRESCHEDULE, ASN_INTEGER, (char*) &vsdatalist[n].reschedule, sizeof(unsigned char));
    netsnmp_set_row_column(row, COLUMN_UM7VSSORRYIPADDRESS, ASN_OCTET_STR, vsdatalist[n].sorryIpAddress, strnlen(vsdatalist[n].sorryIpAddress, L7VS_IPADDR_LEN));
    netsnmp_set_row_column(row, COLUMN_UM7VSSORRYPORTNUMBER, ASN_UNSIGNED, (char*) &vsdatalist[n].sorryPortNumber, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7VSSORRYTHRESHOLD, ASN_INTEGER, (char*) &vsdatalist[n].sorryThreshold, sizeof(unsigned int));
    netsnmp_set_row_column(row, COLUMN_UM7VSSORRYFORCEFLAG, ASN_INTEGER, (char*) &vsdatalist[n].sorryForceFlag, sizeof(unsigned char));
    netsnmp_set_row_column(row, COLUMN_UM7VSQOSTHRESHOLDUP, ASN_INTEGER, (char*) &vsdatalist[n].QoSThresholdUp, sizeof(unsigned long long));
    netsnmp_set_row_column(row, COLUMN_UM7VSQOSTHRESHOLDDOWN, ASN_INTEGER, (char*) &vsdatalist[n].QoSThresholdDown, sizeof(unsigned long long));
    netsnmp_set_row_column(row, COLUMN_UM7VSTHROUGHPUTUP, ASN_INTEGER, (char*) &vsdatalist[n].throughputUp, sizeof(unsigned long long));
    netsnmp_set_row_column(row, COLUMN_UM7VSTHROUGHPUTDOWN, ASN_INTEGER, (char*) &vsdatalist[n].throughputDown, sizeof(unsigned long long));

    return &vsdatalist[n];
}

bool	l7ag_store_mibdata::addVSmibdata(l7ag_mibdata_payload_vs* in_data){
    oid   index[1];
    netsnmp_table_row *row;
    vsdata    data;

    if (NULL == in_data) {
        return false;
    }

    data.index = in_data->index;
    switch (in_data->protocol) {
    case IPPROTO_TCP:
        data.protocol = 1;
        break;
    case IPPROTO_UDP:
        data.protocol = 2;
        break;
    default:
        data.protocol = 0;
        break;
    }
    struct in_addr vip_in = { in_data->ipAddress };
    char* vip = inet_ntoa(vip_in);
    strncpy(data.ipAddress, vip, L7VS_IPADDR_LEN);
    data.portNumber = ntohs(in_data->portNumber);
    strncpy(data.scheduleModule, in_data->scheduleModule, L7VS_MODNAME_LEN);
    strncpy(data.protocolModule, in_data->protocolModule, L7VS_MODNAME_LEN);
    strncpy(data.protocolModuleOption, in_data->protocolModuleOption, L7VS_PROTOMOD_OPT_LEN);
    switch (in_data->reschedule) {
    case 0:
        data.reschedule = 2;
        break;
    case 1:
        data.reschedule = 1;
        break;
    default:
        data.reschedule = 0;
        break;
    }
    struct in_addr sorry_in = { in_data->sorryIpAddress };
    char* sorry = inet_ntoa(sorry_in);
    strncpy(data.sorryIpAddress, sorry, L7VS_IPADDR_LEN);
    data.sorryPortNumber    = ntohs(in_data->sorryPortNumber);
    data.sorryThreshold     = in_data->sorryThreshold;
    switch (in_data->sorryForceFlag) {
    case 0:
        data.sorryForceFlag = 2;
        break;
    case 1:
        data.sorryForceFlag = 1;
        break;
    default:
        data.sorryForceFlag = 0;
        break;
    }
    data.QoSThresholdUp     = in_data->QoSThresholdUp   * 8;
    data.QoSThresholdDown   = in_data->QoSThresholdDown * 8;
    data.throughputUp       = in_data->throughputUp     * 8;
    data.throughputDown     = in_data->throughputDown   * 8;
    vsdatalist.push_back(data);

    if (vs_table_set == NULL) {
        return false;
    }
    index[0] = data.index;
    row = netsnmp_table_data_get_from_oid(vs_table_set->table, index, 1);
    if( row == NULL ) {
//        row = netsnmp_table_data_set_create_row_from_defaults(vs_table_set->default_row);
        row = netsnmp_create_table_data_row();
        if( row == NULL ) return false;
        netsnmp_table_row_add_index(row, ASN_INTEGER, (char*)&data.index, sizeof(unsigned short));
        netsnmp_table_dataset_add_row(vs_table_set, row);
    }

    netsnmp_set_row_column(row, COLUMN_UM7VSINDEX, ASN_INTEGER, (char*) &data.index, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7VSPROTOCOL, ASN_INTEGER, (char*) &data.protocol, sizeof(unsigned char));
    netsnmp_set_row_column(row, COLUMN_UM7VSIPADDRESS, ASN_OCTET_STR, data.ipAddress, strnlen(data.ipAddress, L7VS_IPADDR_LEN));
    netsnmp_set_row_column(row, COLUMN_UM7VSPORTNUMBER, ASN_UNSIGNED, (char*) &data.portNumber, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7VSSCHEDULEMODULE, ASN_OCTET_STR, (char*) &data.scheduleModule, L7VS_MODNAME_LEN);
    netsnmp_set_row_column(row, COLUMN_UM7VSPROTOCOLMODULE, ASN_OCTET_STR, (char*) &data.protocolModule, L7VS_MODNAME_LEN);
    netsnmp_set_row_column(row, COLUMN_UM7VSPROTOCOLMODULEOPTION, ASN_OCTET_STR, (char*) &data.protocolModuleOption, L7VS_PROTOMOD_OPT_LEN);
    netsnmp_set_row_column(row, COLUMN_UM7VSRESCHEDULE, ASN_INTEGER, (char*) &data.reschedule, sizeof(unsigned char));
    netsnmp_set_row_column(row, COLUMN_UM7VSSORRYIPADDRESS, ASN_OCTET_STR, data.sorryIpAddress, strnlen(data.sorryIpAddress, L7VS_IPADDR_LEN));
    netsnmp_set_row_column(row, COLUMN_UM7VSSORRYPORTNUMBER, ASN_UNSIGNED, (char*) &data.sorryPortNumber, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7VSSORRYTHRESHOLD, ASN_INTEGER, (char*) &data.sorryThreshold, sizeof(unsigned int));
    netsnmp_set_row_column(row, COLUMN_UM7VSSORRYFORCEFLAG, ASN_INTEGER, (char*) &data.sorryForceFlag, sizeof(unsigned char));
    netsnmp_set_row_column(row, COLUMN_UM7VSQOSTHRESHOLDUP, ASN_INTEGER, (char*) &data.QoSThresholdUp, sizeof(unsigned long long));
    netsnmp_set_row_column(row, COLUMN_UM7VSQOSTHRESHOLDDOWN, ASN_INTEGER, (char*) &data.QoSThresholdDown, sizeof(unsigned long long));
    netsnmp_set_row_column(row, COLUMN_UM7VSTHROUGHPUTUP, ASN_INTEGER, (char*) &data.throughputUp, sizeof(unsigned long long));
    netsnmp_set_row_column(row, COLUMN_UM7VSTHROUGHPUTDOWN, ASN_INTEGER, (char*) &data.throughputDown, sizeof(unsigned long long));

    return true;
}

void	l7ag_store_mibdata::clearVSmibdata(){
    oid   index[1];
    netsnmp_table_row *row;
    for (size_t i = 1; i <= vsdatalist.size(); i++) {
        index[0] = i;
        row = netsnmp_table_data_get_from_oid(vs_table_set->table, index, 1);
        netsnmp_table_dataset_remove_and_delete_row(vs_table_set, row);
    }
    vsdatalist.clear();
}

rsdata*	l7ag_store_mibdata::getRS_first_data_point(){
    return &rsdatalist.front();
}

rsdata*	l7ag_store_mibdata::getRS_data_point(std::vector<rsdata>::iterator in_it){
    if (rsdatalist.end() == in_it) {
        return NULL;
    }
    for (size_t i = 0; i < rsdatalist.size(); i++) {
        if (rsdatalist[i].index == in_it->index) {
            return &rsdatalist[i];
        }
    }

    return NULL;
}

std::vector<rsdata>::iterator	l7ag_store_mibdata::getRS_first_data_iterator(){
    return rsdatalist.begin();
}

std::size_t	l7ag_store_mibdata::getRSdatacount(){
    return rsdatalist.size();
}

rsdata*	l7ag_store_mibdata::getRSmibdata(std::size_t n){
    if (rsdatalist.size() <= n) {
        return NULL;
    }
    return &rsdatalist[n];
}

rsdata*	l7ag_store_mibdata::updateRSmibdata(std::size_t n, l7ag_mibdata_payload_rs* in_data){
    oid   index[1];
    netsnmp_table_row *row;

    if (rsdatalist.size() <= n) {
        return NULL;
    }
    if (in_data == NULL) {
        for (size_t i = rsdatalist.size(); i > n; i--) {
            index[0] = i;
            row = netsnmp_table_data_get_from_oid(rs_table_set->table, index, 1);
            netsnmp_table_dataset_remove_and_delete_row(rs_table_set, row);
            rsdatalist.pop_back();
        }
        return NULL;
    }

    rsdatalist[n].index                  = in_data->index;
    rsdatalist[n].virtualServiceIndex    = in_data->virtualServiceIndex;
    struct in_addr in = { in_data->ipAddress };
    char* ip = inet_ntoa(in);
    strncpy(rsdatalist[n].ipAddress, ip, L7VS_IPADDR_LEN);
    rsdatalist[n].portNumber             = ntohs(in_data->portNumber);
    rsdatalist[n].forwardMode            = in_data->forwardMode;
    rsdatalist[n].weight                 = in_data->weight;
    rsdatalist[n].activeConn             = in_data->activeConn;
    rsdatalist[n].inactiveConn           = in_data->inactiveConn;

    if (rs_table_set == NULL) {
        return false;
    }
    index[0] = rsdatalist[n].index;
    row = netsnmp_table_data_get_from_oid(rs_table_set->table, index, 1);
    if( row == NULL ) {
//        row = netsnmp_table_data_set_create_row_from_defaults(rs_table_set->default_row);
        row = netsnmp_create_table_data_row();
        if( row == NULL ) return false;
        netsnmp_table_row_add_index(row, ASN_INTEGER, (char*)&rsdatalist[n].index, sizeof(unsigned short));
        netsnmp_table_dataset_add_row(rs_table_set, row);
    }

    netsnmp_set_row_column(row, COLUMN_UM7RSINDEX, ASN_INTEGER, (char*) &rsdatalist[n].index, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7RSVIRTUALSERVICEINDEX, ASN_INTEGER, (char*) &rsdatalist[n].virtualServiceIndex, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7RSIPADDRESS, ASN_OCTET_STR, rsdatalist[n].ipAddress, strnlen(rsdatalist[n].ipAddress, L7VS_IPADDR_LEN));
    netsnmp_set_row_column(row, COLUMN_UM7RSPORTNUMBER, ASN_UNSIGNED, (char*) &rsdatalist[n].portNumber, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7RSFORWARDMODE, ASN_INTEGER, (char*) &rsdatalist[n].forwardMode, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7RSWEIGHT, ASN_INTEGER, (char*) &rsdatalist[n].weight, sizeof(unsigned int));
    netsnmp_set_row_column(row, COLUMN_UM7RSACTIVECONN, ASN_INTEGER, (char*) &rsdatalist[n].activeConn, sizeof(unsigned int));
    netsnmp_set_row_column(row, COLUMN_UM7RSINACTIVECONN, ASN_INTEGER, (char*) &rsdatalist[n].inactiveConn, sizeof(unsigned int));

    return &rsdatalist[n];
}

bool	l7ag_store_mibdata::addRSmibdata(l7ag_mibdata_payload_rs* in_data){
    oid   index[1];
    netsnmp_table_row *row;
    rsdata data;

    if (NULL == in_data) {
        return false;
    }

    data.index                  = in_data->index;
    data.virtualServiceIndex    = in_data->virtualServiceIndex;
    struct in_addr in = { in_data->ipAddress };
    char* ip = inet_ntoa(in);
    strncpy(data.ipAddress, ip, L7VS_IPADDR_LEN);
    data.portNumber             = ntohs(in_data->portNumber);
    data.forwardMode            = in_data->forwardMode;
    data.weight                 = in_data->weight;
    data.activeConn             = in_data->activeConn;
    data.inactiveConn           = in_data->inactiveConn;

    rsdatalist.push_back(data);

    if (rs_table_set == NULL) {
        return false;
    }
    index[0] = data.index;
    row = netsnmp_table_data_get_from_oid(rs_table_set->table, index, 1);
    if( row == NULL ) {
//        row = netsnmp_table_data_set_create_row_from_defaults(rs_table_set->default_row);
        row = netsnmp_create_table_data_row();
        if( row == NULL ) return false;
        netsnmp_table_row_add_index(row, ASN_INTEGER, (char*)&data.index, sizeof(unsigned short));
        netsnmp_table_dataset_add_row(rs_table_set, row);
    }

    netsnmp_set_row_column(row, COLUMN_UM7RSINDEX, ASN_INTEGER, (char*) &data.index, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7RSVIRTUALSERVICEINDEX, ASN_INTEGER, (char*) &data.virtualServiceIndex, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7RSIPADDRESS, ASN_OCTET_STR, data.ipAddress, strnlen(data.ipAddress, L7VS_IPADDR_LEN));
    netsnmp_set_row_column(row, COLUMN_UM7RSPORTNUMBER, ASN_UNSIGNED, (char*) &data.portNumber, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7RSFORWARDMODE, ASN_INTEGER, (char*) &data.forwardMode, sizeof(unsigned short));
    netsnmp_set_row_column(row, COLUMN_UM7RSWEIGHT, ASN_INTEGER, (char*) &data.weight, sizeof(unsigned int));
    netsnmp_set_row_column(row, COLUMN_UM7RSACTIVECONN, ASN_INTEGER, (char*) &data.activeConn, sizeof(unsigned int));
    netsnmp_set_row_column(row, COLUMN_UM7RSINACTIVECONN, ASN_INTEGER, (char*) &data.inactiveConn, sizeof(unsigned int));

    return true;
}

void	l7ag_store_mibdata::clearRSmibdata(){
    oid   index[1];
    netsnmp_table_row *row;
    for (size_t i = 1; i <= rsdatalist.size(); i++) {
        index[0] = i;
        row = netsnmp_table_data_get_from_oid(rs_table_set->table, index, 1);
        netsnmp_table_dataset_remove_and_delete_row(rs_table_set, row);
    }
    rsdatalist.clear();
}

l7vsd_log_level*	l7ag_store_mibdata::getVsdLogmibdata(){
    return &vsd_log;
}

l7vsadm_log_level*	l7ag_store_mibdata::getVsadmLogmibdata(){
    return &adm_log;
}

l7snmpagent_log_level*	l7ag_store_mibdata::getAgentLogmibdata(){
    return &snmp_log;
}

l7_status*	l7ag_store_mibdata::getStatmibdata(){
    return &status;
}

void	l7ag_store_mibdata::clearVsdLogmibdata(){
    vsd_log.network                 = l7vs::LOG_LV_NONE;
    vsd_log.networkBandwidth        = l7vs::LOG_LV_NONE;
    vsd_log.networkNumConnection    = l7vs::LOG_LV_NONE;
    vsd_log.networkQoS              = l7vs::LOG_LV_NONE;
    vsd_log.virtualService          = l7vs::LOG_LV_NONE;
    vsd_log.realServer              = l7vs::LOG_LV_NONE;
    vsd_log.realServerBalancing     = l7vs::LOG_LV_NONE;
    vsd_log.sorryServer             = l7vs::LOG_LV_NONE;
    vsd_log.replication             = l7vs::LOG_LV_NONE;
    vsd_log.startStop               = l7vs::LOG_LV_NONE;
    vsd_log.system                  = l7vs::LOG_LV_NONE;
    vsd_log.systemMemory            = l7vs::LOG_LV_NONE;
    vsd_log.systemSocket            = l7vs::LOG_LV_NONE;
    vsd_log.systemSignal            = l7vs::LOG_LV_NONE;
    vsd_log.environment             = l7vs::LOG_LV_NONE;
    vsd_log.environmentParameter    = l7vs::LOG_LV_NONE;
    vsd_log.logger                  = l7vs::LOG_LV_NONE;
    vsd_log.parameter               = l7vs::LOG_LV_NONE;
    vsd_log.event                   = l7vs::LOG_LV_NONE;
    vsd_log.schedule                = l7vs::LOG_LV_NONE;
    vsd_log.program                 = l7vs::LOG_LV_NONE;
    vsd_log.protocol                = l7vs::LOG_LV_NONE;
    vsd_log.module                  = l7vs::LOG_LV_NONE;
}

void	l7ag_store_mibdata::clearVsadmLogmibdata(){
    adm_log.parse           = l7vs::LOG_LV_NONE;
    adm_log.operate         = l7vs::LOG_LV_NONE;
    adm_log.communicate     = l7vs::LOG_LV_NONE;
    adm_log.configResult    = l7vs::LOG_LV_NONE;
    adm_log.common          = l7vs::LOG_LV_NONE;
    adm_log.logger          = l7vs::LOG_LV_NONE;
    adm_log.parameter       = l7vs::LOG_LV_NONE;
    adm_log.protocol        = l7vs::LOG_LV_NONE;
    adm_log.module          = l7vs::LOG_LV_NONE;
}

void	l7ag_store_mibdata::clearAgentLogmibdata(){
    snmp_log.startStop      = l7vs::LOG_LV_NONE;
    snmp_log.managerReceive = l7vs::LOG_LV_NONE;
    snmp_log.managerSend    = l7vs::LOG_LV_NONE;
    snmp_log.l7vsdReceive   = l7vs::LOG_LV_NONE;
    snmp_log.l7vsdSend      = l7vs::LOG_LV_NONE;
    snmp_log.logger         = l7vs::LOG_LV_NONE;
    snmp_log.parameter      = l7vs::LOG_LV_NONE;
}

void	l7ag_store_mibdata::clearStatmibdata(){
    memset(&status.message, 0, DISPLAYSTRING_LEN);
    status.snmpAgent    = UNKNOWN; 
//	status.replication  = l7vs::replication::REPLICATION_OUT;
}

void	l7ag_store_mibdata::setVsdLogmibdata(l7vsd_log_level log){
    vsd_log.network                 = log.network;
    vsd_log.networkBandwidth        = log.networkBandwidth;
    vsd_log.networkNumConnection    = log.networkNumConnection;
    vsd_log.networkQoS              = log.networkQoS;
    vsd_log.virtualService          = log.virtualService;
    vsd_log.realServer              = log.realServer;
    vsd_log.realServerBalancing     = log.realServerBalancing;
    vsd_log.sorryServer             = log.sorryServer;
    vsd_log.replication             = log.replication;
    vsd_log.startStop               = log.startStop;
    vsd_log.system                  = log.system;
    vsd_log.systemMemory            = log.systemMemory;
    vsd_log.systemSocket            = log.systemSocket;
    vsd_log.systemSignal            = log.systemSignal;
    vsd_log.environment             = log.environment;
    vsd_log.environmentParameter    = log.environmentParameter;
    vsd_log.logger                  = log.logger;
    vsd_log.parameter               = log.parameter;
    vsd_log.event                   = log.event;
    vsd_log.schedule                = log.schedule;
    vsd_log.program                 = log.program;
    vsd_log.protocol                = log.protocol;
    vsd_log.module                  = log.module;
}

void	l7ag_store_mibdata::setVsadmLogmibdata(l7vsadm_log_level log){
    adm_log.parse           = log.parse;
    adm_log.operate         = log.operate;
    adm_log.communicate     = log.communicate;
    adm_log.configResult    = log.configResult;
    adm_log.common          = log.common;
    adm_log.logger          = log.logger;
    adm_log.parameter       = log.parameter;
    adm_log.protocol        = log.protocol;
    adm_log.module          = log.module;
}

void	l7ag_store_mibdata::setAgentLogmibdata(l7snmpagent_log_level log){
    snmp_log.startStop      = log.startStop;
    snmp_log.managerReceive = log.managerReceive;
    snmp_log.managerSend    = log.managerSend;
    snmp_log.l7vsdReceive   = log.l7vsdReceive;
    snmp_log.l7vsdSend      = log.l7vsdSend;
    snmp_log.logger         = log.logger;
    snmp_log.parameter      = log.parameter;
}

void	l7ag_store_mibdata::setStatmibdata(l7_status stat){
    memcpy(&status.message, &stat.message, strnlen(stat.message, DISPLAYSTRING_LEN));
    status.snmpAgent    = stat.snmpAgent; 
// 	status.replication  = stat.replication;
}

void	l7ag_store_mibdata::setVsTableSet(netsnmp_table_data_set* table_set){
    vs_table_set = table_set;
}

void	l7ag_store_mibdata::setRsTableSet(netsnmp_table_data_set* table_set){
    rs_table_set = table_set;
}
