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

$L7VSADM -S -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -i 0"
        exit 1
fi



#set cache update interval
$L7VSADM -S -i 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -i 0"
        exit 1
fi

#Check Return Value
RET=`snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsNumber.0 `
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsNumber.0 = INTEGER: 0"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsNumber.0"
        exit 1
fi

#set cache update interval
$L7VSADM -S -i 5
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -i 30"
        exit 1
fi
#Add Service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless  
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

usleep 6000000


RET=`snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsNumber.0`
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsNumber.0 = INTEGER: 1"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsNumber.0"
        exit 1
fi



exit 0
