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

EXPECT="PARSE ERROR: sorryserver endpoint parse error(--bypass): invalid endpoint:Host not found.*:333.333.333.333:3333"
$L7VSADM -A -t 127.0.0.1:40001 -m ip -b 333.333.333.333:3333 2>&1 | grep "${EXPECT}" > /dev/null
if [ "$?" -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -b 333.333.333.333:3333"
        exit 1
fi

exit 0

