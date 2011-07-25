#!/bin/bash

# set config file
cp materials/parameter-37-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

RET=`$L7VSADM`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

EXPECT="Layer-7 Virtual Server version 3.0.1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log | grep "\[WARN\] l7vsadm_common ADM33100002 Get cmd_interval parameter error. Use default value."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0

