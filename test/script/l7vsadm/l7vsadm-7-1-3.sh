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

RET=`$L7VSADM -V 2>&1 | grep "l7vsd_snmpagent"`
EXPECT="l7vsd_snmpagent                info"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd_agent"
        exit 1
fi

$L7VSADM --log --category l7vsd_snmpagent --level error
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM --log --category l7vsd_snmpagent --level error"
        exit 1
fi

RET=`$L7VSADM -V 2>&1 | grep "l7vsd_snmpagent"`
EXPECT="l7vsd_snmpagent                error"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V | grep l7vsd_snmpagent"
        exit 1
fi

exit 0

