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
$L7VSADM -A -t 127.0.0.1:40001 -m ip --timeout 10
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip --timeout 10"
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

#Connect
RET=`curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
	echo "curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/"
	exit 1
fi

sleep 8

RET=`curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
	echo "curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/"
	exit 1
fi

sleep 12

RET=`curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer2}" ]
then
	echo "curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/"
	exit 1
fi

sleep 8

RET=`curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer2}" ]
then
	echo "curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/"
	exit 1
fi

sleep 12

RET=`curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
	echo "curl --retry 0 --interface 127.0.0.1 http://127.0.0.1:40001/"
	exit 1
fi

stop_lighttpd $RealServer1
stop_lighttpd $RealServer2

exit 0

