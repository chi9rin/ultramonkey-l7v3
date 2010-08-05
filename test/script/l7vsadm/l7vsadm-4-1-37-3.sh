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

RET=`$L7VSADM -A -t localhost:40001 -m ip -O deferacceptnodelay 2>&1 | grep "PARSE ERROR : unknown socket option."`
EXPECT="PARSE ERROR : unknown socket option."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -O deferacceptnodelay"
        exit 1
fi

exit 0

