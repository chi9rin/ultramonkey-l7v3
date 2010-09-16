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

RET=`$L7VSADM -R -x 2>&1 | grep "PARSE ERROR: replication option not found(--replication): -x"`
EXPECT="PARSE ERROR: replication option not found(--replication): -x"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -R -x"
        exit 1
fi

exit 0

