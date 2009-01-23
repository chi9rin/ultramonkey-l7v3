//
//!	@file	massage.h
//!	@brief	snmpagent - l7vsd message header
//
//	copyright(c) sdy corporation.2008
//	mail: h.okada at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MESSAGE_H
#define MESSAGE_H

#define PADDINGSIZE             (1)

#define OIDDATASIZE             (128)	// must 64 multiple number for structure byte offset
#define COMMANDDATASIZE         (256)
#define TRAPREQUESTMESSAGESIZE  (64)	// must 64 multiple number for structure byte offset
#define MIBREQUESTDATASIZE      (64)	// must 64 multiple number for structure byte offset
#define	L7VS_MODNAME_LEN		(256)	// l7vs.h remove this numeric. define this,
#define	L7VS_PROTOMOD_OPT_LEN	(512)	// l7vs.h remove this numeric, define this,

//message id definitions(l7vsd to subagent)
#define MESSAGE_ID_COMMANDREQUEST           (0)
#define MESSAGE_ID_TRAPREQUEST              (1)
#define MESSAGE_ID_MIBCOLLECTRESPONSE_VS    (2)
#define MESSAGE_ID_MIBCOLLECTRESPONSE_RS    (3)

//message id definitions(subagent to l7vsd)
#define MESSAGE_ID_MIBCOLLECTREQUEST        (100)

//setting command definitions
#define COMMAND_LOGLEVEL_CHANGE             (0x1)
#define COMMAND_SETTINGFILE_RELOAD          (0x2)

#define MIBDATA_CATEGORY_VS                 (1)
#define MIBDATA_CATEGORY_RS                 (2)

#define MAGIC_NUMBER_LENGTH                 (2)
#define MAGIC_MESSAGE_HEADER                "MG" // 0x4D 0x47
#define MAGIC_PAYLOAD_HEADER                "PY" // 0x50 0x59
#define MAGIC_SETTING_COMMAND               "ST" // 0x53 0x54
#define MAGIC_CHANGE_LOGLEVEL               "LL" // 0x4c 0x4c
#define MAGIC_TRAP_REQUEST                  "TR" // 0x54 0x52
#define MAGIC_VIRTUAL_SERVICE               "VS" // 0x56 0x53
#define MAGIC_REAL_SERVER                   "RS" // 0x52 0x53
#define MAGIC_MIB_REQUEST                   "RQ" // 0x52 0x51


struct  l7ag_message_header
{
    char                magic[MAGIC_NUMBER_LENGTH]; //! Magic number
    unsigned long long  version;                    //! Version
    unsigned long long  time;                       //! Created time
    unsigned long long  size;                       //! Message size
    unsigned long long  payload_count;              //! Payload count
};

/*!
 * 
 */
struct  l7ag_payload_header
{
    char                magic[MAGIC_NUMBER_LENGTH]; //! Magic number
    unsigned long long  message_id;
    unsigned long long  payload_datasize;
};

/*
 * log level setting
 */
struct  l7ag_settingcommand_message
{
    char                magic[MAGIC_NUMBER_LENGTH]; //! Magic number
    unsigned long long  command_id;                 //! 0 when change log level, 1 when reload config
    unsigned char       data[COMMANDDATASIZE];      //! command parameter (set category when change log level)
};

/*!
 * command parameter of change log level
 */
struct  l7ag_changeloglevel_parameter
{
    char                magic[MAGIC_NUMBER_LENGTH]; //! Magic number
    unsigned long long  log_category;
    unsigned long long  log_level;
};

/*!
 * Request message of Trap
 */
struct  l7ag_traprequest_message
{
    char    magic[MAGIC_NUMBER_LENGTH];         //! Magic number
    char    oid[OIDDATASIZE];                   //! OID (must "1.3.6.1.4.1.60000.1.0.2")
    char    message[TRAPREQUESTMESSAGESIZE];    //! Errno
};

/*!
 * Message for collect MIB data (VirtualService)
 */
struct  l7ag_mibdata_payload_vs
{
    char                magic[MAGIC_NUMBER_LENGTH];                     //! Magic number
    int                 index;                                          //! VirtualService index
    uint8_t             protocol;                                       //! Protocol
    uint32_t            ipAddress;                                      //! IP
    uint16_t            portNumber;                                     //! Port
    char                scheduleModule[L7VS_MODNAME_LEN];               //! schedule module
    char                protocolModule[L7VS_MODNAME_LEN];               //! protocol module
    char                protocolModuleOption[L7VS_PROTOMOD_OPT_LEN];    //! protomod option
    int                 reschedule;                                     //! reschedule flag
    uint32_t            sorryIpAddress;                                 //! sorry IP
    uint16_t            sorryPortNumber;                                //! sorry port
    int                 sorryThreshold;                                 //! sorry threshold
    int                 sorryForceFlag;                                 //! sorry flag
    unsigned long long  QoSThresholdUp;                                 //! QoS Threashold value for up stream
    unsigned long long  QoSThresholdDown;                               //! QoS Threashold value for down stream
    unsigned long long  throughputUp;                                   //! Throughput for up stream
    unsigned long long  throughputDown;                                 //! Throughput for down stream
    int                 vs_count;                                       //! VirtualService count

    bool    operator==( const l7ag_mibdata_payload_vs& in_data ) {
        if (index == in_data.index) {
            return true;
        }
        else {
            return false;
        }
    }
    l7ag_mibdata_payload_vs& operator=( const l7ag_mibdata_payload_vs& in_data ){
        index               = in_data.index;
        protocol            = in_data.protocol;
        ipAddress           = in_data.ipAddress;
        strncpy(scheduleModule, in_data.scheduleModule, L7VS_MODNAME_LEN);
        strncpy(protocolModule, in_data.protocolModule, L7VS_MODNAME_LEN);
        strncpy(protocolModuleOption, in_data.protocolModuleOption, L7VS_PROTOMOD_OPT_LEN);
        reschedule          = in_data.reschedule;
        sorryIpAddress      = in_data.sorryIpAddress;
        sorryPortNumber     = in_data.sorryPortNumber;
        sorryThreshold      = in_data.sorryThreshold;
        sorryForceFlag      = in_data.sorryForceFlag;
        QoSThresholdUp      = in_data.QoSThresholdUp;
        QoSThresholdDown    = in_data.QoSThresholdDown;
        throughputUp        = in_data.throughputUp;
        throughputDown      = in_data.throughputDown;
        vs_count            = in_data.vs_count;

        return *this;
    }
};


/*!
 * Message for collect MIB data (RealServer)
 */
struct  l7ag_mibdata_payload_rs
{
    char                magic[MAGIC_NUMBER_LENGTH]; //! Magic number
    int                 index;                      //! RealServer index
    int                 virtualServiceIndex;        //! Relation VirtualService index
    uint32_t            ipAddress;                  //! IP
    uint16_t            portNumber;                 //! Port
    int                 forwardMode;                //! forward mode
    unsigned long long  weight;                     //! weight
    unsigned long long  activeConn;                 //! active connection
    unsigned long long  inactiveConn;               //! inactive connection
    int                 rs_count;

    bool    operator==( const l7ag_mibdata_payload_rs& in_data ) {
        if (index == in_data.index) {
            return true;
        }
        else {
            return false;
        }
    }
    l7ag_mibdata_payload_rs& operator=( const l7ag_mibdata_payload_rs& in_data ){
        index               = in_data.index;
        virtualServiceIndex = in_data.virtualServiceIndex;
        ipAddress           = in_data.ipAddress;
        portNumber          = in_data.portNumber;
        forwardMode         = in_data.forwardMode;
        weight              = in_data.weight;
        activeConn          = in_data.activeConn;
        inactiveConn        = in_data.inactiveConn;
        rs_count            = in_data.rs_count;

        return *this;
    }
};

/*!
 * Request message of collect MIB data
 */
struct  l7ag_mibrequest_message
{
    char            magic[MAGIC_NUMBER_LENGTH]; //! Magic number
    unsigned char   oid[OIDDATASIZE];           //! OID. Get all data when set oid to 0
    unsigned char   data[MIBREQUESTDATASIZE];   //! Expansion
};

#endif	//MESSAGE_H
