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
        echo "Test failed: start_lighttpd RealServer2"        exit 1
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

$L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer2_ADDR}:${RealServer2_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r ${RealServer2_ADDR}:${RealServer2_PORT}"
        exit 1
fi

RET=`$L7VSADM -e -t localhost:40001 -m ip -r $RealServer1_ADDR:$RealServer1_PORT -M -T 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -e -t localhost:40001 -m ip -r $RealServer1_ADDR:$RealServer1_PORT -M -T"
        exit 1
fi

RET=`$L7VSADM -e -t localhost:40002 -m ip -r $RealServer1_ADDR:$RealServer1_PORT --masq --tproxy 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -e -t localhost:40002 -m ip -r $RealServer1_ADDR:$RealServer1_PORT --masq --tproxy"
        exit 1
fi

RET=`$L7VSADM -e -t localhost:40003 -m ip -r $RealServer1_ADDR:$RealServer1_PORT -M --tproxy 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -e -t localhost:40003 -m ip -r $RealServer1_ADDR:$RealServer1_PORT -M --tproxy"
        exit 1
fi

RET=`$L7VSADM -e -t localhost:40004 -m ip -r $RealServer1_ADDR:$RealServer1_PORT --masq -T 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -e -t localhost:40004 -m ip -r $RealServer1_ADDR:$RealServer1_PORT --masq -T"
        exit 1
fi

RET=`$L7VSADM -e -t localhost:40005 -m ip -r $RealServer2_ADDR:$RealServer2_PORT -M -T 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -e -t localhost:40005 -m ip -r $RealServer2_ADDR:$RealServer2_PORT -M -T"
        exit 1
fi

RET=`$L7VSADM -e -t localhost:40006 -m ip -r $RealServer2_ADDR:$RealServer2_PORT --masq --tproxy 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -e -t localhost:40006 -m ip -r $RealServer2_ADDR:$RealServer2_PORT --masq --tproxy"
        exit 1
fi

RET=`$L7VSADM -e -t localhost:40007 -m ip -r $RealServer2_ADDR:$RealServer2_PORT -M --tproxy 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -e -t localhost:40007 -m ip -r $RealServer2_ADDR:$RealServer2_PORT -M --tproxy"
        exit 1
fi

RET=`$L7VSADM -e -t localhost:40008 -m ip -r $RealServer2_ADDR:$RealServer2_PORT --masq -T 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -e -t localhost:40008 -m ip -r $RealServer2_ADDR:$RealServer2_PORT --masq -T"
        exit 1
fi

exit 0

