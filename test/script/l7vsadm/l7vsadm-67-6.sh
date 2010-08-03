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

RET=`$L7VSADM -A -t localhost:40001 -m ip -O quickackon,quickackoff 2>&1 | grep "PARSE ERROR : socket option quickack is duplicated."`
EXPECT="PARSE ERROR : socket option quickack is duplicated."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -O quickackon,quickackoff"
        exit 1
fi

exit 0

