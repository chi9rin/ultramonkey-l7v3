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

RET=`$L7VSADM -A -t 127.0.0.1:44431 -m sslid --timeout 100 -T 1000 2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR: protocol module argument error(--proto-module): Cannot set multiple option '-T/--timeout'."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m sslid --timeout 100 -T 1000"
        exit 1
fi

exit 0

