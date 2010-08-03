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

RET=`$L7VSADM -D -t localhost:40001 -m xx 2>&1 | grep "PARSE ERROR : protocol module not found:xx"`
EXPECT="PARSE ERROR : protocol module not found:xx"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -D -t localhost:40001 -m xx"
        exit 1
fi

exit 0

