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

RET=`$L7VSADM -A -t localhost:40001 -m ip -q K 2>&1 | grep "PARSE ERROR : invalid qos_downstream value."`
EXPECT="PARSE ERROR : invalid qos_downstream value."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -q K"
        exit 1
fi

exit 0

