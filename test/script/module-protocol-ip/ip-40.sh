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

RET=`$L7VSADM -A -t 127.0.0.1:44431 -m ip -R -N 2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR: protocol module argument error(--proto-module): You have to choose either of reschedule or no-reschedule."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m ip -R -N"
        exit 1
fi

exit 0

