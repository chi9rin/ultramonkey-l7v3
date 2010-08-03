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

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -A -t 127.0.0.1:40001 -m ip -s lc -u 200 -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -f 0 -Q 1M -q 1M -L 0 2>&1`

if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -s lc -u 200 -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -f 0 -Q 1M -q 1M -L 0"
        exit 1
fi

if [ -n "$RET" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -s lc -u 200 -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -f 0 -Q 1M -q 1M -L 0"
        exit 1
fi

RET=`$L7VSADM -E -t 127.0.0.1:40001 -m ip -s lc -u 200 -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -f 0 -Q 1M -q 1M -L 0 2>&1`

if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -E -t 127.0.0.1:40001 -m ip -s lc -u 200 -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -f 0 -Q 1M -q 1M -L 0"
        exit 1
fi

if [ -n "$RET" ]
then
        echo "Test failed: $L7VSADM -E -t 127.0.0.1:40001 -m ip -s lc -u 200 -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -f 0 -Q 1M -q 1M -L 0"
        exit 1
fi

exit 0

