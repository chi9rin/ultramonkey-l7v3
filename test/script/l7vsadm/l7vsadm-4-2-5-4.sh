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

RET=`$L7VSADM -D -t localhost: -m ip 2>&1 | grep "PARSE ERROR: target endpoint parse error(--tcp-service): invalid port number (0):localhost:"`
EXPECT="PARSE ERROR: target endpoint parse error(--tcp-service): invalid port number (0):localhost:"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -D -t localhost: -m ip"
        exit 1
fi

exit 0

