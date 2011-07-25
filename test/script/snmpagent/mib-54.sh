#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#Run http server
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001

SorryServer_ADDR=127.0.0.1
SorryServer_PORT=50002

#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add Service
$L7VSADM -A -t [::1]:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT} 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t [::1]:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT}"
        exit 1
fi



$L7VSADM -a -t [::1]:40001 -m sessionless -r 127.0.0.1:50001 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001
              -m sessionless -r 127.0.0.1:50001"
        exit 1
fi

$L7VSADM -S -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1"
        exit 1
fi

#IPv6 address type check
RET=`snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsAddrType.1`
echo "RET = $RET"
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsAddrType.1 = INTEGER: IPv6(2)"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpget -v 1 -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsAddrType.1"
        exit 1
fi


#IPv6 address check
RET=`snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsAddr.1`
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsAddr.1 = STRING: \"::1\""

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpget -v 1 -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsAddr.1"
        exit 1
fi



exit 0
