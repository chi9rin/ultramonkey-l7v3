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

$L7VSADM -A -t [::1]:40001 -m sessionless -s rr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t [::1]:40001 -m sessionless -s rr"
        exit 1
fi

$L7VSADM -A -t [::1]:40002 -m sessionless -s rr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t [::1]:40002 -m sessionless -s rr"
        exit 1
fi

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP [::1]:40001 sessionless rr
TCP [::1]:40002 sessionless rr"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

$L7VSADM -C
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -C"
        exit 1
fi


RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

exit 0

