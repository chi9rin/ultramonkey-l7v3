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

touch /usr/lib64/l7vs/sched_x.so
RET=`$L7VSADM -A -t 127.0.0.1:40001 -m ip -s x 2>&1 | grep "PARSE ERROR : scheduler module not found:x"`
EXPECT="PARSE ERROR : scheduler module not found:x"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -s x"
        exit 1
fi

exit 0

