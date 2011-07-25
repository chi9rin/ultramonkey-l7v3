#!/bin/bash

# set config file
cp materials/logger-139-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

RET=`$L7VSADM`
EXPECT="Layer-7 Virtual Server version 3.0.1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless rr"
if [ "$RET" != "$EXPECT" ]
then
       	echo "Test failed: $L7VSADM"
       	exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log | grep "\[WARN\] l7vsadm_common ADM33100003 Illegal cmd_count parameter value. Use default value."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0
