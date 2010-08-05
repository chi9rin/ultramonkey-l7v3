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

touch /usr/lib64/l7vs/protomod_x.so
RET=`$L7VSADM -D -t localhost:40001 -m x 2>&1 | grep "PARSE ERROR : protocol module not found:x"`
EXPECT="PARSE ERROR : protocol module not found:x"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -D -t localhost:40001 -m x"
        exit 1
fi

exit 0

