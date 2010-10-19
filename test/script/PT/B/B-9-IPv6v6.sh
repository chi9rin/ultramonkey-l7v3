#!/bin/bash

. ${SET_DEFAULT_CONF}

VS="[::1]"
RS="[::1]"

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=$RS
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
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

$L7VSADM -A -t $VS:40001 -m sessionless -s rr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS:40001 -m sessionless -s rr"
        exit 1
fi

$L7VSADM -a -t $VS:40001 -m sessionless -r $RS:50001 --tproxy
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS:40001 -m sessionless -s rr"
        exit 1
fi

RET=`$L7VSADM -l`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless rr
  -> localhost:50001              Tproxy  1      0          0         "

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi

exit 0

