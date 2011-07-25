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

RET=`$L7VSADM -A -t localhost:40001 -m ip -Q M 2>&1 | grep "PARSE ERROR: invalid QoS upstream value.(--qos-up)"`
EXPECT="PARSE ERROR: invalid QoS upstream value.(--qos-up)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -Q M"
        exit 1
fi

exit 0

