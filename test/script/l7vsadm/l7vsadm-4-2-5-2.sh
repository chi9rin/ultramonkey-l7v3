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

EXPECT="PARSE ERROR: target endpoint parse error(--tcp-service): invalid endpoint:Host not found.*:333.333.333.333:3333"
$L7VSADM -D -t 333.333.333.333:3333 -m ip 2>&1 | grep "${EXPECT}" > /dev/null
if [ "$?" -ne 0 ]
then
        echo "Test failed: $L7VSADM -D -t 333.333.333.333:3333 -m ip"
        exit 1
fi

exit 0

