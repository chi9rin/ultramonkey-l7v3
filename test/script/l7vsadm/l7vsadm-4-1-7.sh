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

RET=`$L7VSADM -A -t 127.0.0.1:40001 -m ip -q 1M --timeout 100 2>&1 | grep "PARSE ERROR : virtualservice option not found:--timeout"`
EXPECT="PARSE ERROR : virtualservice option not found:--timeout"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -q 1M --timeout 100"
        exit 1
fi

exit 0

