#!/bin/bash
. ${SET_DEFAULT_CONF}

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
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
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

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r $RealServer1_ADDR:$RealServer1_PORT"
        exit 1
fi

#Connect
GET_REQ=""
for((I=0;I<100;I++)) do GET_REQ="${GET_REQ}GET / HTTP/1.0\r\n\r\n"; done
POST_REQ=""
for((I=0;I<200;I++)) do POST_REQ="${POST_REQ}POST / HTTP/1.0\r\n\r\n"; done
echo -e -n "${GET_REQ}" | nc 127.0.0.1 40001
echo -e -n "${POST_REQ}" | nc 127.0.0.1 40001

#HTTP Total Requests check
RET=`$L7VSADM -V -n | grep "HTTP Total Requests" | awk '{print $4}'`
EXPECT="300"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP Total Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP GET Requests check
RET=`$L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'`
EXPECT="100"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP POST Requests check
RET=`$L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}'`
EXPECT="200"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}"
        exit 1
fi

#clear statistic
$L7VSADM -S -r
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -r"
        exit 1
fi


#HTTP Total Requests check
RET=`$L7VSADM -V -n | grep "HTTP Total Requests" | awk '{print $4}'`
EXPECT="0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP Total Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP GET Requests check
RET=`$L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'`
EXPECT="0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'"
        exit 1
fi

#HTTP POST Requests check
RET=`$L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}'`
EXPECT="0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}"
        exit 1
fi


exit 0

