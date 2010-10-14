#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add a virtual service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
	exit 1
fi

#Set SNMPAgent function enable and 0 into cache update interval
$L7VSADM -S -f 1 -i 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1 -i 0"
	exit 1
fi

#l7vsVsTable enable check
RET=`snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsTable`

EXPECT="SNMP table: ULTRAMONKEY-L7-MIB::l7vsVsTable

 l7vsVsProtocol l7vsVsAddrType  l7vsVsAddr l7vsVsPort l7vsVsProtocolModuleName l7vsVsProtocolModuleOptions l7vsVsScheduleModuleName l7vsVsScheduleModuleOptions l7vsVsSorryAddrType l7vsVsSorryAddr l7vsVsSorryPort l7vsVsSorryConnLimit l7vsVsSorryForceEnabled l7vsVsThroughputUp l7vsVsThroughputDown l7vsVsThroughputUpQoS l7vsVsThroughputDownQos l7vsVsSslFlag l7vsVsSessionPoolCount l7vsVsSessionActiveCount l7vsVsSessionWaitCount l7vsVsHttpTotalCount l7vsVsHttpGetCount l7vsVsHttpPostCount
            tcp           IPv4 \"127.0.0.1\"      40001              sessionless                                                   rr                                            IPv4       \"0.0.0.0\"               0        0 Connections                     off              0 B/s                0 B/s                 0 B/s                   0 B/s             0                     31                        0                      1                    0                  0                   0"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsTable"
        exit 1
fi

#l7vsRsTable empty check
RET=
RET=`snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable`

EXPECT="ULTRAMONKEY-L7-MIB::l7vsRsTable: No entries"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable"
        exit 1
fi

#l7vsVsNumber check
RET=
RET=`snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsNumber.0`

EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsNumber.0 = INTEGER: 1"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "snmpget -v 2c -c public localhost ULTRAMONKEY-L7-MIB::l7vsVsNumber.0"
        exit 1
fi

exit 0
