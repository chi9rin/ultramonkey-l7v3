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
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
if [ $? -ne 0 ]
then
	echo "Test failed: start_lighttpd RealServer1"
	exit 1
fi

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless --statistic 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless --statistic 0"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r $RealServer1_ADDR:$RealServer1_PORT"
        exit 1
fi

#Connect
echo -e -n "GET / HTTP/1.0\r\n\r\n" | nc 127.0.0.1 40001
echo -e -n "POST / HTTP/1.0\r\n\r\n" | nc 127.0.0.1 40001

#HTTP Total Requests check
RET=`$L7VSADM -V -n | grep "HTTP Total Requests" | awk '{print $4}'`
EXPECT="0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n"
        exit 1
fi

#HTTP GET Requests check
RET=`$L7VSADM -V -n | grep "HTTP GET Requests" | awk '{print $4}'`
EXPECT="0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n"
        exit 1
fi

#HTTP POST Requests check
RET=`$L7VSADM -V -n | grep "HTTP POST Requests" | awk '{print $4}'`
EXPECT="0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n"
        exit 1
fi



exit 0

