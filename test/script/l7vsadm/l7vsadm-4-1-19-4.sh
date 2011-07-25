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

RET=`$L7VSADM -A -t 127.0.0.1:40001 -m ip -b localhost: 2>&1 | grep "PARSE ERROR: sorryserver endpoint parse error(--bypass): invalid port number (0):localhost:"`
EXPECT="PARSE ERROR: sorryserver endpoint parse error(--bypass): invalid port number (0):localhost:"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -b localhost:"
        exit 1
fi

exit 0

