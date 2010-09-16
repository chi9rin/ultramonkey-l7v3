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

RET=`$L7VSADM -A -t localhost:40001 -m ip -z /dev/floppy 2>&1 | grep "PARSE ERROR: ssl config file cannot open.(--ssl)"`
EXPECT="PARSE ERROR: ssl config file cannot open.(--ssl)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -z /dev/floppy"
        exit 1
fi

exit 0

