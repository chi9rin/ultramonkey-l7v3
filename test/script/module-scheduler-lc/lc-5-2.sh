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

RealServer3=RealServer3
RealServer3_ADDR=127.0.0.1
RealServer3_PORT=50003
start_lighttpd -s $RealServer3 -a $RealServer3_ADDR -p $RealServer3_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer3"
        exit 1
fi

#Start L7vsd
$L7VSD
usleep 100000
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

#Add Service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -s lc
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -s lc"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 100
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 100"
        exit 1
fi

# Check active connects
RET=`$L7VSADM -l -n | grep "${RealServer1_ADDR}:${RealServer1_PORT}" | awk '{print $5}'`
if [ "${RET}" -ne 0 ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer1_ADDR}:${RealServer1_PORT} | awk '{print $5}'"
	exit 1
fi

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT1=$!
usleep 100000
RET=`$L7VSADM -l -n | grep "${RealServer1_ADDR}:${RealServer1_PORT}" | awk '{print $5}'`
if [ "${RET}" != "1" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer1_ADDR}:${RealServer1_PORT} | awk '{print $5}'" 
        exit 1
fi

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT2=$!
usleep 100000
RET=`$L7VSADM -l -n | grep "${RealServer1_ADDR}:${RealServer1_PORT}" | awk '{print $5}'`
if [ "${RET}" != "2" ]
then
	 echo "Test failed: $L7VSADM -l -n | grep ${RealServer1_ADDR}:${RealServer1_PORT} | awk '{print $5}'" 
         exit 1
fi


$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT} -w 50

RET=`$L7VSADM -l -n | grep "${RealServer1_ADDR}:${RealServer1_PORT}" | awk '{print $5}'`
if [ "${RET}" != "2" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer1_ADDR}:${RealServer1_PORT} | awk '{print $5}'" 
	exit 1
fi

RET=`$L7VSADM -l -n | grep "${RealServer2_ADDR}:${RealServer2_PORT}" | awk '{print $5}'`
if [ "${RET}" != "0" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer2_ADDR}:${RealServer2_PORT} | awk '{print $5}'" 
        exit 1
fi

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT3=$!
usleep 100000
RET=`$L7VSADM -l -n | grep "${RealServer2_ADDR}:${RealServer2_PORT}" | awk '{print $5}'`
if [ "${RET}" != "1" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer2_ADDR}:${RealServer2_PORT} | awk '{print $5}'" 
        exit 1
fi

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT4=$!
usleep 100000
RET=`$L7VSADM -l -n | grep "${RealServer2_ADDR}:${RealServer2_PORT}" | awk '{print $5}'`
if [ "${RET}" != "2" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer2_ADDR}:${RealServer2_PORT} | awk '{print $5}'" 
	exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer3_ADDR}:${RealServer3_PORT} -w 1
RET=`$L7VSADM -l -n | grep "${RealServer1_ADDR}:${RealServer1_PORT}" | awk '{print $5}'`
if [ "${RET}" != "2" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer1_ADDR}:${RealServer1_PORT} | awk '{print $5}'" 
        exit 1
fi

RET=`$L7VSADM -l -n | grep "${RealServer2_ADDR}:${RealServer2_PORT}" | awk '{print $5}'`
if [ "${RET}" != "2" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer2_ADDR}:${RealServer2_PORT} | awk '{print $5}'" 
        exit 1
fi

RET=`$L7VSADM -l -n | grep "${RealServer3_ADDR}:${RealServer3_PORT}" | awk '{print $5}'`
if [ "${RET}" != "0" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer3_ADDR}:${RealServer3_PORT} | awk '{print $5}'" 
        exit 1
fi

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT5=$!
usleep 100000
RET=`$L7VSADM -l -n | grep "${RealServer3_ADDR}:${RealServer3_PORT}" | awk '{print $5}'`
if [ "${RET}" != "1" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer3_ADDR}:${RealServer3_PORT} | awk '{print $5}'" 
        exit 1
fi

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT6=$!
usleep 100000
RET=`$L7VSADM -l -n | grep "${RealServer3_ADDR}:${RealServer3_PORT}" | awk '{print $5}'`
if [ "${RET}" != "2" ]
then
	echo "Test failed: $L7VSADM -l -n | grep ${RealServer3_ADDR}:${RealServer3_PORT} | awk '{print $5}'" 
        exit 1
fi


# kill active connect
kill ${CONNECT1}
if [ $? -ne 0 ]
then
        echo "Test failed: kill ${CONNECT1}"
        exit 1
fi

kill ${CONNECT2}
if [ $? -ne 0 ]
then
        echo "Test failed: kill ${CONNECT2}"
        exit 1
fi

kill ${CONNECT3}
if [ $? -ne 0 ]
then
        echo "Test failed: kill ${CONNECT3}"
        exit 1
fi

kill ${CONNECT4}
if [ $? -ne 0 ]
then
        echo "Test failed: kill ${CONNECT4}"
        exit 1
fi

kill ${CONNECT5}
if [ $? -ne 0 ]
then
        echo "Test failed: kill ${CONNECT5}"
        exit 1
fi

kill ${CONNECT6}
if [ $? -ne 0 ]
then
        echo "Test failed: kill ${CONNECT6}"
        exit 1
fi

exit 0

