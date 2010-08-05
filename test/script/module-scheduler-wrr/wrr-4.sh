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


#Add Service
$L7VSD
usleep 100000
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -s wrr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -s wrr"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} -w 1"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT} -w 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT} -w 1"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer3_ADDR}:${RealServer3_PORT} -w 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer3_ADDR}:${RealServer3_PORT} -w 1"
        exit 1
fi

#Connect
RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer2}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer3}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer2}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer3}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

stop_lighttpd $RealServer1
stop_lighttpd $RealServer2
stop_lighttpd $RealServer3

exit 0

