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

$L7VSADM -A -t localhost:40001 -m ip
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip"
        exit 1
fi

RET=`$L7VSADM -E -t localhost:40001 -m ip -q 1000G 2>&1 | grep "PARSE ERROR : qos_downstream value is too big."`
EXPECT="PARSE ERROR : qos_downstream value is too big."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -E -t localhost:40001 -m ip -q 1000G"
        exit 1
fi

exit 0

