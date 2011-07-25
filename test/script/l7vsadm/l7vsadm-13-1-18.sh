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


#"-f" "-t" and "-i" are used at the same time 
$L7VSADM -S -f 1 -t 1 -l error
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1 -t 1 -l error"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | grep "Agent Status"`
EXPECT="Agent Status                   active"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi


RET=`$L7VSADM -V 2>&1 | grep "log trap status"`
EXPECT="log trap status                on"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | grep "log trap level"`
EXPECT="log trap level                 error"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi




exit 0

