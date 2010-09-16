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

RET=`$L7VSADM -L -c l7vsd_x -l error 2>&1 | grep "PARSE ERROR: log category not found(--category): l7vsd_x"`
EXPECT="PARSE ERROR: log category not found(--category): l7vsd_x"
echo "$RET"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -L -c l7vsd_x -l error"
        exit 1
fi

exit 0

