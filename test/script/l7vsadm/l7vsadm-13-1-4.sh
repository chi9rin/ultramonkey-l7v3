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

#set logtrap level though logtrap is disable
$L7VSADM -S -f 1 -t 0 -l debug 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1 -t 0 -l debug"
        exit 1
fi

#enable logtrap so logtrap level's value could be watched
$L7VSADM -S -f 1 -t 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1 -t 1"
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
EXPECT="log trap level                 debug"


if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

exit 0


