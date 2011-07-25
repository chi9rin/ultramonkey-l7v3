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

RET=`$L7VSADM -V | grep "l7vsd_virtualservice"`
EXPECT="l7vsd_virtualservice           warn
l7vsd_virtualservice_thread    warn"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd_virtualservice"
        exit 1
fi

$L7VSADM -L -c vs -l error
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -L -c vs -l error"
        exit 1
fi

RET=`$L7VSADM -V | grep "l7vsd_virtualservice"`
EXPECT="l7vsd_virtualservice           error
l7vsd_virtualservice_thread    warn"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd_virtualservice"
        exit 1
fi

exit 0

