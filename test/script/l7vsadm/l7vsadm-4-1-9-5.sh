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

RET=`$L7VSADM -A -t localhost:999999 -m ip 2>&1 | grep "PARSE ERROR: target endpoint parse error(--tcp-service): invalid port number:localhost:999999"`
EXPECT="PARSE ERROR: target endpoint parse error(--tcp-service): invalid port number:localhost:999999"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:999999 -m ip"
        exit 1
fi

exit 0

