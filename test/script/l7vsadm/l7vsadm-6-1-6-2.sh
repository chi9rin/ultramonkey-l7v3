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

RET=`$L7VSADM -R -s x 2>&1 | grep "PARSE ERROR: replication switch option not found(--switch): x"`
EXPECT="PARSE ERROR: replication switch option not found(--switch): x"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -R -s x"
        exit 1
fi

exit 0

