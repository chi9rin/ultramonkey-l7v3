#!/bin/bash
. ${SET_DEFAULT_CONF}

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

RealServer2=RealServer2
RealServer2_ADDR=[::1]
RealServer2_PORT=50002
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer2"
        exit 1
fi

RealServer3=RealServer3
RealServer3_ADDR=127.0.0.1
RealServer3_PORT=50003
start_lighttpd -s $RealServer3 -a $RealServer3_ADDR -p $RealServer3_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer3"
        exit 1
fi

RealServer4=RealServer4
RealServer4_ADDR=[::1]
RealServer4_PORT=50004
start_lighttpd -s $RealServer4 -a $RealServer4_ADDR -p $RealServer4_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer4"
        exit 1
fi

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

$L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT} -T
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT} -T"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer2_ADDR}:${RealServer2_PORT} -T
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer2_ADDR}:${RealServer2_PORT} -T"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer3_ADDR}:${RealServer3_PORT} --tproxy
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer3_ADDR}:${RealServer3_PORT} --tproxy"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer4_ADDR}:${RealServer4_PORT} --tproxy
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer4_ADDR}:${RealServer4_PORT} --tproxy"
        exit 1
fi

RET=`$L7VSADM -l -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 ip rr
  -> ${RealServer1_ADDR}:${RealServer1_PORT}              Tproxy  1      0          0         
  -> ${RealServer2_ADDR}:${RealServer2_PORT}                  Tproxy  1      0          0         
  -> ${RealServer3_ADDR}:${RealServer3_PORT}              Tproxy  1      0          0         
  -> ${RealServer4_ADDR}:${RealServer4_PORT}                  Tproxy  1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

exit 0

