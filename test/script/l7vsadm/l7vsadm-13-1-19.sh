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

$L7VSADM -S -t 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -t 1"
        exit 1
fi

$L7VSADM -S -f 1 -l error -i 10 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1 -l error -i 10"
        exit 1
fi

#"-f" "-l" and "-i" are used at the same time 
$L7VSADM -S -f 1 -l error -i 10 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1 -l error -i 10"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | grep "Agent Status"`
EXPECT="Agent Status                   active"

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

RET=`$L7VSADM -V 2>&1 | grep "cache update interval"`
EXPECT="cache update interval          10"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0

