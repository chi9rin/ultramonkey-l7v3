//
//!	@file	store_mibdata.h
//!	@brief	net-snmp mibdata stored header
//
//	copyright(c) sdy corporation.2008
//	mail: h.okada at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef STORE_MIBDATA_H
#define STORE_MIBDATA_H

#include <string>
#include <map>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "message.h"
#include "netsnmp_bridge.h"
#include "logger_enum.h"
#include "MessengerClient.h"

#define DISPLAYSTRING_LEN           (256)     // RFC2579
#define	L7VS_IPADDR_LEN				(128)

struct vsdata {
    unsigned short      index;
    unsigned char       protocol;
    char                ipAddress[L7VS_IPADDR_LEN];
    unsigned short      portNumber;
    char                scheduleModule[L7VS_MODNAME_LEN];
    char                protocolModule[L7VS_MODNAME_LEN];
    char                protocolModuleOption[L7VS_PROTOMOD_OPT_LEN];
    unsigned char       reschedule;
    char                sorryIpAddress[L7VS_IPADDR_LEN];
    unsigned short      sorryPortNumber;
    unsigned int        sorryThreshold;
    unsigned char       sorryForceFlag;
    unsigned long long  QoSThresholdUp;
    unsigned long long  QoSThresholdDown;
    unsigned long long  throughputUp;
    unsigned long long  throughputDown;
};

struct rsdata {
    unsigned short  index;
    unsigned short  virtualServiceIndex;
    char            ipAddress[L7VS_IPADDR_LEN];
    unsigned short  portNumber;
    unsigned short  forwardMode;
    unsigned int    weight;
    unsigned int    activeConn;
    unsigned int    inactiveConn;
};

struct l7vsd_log_level {
    l7vs::LOG_LEVEL_TAG   network;
    l7vs::LOG_LEVEL_TAG   networkBandwidth;
    l7vs::LOG_LEVEL_TAG   networkNumConnection;
    l7vs::LOG_LEVEL_TAG   networkQoS;
    l7vs::LOG_LEVEL_TAG   virtualService;
    l7vs::LOG_LEVEL_TAG   realServer;
    l7vs::LOG_LEVEL_TAG   realServerBalancing;
    l7vs::LOG_LEVEL_TAG   sorryServer;
    l7vs::LOG_LEVEL_TAG   replication;
    l7vs::LOG_LEVEL_TAG   startStop;
    l7vs::LOG_LEVEL_TAG   system;
    l7vs::LOG_LEVEL_TAG   systemMemory;
    l7vs::LOG_LEVEL_TAG   systemSocket;
    l7vs::LOG_LEVEL_TAG   systemSignal;
    l7vs::LOG_LEVEL_TAG   environment;
    l7vs::LOG_LEVEL_TAG   environmentParameter;
    l7vs::LOG_LEVEL_TAG   logger;
    l7vs::LOG_LEVEL_TAG   parameter;
    l7vs::LOG_LEVEL_TAG   event;
    l7vs::LOG_LEVEL_TAG   schedule;
    l7vs::LOG_LEVEL_TAG   program;
    l7vs::LOG_LEVEL_TAG   protocol;
    l7vs::LOG_LEVEL_TAG   module;
};

struct l7vsadm_log_level {
    l7vs::LOG_LEVEL_TAG   parse;
    l7vs::LOG_LEVEL_TAG   operate;
    l7vs::LOG_LEVEL_TAG   communicate;
    l7vs::LOG_LEVEL_TAG   configResult;
    l7vs::LOG_LEVEL_TAG   common;
    l7vs::LOG_LEVEL_TAG   logger;
    l7vs::LOG_LEVEL_TAG   parameter;
    l7vs::LOG_LEVEL_TAG   protocol;
    l7vs::LOG_LEVEL_TAG   module;
};

struct l7snmpagent_log_level {
    l7vs::LOG_LEVEL_TAG   startStop;
    l7vs::LOG_LEVEL_TAG   managerReceive;
    l7vs::LOG_LEVEL_TAG   managerSend;
    l7vs::LOG_LEVEL_TAG   l7vsdReceive;
    l7vs::LOG_LEVEL_TAG   l7vsdSend;
    l7vs::LOG_LEVEL_TAG   logger;
    l7vs::LOG_LEVEL_TAG   parameter;
};

struct l7_status {
    char                    message[DISPLAYSTRING_LEN];
    IPC_STATUS_TAG          snmpAgent;
//    REPLICATION_MODE_TAG    replication;
};

class l7ag_store_mibdata
{
protected:
    std::vector<vsdata>     vsdatalist;
    std::vector<rsdata>     rsdatalist;
    l7vsd_log_level         vsd_log;
    l7vsadm_log_level       adm_log;
    l7snmpagent_log_level   snmp_log;
    l7_status               status;
    netsnmp_table_data_set* vs_table_set;
    netsnmp_table_data_set* rs_table_set;
private:
    l7ag_store_mibdata(){};
    l7ag_store_mibdata( const l7ag_store_mibdata& ){};
    l7ag_store_mibdata& operator=( const l7ag_store_mibdata& in_data ){
        vsdatalist = in_data.vsdatalist;
        rsdatalist = in_data.rsdatalist;
        return *this;
    };
public:
    static l7ag_store_mibdata& getInstance(){
        static l7ag_store_mibdata instance;
        return instance;
    }

    vsdata* getVS_first_data_point();
    rsdata* getRS_first_data_point();
    vsdata* getVS_data_point( std::vector<vsdata>::iterator );
    rsdata* getRS_data_point( std::vector<rsdata>::iterator );
    std::vector<vsdata>::iterator   getVS_first_data_iterator();
    std::vector<rsdata>::iterator   getRS_first_data_iterator();

    vsdata*                 getVSmibdata( std::size_t );
    rsdata*                 getRSmibdata( std::size_t );
    l7vsd_log_level*        getVsdLogmibdata();
    l7vsadm_log_level*      getVsadmLogmibdata();
    l7snmpagent_log_level*  getAgentLogmibdata();
    l7_status*              getStatmibdata();

    std::size_t getVSdatacount();
    std::size_t getRSdatacount();

    bool        addVSmibdata( struct l7ag_mibdata_payload_vs* );
    bool        addRSmibdata( struct l7ag_mibdata_payload_rs* );
    vsdata*     updateVSmibdata( std::size_t, l7ag_mibdata_payload_vs* );
    rsdata*     updateRSmibdata( std::size_t, l7ag_mibdata_payload_rs* );

    void    clearVSmibdata();
    void    clearRSmibdata();
    void    clearVsdLogmibdata();
    void    clearVsadmLogmibdata();
    void    clearAgentLogmibdata();
    void    clearStatmibdata();
    void    setVsdLogmibdata(l7vsd_log_level);
    void    setVsadmLogmibdata(l7vsadm_log_level);
    void    setAgentLogmibdata(l7snmpagent_log_level);
    void    setStatmibdata(l7_status);
    void    setVsTableSet(netsnmp_table_data_set*);
    void    setRsTableSet(netsnmp_table_data_set*);
};

#endif //__STORE_MIBDATA_H__
