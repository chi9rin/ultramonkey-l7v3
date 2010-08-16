#!/bin/bash

# set config file
cp materials/logger-133-1-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

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

RET=`$L7VSADM 2>&1 | grep "\[ERROR\] l7vsadm_logger "`
if [ -z "$RET" ]
then
       	echo "Test failed: $L7VSADM"
       	exit 1
fi

RET=`$L7VSADM`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless rr"
if [ "$RET" != "$EXPECT" ]
then
       	echo "Test failed: $L7VSADM"
       	exit 1
fi

exit 0
