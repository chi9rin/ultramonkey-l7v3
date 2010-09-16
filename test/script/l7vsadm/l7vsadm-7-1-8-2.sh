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

RET=`$L7VSADM -L -c l7vsd_virtualservice -l x 2>&1 | grep "PARSE ERROR: log level not found(--level): x"`
EXPECT="PARSE ERROR: log level not found(--level): x"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -L -c l7vsd_virtualservice -l x"
        exit 1
fi

exit 0

