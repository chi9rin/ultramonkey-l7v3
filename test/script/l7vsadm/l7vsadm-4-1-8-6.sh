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

RET=`$L7VSADM -A -t 127.0.0.1:40001 -m ip -q 10M -q 1M 2>&1 | grep "PARSE ERROR : Option -q is conflict."`
EXPECT="PARSE ERROR : Option -q is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -q 10M -q 1M"
        exit 1
fi

exit 0

