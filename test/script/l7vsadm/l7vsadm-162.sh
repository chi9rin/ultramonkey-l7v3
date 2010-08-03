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

RET=`$L7VSADM -A -t 127.0.0.1:40001 -m ip --xxx 2>&1 | grep "PARSE ERROR : protocol module argument error: Option error."` 
EXPECT="PARSE ERROR : protocol module argument error: Option error."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip --xxx"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log | grep "\[ERROR\] l7vsadm_protocol PRM40600010 Option error."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log | grep "\[ERROR\] l7vsadm_parse ADM42800018 protocol module argument error: Option error."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0

