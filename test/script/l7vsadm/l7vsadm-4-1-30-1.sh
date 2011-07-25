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

RET=`$L7VSADM -A -t localhost:40001 -m ip -q 2>&1 | grep "PARSE ERROR: QoS downstream value is not specified.(--qos-down)"`
EXPECT="PARSE ERROR: QoS downstream value is not specified.(--qos-down)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -q"
        exit 1
fi

exit 0

