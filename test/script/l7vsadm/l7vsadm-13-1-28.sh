#!/bin/bash

. ${SET_DEFAULT_CONF}

#Run nc server
#nc -i 1 -l 9876 > ${TMP_DIR}/nc_tmp &
#NC_PID=$!

#chkconfig echo-stream on

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001

RealServer2=RealServer2
RealServer2_ADDR=127.0.0.1
RealServer2_PORT=50002

start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT

if [ $? -ne 0 ]
then
	echo "Test failed: start_lighttpd RealServer1"
	exit 1
fi

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless --statistic 1 -F 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless --statistic 1 -F"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

$L7VSADM -A -t 127.0.0.1:40002 -m sessionless --statistic 1 -F 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless --statistic 1 -F"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40002 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40002 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT}"
        exit 1
fi


#Connect
GET_REQ="GET / HTTP/1.0\r\n\r\n"
POST_REQ="POST / HTTP/1.0\r\n\r\n"
for ((I=0; I<100;I++)) 
{
    echo -e -n "${GET_REQ}" | nc 127.0.0.1 40001 >/dev/null 2>&1
    echo -e -n "${GET_REQ}" | nc 127.0.0.1 40002 >/dev/null 2>&1
}

for ((I=0; I<200;I++)) 
{
    echo -e -n "${POST_REQ}" | nc 127.0.0.1 40001 >/dev/null 2>&1
    echo -e -n "${POST_REQ}" | nc 127.0.0.1 40002 >/dev/null 2>&1
}

#HTTP Total Requests check
RET=`$L7VSADM -V -n | grep "HTTP Total Requests" | awk '{ print $4}'`
EXPECT="300
300"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP Total Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP GET Requests check
RET=`$L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'`
EXPECT="100
100"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP POST Requests check
RET=`$L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}'`
EXPECT="200
200"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}"
        exit 1
fi

#clear statistic(using long option)
$L7VSADM --snmp --refresh --tcp-service  127.0.0.1:40002 --proto-module sessionless 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM --snmp --refresh --tcp-service  127.0.0.1 40002 --proto-module sessionless "
        exit 1
fi

#HTTP Total Requests check
RET=`$L7VSADM -V -n | grep "HTTP Total Requests" | awk '{ print $4}'`
EXPECT="300
0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP Total Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP GET Requests check
RET=`$L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'`
EXPECT="100
0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP POST Requests check
RET=`$L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}'`
EXPECT="200
0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}"
        exit 1
fi

exit 0

