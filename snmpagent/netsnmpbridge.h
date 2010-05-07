/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 15999 2007-03-25 22:32:02Z dts12 $
 */
#ifndef NETSNMPBRIDGE_H
#define NETSNMPBRIDGE_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "MessengerClient.h"

/*
 * function declarations
 */
void init_UltraMonkeyL7(MessengerClient *);
Netsnmp_Node_Handler    handle_VSCount;
Netsnmp_Node_Handler    handle_RSCount;

void initialize_table_VSTable(void);
Netsnmp_Node_Handler        VSTable_handler;
Netsnmp_First_Data_Point    VSTable_get_first_data_point;
Netsnmp_Next_Data_Point     VSTable_get_next_data_point;

void initialize_table_RSTable(void);
Netsnmp_Node_Handler        RSTable_handler;
Netsnmp_First_Data_Point    RSTable_get_first_data_point;
Netsnmp_Next_Data_Point     RSTable_get_next_data_point;

// trap functions
void    set_l7vsError_trap_value(const std::string &);
int     send_l7vsError_trap(void);

void    collect_mib(unsigned int, void *);

/*
 * column number definitions for table VSTable
 */
#define COLUMN_INDEXVS              1
#define COLUMN_QOSTHREASHOLDUP      2
#define COLUMN_QOSTHREASHOLDDOWN    3
#define COLUMN_RSCOUNTVS            4

/*
 * column number definitions for table RSTable
 */
#define COLUMN_INDEXRS              1
#define COLUMN_VSINDEX              2
#define COLUMN_WEIGHT               3
#define COLUMN_ACTIVECONN           4
#define COLUMN_INACTIVECONN         5

#endif                          /* NETSNMPBRIDGE_H */
