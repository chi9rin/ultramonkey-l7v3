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

$L7VSADM -A -t localhost:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless"
        exit 1
fi

$L7VSADM -a -t localhost:40001 -m sessionless -r localhost:40002 -w 3
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t localhost:40001 -m sessionless -r localhost:40002 -w 3"
        exit 1
fi

$L7VSADM -a -t localhost:40001 -m sessionless -r localhost:40003 -w 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t localhost:40001 -m sessionless -r localhost:40003 -w 0"
        exit 1
fi

RET=`$L7VSADM -l`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 sessionless rr
  -> localhost:40002              Masq    3      0          0         
  -> localhost:40003              Masq    0      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi

exit 0

