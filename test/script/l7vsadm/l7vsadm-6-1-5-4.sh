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

RET=`$L7VSADM -R -f -f 2>&1 | grep "PARSE ERROR: replication option is double specified.(--force)"`
EXPECT="PARSE ERROR: replication option is double specified.(--force)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -R -f -f"
        exit 1
fi

exit 0

