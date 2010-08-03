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

RET=`$L7VSADM -D -t localhost:40001 -m rrrrrrrrrrrrrrrrrrrr 2>&1 | grep "PARSE ERROR : protomod name is too long."`
EXPECT="PARSE ERROR : protomod name is too long."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -D -t localhost:40001 -m rrrrrrrrrrrrrrrrrrrr"
        exit 1
fi

exit 0

