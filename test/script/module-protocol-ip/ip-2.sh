#!/bin/bash
. ${SET_DEFAULT_CONF}

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=[::1]
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -i

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
RealServer3_ADDR=[::1]
RealServer3_PORT=50003
start_lighttpd -s $RealServer3 -a $RealServer3_ADDR -p $RealServer3_PORT -i

if [ $? -ne 0 ]
then
	echo "Test failed: start_lighttpd RealServer3"
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
$L7VSADM -A -t [::1]:40001 -m ip
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -A -t [::1]:40001 -m ip"
        exit 1
fi

$L7VSADM -a -t [::1]:40001 -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -a -t [::1]:40001 -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

$L7VSADM -a -t [::1]:40001 -m ip -r ${RealServer2_ADDR}:${RealServer2_PORT}
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -a -t [::1]:40001 -m ip -r ${RealServer2_ADDR}:${RealServer2_PORT}"
        exit 1
fi

$L7VSADM -a -t [::1]:40001 -m ip -r ${RealServer3_ADDR}:${RealServer3_PORT}
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -a -t [::1]:40001 -m ip -r ${RealServer3_ADDR}:${RealServer3_PORT}"
        exit 1
fi

#Add IPaliasing
ifconfig lo:0 add ::2
ifconfig lo:1 add ::3

#Connect
RET=`curl -g --retry 0 --interface ::1 http://[::1]:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
	echo "Test failed: curl -g --retry 0 --interface ::1 http://[::1]:40001/"
	exit 1
fi

RET=`curl -g --retry 0 --interface ::1 http://[::1]:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
	echo "Test failed: curl -g --retry 0 --interface ::1 http://[::1]:40001/"
	exit 1
fi

RET=`curl -g --retry 0 --interface ::2 http://[::1]:40001/`
if [ "${RET}" != "${RealServer2}" ]
then
	echo "Test failed: curl -g --retry 0 --interface ::2 http://[::1]:40001/"
	exit 1
fi

RET=`curl -g --retry 0 --interface ::2 http://[::1]:40001/`
if [ "${RET}" != "${RealServer2}" ]
then
	echo "Test failed: curl -g --retry 0 --interface ::2 http://[::1]:40001/"
	exit 1
fi

RET=`curl -g --retry 0 --interface ::3 http://[::1]:40001/`
if [ "${RET}" != "${RealServer3}" ]
then
	echo "Test failed: curl -g --retry 0 --interface ::3 http://[::1]:40001/"
	exit 1
fi

RET=`curl -g --retry 0 --interface ::3 http://[::1]:40001/`
if [ "${RET}" != "${RealServer3}" ]
then
	echo "Test failed: curl -g --retry 0 --interface ::3 http://[::1]:40001/"
	exit 1
fi

#Del IPaliasing
ifconfig lo:0 del ::2
ifconfig lo:1 del ::3

stop_lighttpd $RealServer1
stop_lighttpd $RealServer2
stop_lighttpd $RealServer3

exit 0

