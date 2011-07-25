#!/bin/bash

. ${SET_DEFAULT_CONF}

VS1="[::1]"
VS2="[::1]"

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
RealServer1_ADDR=[::1]
RealServer1_PORT=50001

RealServer2=RealServer2
RealServer2_ADDR=[::1]
RealServer2_PORT=50002

start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT -i
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer2"
        exit 1
fi


$L7VSADM -A -t $VS1:40001 -m ip -c 1 -F 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS1:40001 -m ip -c 1 -F "
        exit 1
fi

$L7VSADM -a -t $VS1:40001 -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS1:40001 -m ip -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

$L7VSADM -A -t $VS2:40002 -m ip -c 1 -F 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS2:40002 -m ip -c 1 -F "
        exit 1
fi

$L7VSADM -a -t $VS2:40002 -m ip -r ${RealServer2_ADDR}:${RealServer2_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS2:40002 -m ip -r ${RealServer2_ADDR}:${RealServer2_PORT}"
        exit 1
fi

RET=`ifconfig lo | grep ::2`
if [ -z "${RET}" ]
then
	ifconfig lo add ::2
fi

#Connect
GET_REQ="GET / HTTP/1.0\r\n\r\n"
POST_REQ="POST / HTTP/1.0\r\n\r\n"
for ((I=0; I<100;I++)) 
{
    echo -e -n "${GET_REQ}" | nc ::1 40001 -s ::2 >/dev/null 2>&1
    echo -e -n "${GET_REQ}" | nc ::1 40002 -s ::2 >/dev/null 2>&1
}

for ((I=0; I<200;I++)) 
{
    echo -e -n "${POST_REQ}" | nc ::1 40001 -s ::2 >/dev/null 2>&1
    echo -e -n "${POST_REQ}" | nc ::1 40002 -s ::2 >/dev/null 2>&1
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
$L7VSADM --snmp --refresh 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM --snmp --refresh"
        exit 1
fi

#HTTP Total Requests check
RET=`$L7VSADM -V -n | grep "HTTP Total Requests" | awk '{ print $4}'`
EXPECT="0
0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP Total Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP GET Requests check
RET=`$L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'`
EXPECT="0
0"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP POST Requests check
RET=`$L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}'`
EXPECT="0
0"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}"
        exit 1
fi



exit 0


