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

RET=`$L7VSADM -R -f -d 2>&1 | grep "PARSE ERROR : replication option is double specified."`
EXPECT="PARSE ERROR : replication option is double specified."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -R -f -d"
        exit 1
fi

exit 0

