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

RET=`$L7VSADM -A -t localhost:40001 -m ip -Q 1000G 2>&1 | grep "PARSE ERROR : qos_upstream value is too big."`
EXPECT="PARSE ERROR : qos_upstream value is too big."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -Q 1000G"
        exit 1
fi

exit 0

