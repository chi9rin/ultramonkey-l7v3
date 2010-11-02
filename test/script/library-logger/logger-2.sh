#!/bin/bash

# set config file
cp materials/logger-2-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

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
RET=`wget -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
	echo "Test failed: wget -t 1 -qO- http://127.0.0.1:40001/"
	exit 1
fi

RET=`wget -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
	echo "Test failed: wget -t 1 -qO- http://127.0.0.1:40001/"
	exit 1
fi

RET=`wget -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
	echo "Test failed: wget -t 1 -qO- http://127.0.0.1:40001/"
	exit 1
fi

stop_lighttpd $RealServer1

$INIT_L7VSD stop
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSD"
        exit 1
fi
usleep 500000

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "\[DEBUG\]" | wc -l`
if [ ${RET} -gt 0 ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "\[INFO\]" | wc -l`
if [ ${RET} -le 0 ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

exit 0

