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

RET=`$L7VSADM -V 2>&1 | grep "l7vsd_system"`
EXPECT="l7vsd_system                   warn
l7vsd_system_memory            warn
l7vsd_system_endpoint          warn
l7vsd_system_signal            warn
l7vsd_system_environment       warn"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd_system"
        exit 1
fi

$L7VSADM --log --category l7vsd_system_memory --level error
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM --log --category l7vsd_system_memory --level error"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | grep "l7vsd_system"`
EXPECT="l7vsd_system                   warn
l7vsd_system_memory            error
l7vsd_system_endpoint          warn
l7vsd_system_signal            warn
l7vsd_system_environment       warn"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd_system"
        exit 1
fi

exit 0

