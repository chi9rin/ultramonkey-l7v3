#!/bin/bash

. ${SET_DEFAULT_CONF}

VS="127.0.0.1"
SS="127.0.0.1"

#Run http server
SorryServer1=SorryServer1
SorryServer1_ADDR=$SS
SorryServer1_PORT=50001
start_lighttpd -s $SorryServer1 -a $SorryServer1_ADDR -p $SorryServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer1"
        exit 1
fi

#Add Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t $VS:40001 -m sessionless -s rr -u 100 -b $SS:50001 -f 0 -Q 200K -q 200k
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS:40001 -m sessionless -s rr -u 100 -b $SS:50001 -f 0 -Q 200K -q 200k"
        exit 1
fi

RET=`$L7VSADM -l`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless rr"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi

$L7VSADM -E -t $VS:40001 -m sessionless -s wrr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -E -t $VS:40001 -m sessionless -s wrr"
        exit 1
fi

RET=`$L7VSADM -l`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless wrr"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi

exit 0


