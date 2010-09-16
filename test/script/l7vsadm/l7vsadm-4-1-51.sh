#!/bin/bash
. ${SET_DEFAULT_CONF}

#Run http server
SorryServer1=SorryServer1
SorryServer1_ADDR=127.0.0.1
SorryServer1_PORT=50001
start_lighttpd -s $SorryServer1 -a $SorryServer1_ADDR -p $SorryServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer1"
        exit 1
fi

SorryServer2=SorryServer2
SorryServer2_ADDR=[::1]
SorryServer2_PORT=50002
start_lighttpd -s $SorryServer2 -a $SorryServer2_ADDR -p $SorryServer2_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer2"        exit 1
fi

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer1_ADDR:$SorryServer1_PORT -M -T 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer1_ADDR:$SorryServer1_PORT -M -T"
        exit 1
fi

RET=`$L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer1_ADDR:$SorryServer1_PORT --masq --tproxy 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer1_ADDR:$SorryServer1_PORT --masq --tproxy"
        exit 1
fi

RET=`$L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer1_ADDR:$SorryServer1_PORT -M --tproxy 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer1_ADDR:$SorryServer1_PORT -M --tproxy"
        exit 1
fi

RET=`$L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer1_ADDR:$SorryServer1_PORT --masq -T 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer1_ADDR:$SorryServer1_PORT --masq -T"
        exit 1
fi

RET=`$L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer2_ADDR:$SorryServer2_PORT -M -T 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer2_ADDR:$SorryServer2_PORT -M -T"
        exit 1
fi

RET=`$L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer2_ADDR:$SorryServer2_PORT --masq --tproxy 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer2_ADDR:$SorryServer2_PORT --masq --tproxy"
        exit 1
fi

RET=`$L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer2_ADDR:$SorryServer2_PORT -M --tproxy 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer2_ADDR:$SorryServer2_PORT -M --tproxy"
        exit 1
fi

RET=`$L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer2_ADDR:$SorryServer2_PORT --masq -T 2>&1 | grep "PARSE ERROR: Option --masq/tproxy is conflict."`
EXPECT="PARSE ERROR: Option --masq/tproxy is conflict."
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless -b $SorryServer2_ADDR:$SorryServer2_PORT --masq -T"
        exit 1
fi

exit 0

