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
RealServer2_ADDR=127.0.0.1
RealServer2_PORT=50002
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer2"
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
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -s wrr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -s wrr"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 0"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT} -w 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT} -w 0"
        exit 1
fi

#Connect
RET=`LANG=C $WGET -t 1 -O- http://127.0.0.1:40001/ 2>&1`
if [ $? -eq 0 ]
then
        echo "Test failed: LANG=C $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

RET=`echo "$RET" | grep "Connecting to .* connected."`
if [ -z "${RET}" ]
then
	echo"Test failed: echo $RET | grep Connecting to .* connected."
        exit 1
fi

RET=`LANG=C $WGET -t 1 -O- http://127.0.0.1:40001/ 2>&1`
if [ $? -eq 0 ]
then
        echo "Test failed: LANG=C $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

RET=`echo "$RET" | grep "Connecting to .* connected."`
if [ -z "${RET}" ]
then
	echo"Test failed: echo $RET | grep Connecting to .* connected."
        exit 1
fi

stop_lighttpd $RealServer1
stop_lighttpd $RealServer2

exit 0

