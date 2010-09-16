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

RET=`$L7VSADM -L -c l7vsd_virtualservice -c l7vsd_virtualservice 2>&1 | grep "PARSE ERROR: Option -c conflict."`
EXPECT="PARSE ERROR: Option -c conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -L -c l7vsd_virtualservice -c l7vsd_virtualservice"
        exit 1
fi

exit 0

