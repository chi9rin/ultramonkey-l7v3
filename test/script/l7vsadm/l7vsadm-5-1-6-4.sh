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

RET=`$L7VSADM -a -t localhost: -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT} 2>&1 | grep "PARSE ERROR: target endpoint parse error(--tcp-service): invalid port number (0):localhost:"`
EXPECT="PARSE ERROR: target endpoint parse error(--tcp-service): invalid port number (0):localhost:"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -a -t localhost: -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

exit 0

