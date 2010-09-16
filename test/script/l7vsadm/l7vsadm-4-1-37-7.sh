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

RET=`$L7VSADM -A -t localhost:40001 -m ip -O deferaccept,deferaccept 2>&1 | grep "PARSE ERROR: socket option deferaccept is duplicated.(--sockopt)"`
EXPECT="PARSE ERROR: socket option deferaccept is duplicated.(--sockopt)"
echo "$RET"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -O deferaccept,deferaccept"
        exit 1
fi

exit 0

