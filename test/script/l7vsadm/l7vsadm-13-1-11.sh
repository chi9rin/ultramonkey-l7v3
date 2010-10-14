#!/bin/bash

. ${SET_DEFAULT_CONF}

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#enable snmpagent 
$L7VSADM -S -f 1 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1"
        exit 1
fi

#enable log trap 
$L7VSADM -S -t 1 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -t 1"
        exit 1
fi


#set log trap level
$L7VSADM -S -l debug
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -l debug"
        exit 1
fi


RET=`$L7VSADM -V 2>&1 | grep "log trap level"`
EXPECT="log trap level                 debug"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0


