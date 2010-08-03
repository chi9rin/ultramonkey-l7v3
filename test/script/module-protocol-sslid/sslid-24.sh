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

RET=`$L7VSADM -A -t 127.0.0.1:44431 -m sslid --maxlist 2147483648 2>&1 | grep "PARSE ERROR"`
EXPECT="PARSE ERROR : protocol module argument error: '-M/--maxlist' option value '2147483648' is too large."

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m sslid --maxlist 2147483648"
        exit 1
fi

exit 0

