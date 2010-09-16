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

RET=`$L7VSADM -A -t localhost:40001 -m ip -q G 2>&1 | grep "PARSE ERROR: invalid QoS downstream value.(--qos-down)"`
EXPECT="PARSE ERROR: invalid QoS downstream value.(--qos-down)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -q G"
        exit 1
fi

exit 0

