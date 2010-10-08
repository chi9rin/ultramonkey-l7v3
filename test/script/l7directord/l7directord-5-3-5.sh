#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-5-3-5-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 10

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:50000 sessionless rr"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -l -n"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0303\] Failed to command ./usr/sbin/l7vsadm -A -t 127.0.0.1:50001 -m ip --foo -s wrr -u 0 -b 0.0.0.0:0  -Q 0 -q 0 2>&1. with return: 65280"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0201\] Failed to add virtual service to l7vsd: .127.0.0.1:50001 ip ', output: .PARSE ERROR: protocol module argument error(--proto-module): Option error.'"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

exit 0
