#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

SorryServer=SorryServer
SorryServer_ADDR=127.0.0.1
SorryServer_PORT=50002
start_lighttpd -s $SorryServer -a $SorryServer_ADDR -p $SorryServer_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer"
        exit 1
fi

#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add Service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT} 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT}"
        exit 1
fi



$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001
              -m sessionless -r 127.0.0.1:50001 -w 100"
        exit 1
fi

$L7VSADM -S -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

#Check Return Value
RET=`snmpwalk -v 1 -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::ultramonkey-l7`
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsNumber.0 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsProtocol.1 = INTEGER: tcp(1)
ULTRAMONKEY-L7-MIB::l7vsVsAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsVsAddr.1 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsVsPort.1 = Gauge32: 40001
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleName.1 = STRING: sessionless
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleOptions.1 = STRING: 
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleName.1 = STRING: rr
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleOptions.1 = STRING:  
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddr.1 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsVsSorryPort.1 = Gauge32: 50002
ULTRAMONKEY-L7-MIB::l7vsVsSorryForceEnabled.1 = INTEGER: off(2)
ULTRAMONKEY-L7-MIB::l7vsVsSslFlag.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsVsSessionPoolCount.1 = INTEGER: 31
ULTRAMONKEY-L7-MIB::l7vsVsSessionActiveCount.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsVsSessionWaitCount.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsRsNumber.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsVsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsRsAddr.1 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsRsPort.1 = Gauge32: 50001
ULTRAMONKEY-L7-MIB::l7vsRsForward.1 = INTEGER: masq(1)
ULTRAMONKEY-L7-MIB::l7vsRsWeight.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsActiveConn.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsRsInActiveConn.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0 = INTEGER: single(1)"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpwalk -v 1 -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::ultramonkey-l7"
        exit 1
fi

exit 0
