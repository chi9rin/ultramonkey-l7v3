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

$L7VSADM -A -t 127.0.0.1:40001 -m ip -s rr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -s rr"
        exit 1
fi


RET=`$L7VSADM -E -t 127.0.0.1:40001 -m ip -u 100001 2>&1 | grep "PARSE ERROR: invalid sorry max connection value.(--upper)"`
EXPECT="PARSE ERROR: invalid sorry max connection value.(--upper)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -E -t 127.0.0.1:40001 -m ip -u 100001"
        exit 1
fi

exit 0

