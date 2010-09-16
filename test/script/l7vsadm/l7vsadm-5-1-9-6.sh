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

$L7VSADM -A -t 127.0.0.1:40001 -m ip
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip"
        exit 1
fi

RET=`$L7VSADM -a -t 127.0.0.1:40001 -m ip -r 0.0.0.0:3333 2>&1 | grep "PARSE ERROR: realserver endpoint parse error(--real-server): invalid address (INADDR_ANY):0.0.0.0:3333"`
EXPECT="PARSE ERROR: realserver endpoint parse error(--real-server): invalid address (INADDR_ANY):0.0.0.0:3333"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r 0.0.0.0:3333"
        exit 1
fi

exit 0

