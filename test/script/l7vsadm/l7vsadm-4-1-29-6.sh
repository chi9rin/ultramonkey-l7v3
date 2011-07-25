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

RET=`$L7VSADM -A -t localhost:40001 -m ip -q 100m 2>&1`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -q 100m"
        exit 1
fi

if [ -n "$RET" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -q 100m"
        exit 1
fi

exit 0

