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

RET=`$L7VSADM -A -t localhost:40001 -m ip -L 2>&1 | grep "PARSE ERROR: access log flag value is not specified.(--access-log)"`
EXPECT="PARSE ERROR: access log flag value is not specified.(--access-log)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -L"
        exit 1
fi

exit 0

