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

RET=`$L7VSADM -X 2>&1 | grep "PARSE ERROR: command not found."`
EXPECT="PARSE ERROR: command not found."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -X"
        exit 1
fi

exit 0

