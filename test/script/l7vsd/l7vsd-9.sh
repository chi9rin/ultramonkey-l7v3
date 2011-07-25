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

RET=`ps aux | grep $L7VSD | grep -v grep | awk '{print $11}'`
EXPECT="$L7VSD"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ps x | grep $L7VSD | grep -v grep | awk '{print $5}'"
        exit 1
fi

exit 0

