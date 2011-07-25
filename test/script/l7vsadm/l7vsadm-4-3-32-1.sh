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

$L7VSADM -A -t 127.0.0.1:40001 -m ip 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip"
        exit 1
fi

RET=`$L7VSADM -E -t 127.0.0.1:40001 -m ip -L 1 2>&1`
EXPECT="edit vs error : access log flag change err."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -E -t 127.0.0.1:40001 -m ip -L 1"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log | grep "\[ERROR\] l7vsadm_common ADM43100007 edit vs error : access log flag change err."`
if [ -z "$RET"  ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0
