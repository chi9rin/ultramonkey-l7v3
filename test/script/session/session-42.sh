#!/bin/bash

# set config file
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

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}"
	exit 1
fi

#Connect
RET=`wget -t 1 -T 1 -O- https://127.0.0.1:40001/ --no-check-certificate`
if [ $? -eq 0 ]
then
        echo "Test failed: wget -qO- -t 1 http://127.0.0.1:40001/ "
        exit 1
fi

if [ -n "${RET}" ]
then
        echo "Test failed: wget -qO- -t 1 http://127.0.0.1:40001/ "
        exit 1
fi

exit 0

