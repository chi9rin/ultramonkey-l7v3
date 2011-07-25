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

#disable snmpagent
$L7VSADM -S -f 0 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 0"
        exit 1
fi


RET=`$L7VSADM -V 2>&1 | grep "Agent Status"`
EXPECT="Agent Status                   inactive"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0


