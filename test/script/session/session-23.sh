#!/bin/bash

. ${SET_DEFAULT_CONF}

#Run http server
SorryServer1=SorryServer1
SorryServer1_ADDR=127.0.0.1
SorryServer1_PORT=50001
start_lighttpd -s $SorryServer1 -a $SorryServer1_ADDR -p $SorryServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer1"
        exit 1
fi

RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50002
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

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -u 4
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -u 4"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT1=$!
usleep 100000

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT2=$!
usleep 100000

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT3=$!
usleep 100000

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT4=$!
usleep 100000

#Connect
RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${SorryServer1}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

# kill active connect
kill ${CONNECT1}
if [ $? -ne 0 ]
then
        echo "Test failed: kill ${CONNECT1}"
        exit 1
fi

#Connect
RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

# kill active connect
kill ${CONNECT2}
if [ $? -ne 0 ]
then
        echo "Test failed: kill ${CONNECT2}"
        exit 1
fi

#Connect
RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

# kill active connect
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

#Connect
RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

stop_lighttpd $RealServer1
stop_lighttpd $SorryServer1

exit 0

