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
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless -s wrr"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 0"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT} -w 6
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT} -w 6"
        exit 1
fi


RET=`wget -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer2}" ]
then
        echo "Test failed: wget -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

$L7VSADM -e -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 9
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -e -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 9"
        exit 1
fi

RET=`wget -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
        echo "Test failed: wget -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

exit 0

