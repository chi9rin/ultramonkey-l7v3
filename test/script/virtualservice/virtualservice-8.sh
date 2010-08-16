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

RET=`$L7VSADM -D -t 127.0.0.2:40002 -m sessionless 2>&1`
EXPECT="del vs error : virtual service not found."

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -D -t 127.0.0.2:40002 -m sessionless"
        exit 1
fi

exit 0

