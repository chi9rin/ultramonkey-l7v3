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

EXPECT="PARSE ERROR: realserver endpoint parse error(--real-server): invalid endpoint:Host not found.*:333.333.333.333:3333"
$L7VSADM -a -t 127.0.0.1:40001 -m ip -r 333.333.333.333:3333 2>&1 | grep "${EXPECT}" > /dev/null
if [ "$?" -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r 333.333.333.333:3333"
        exit 1
fi

exit 0

