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

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -u 2
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer1_ADDR}:${SorryServer1_PORT} -u 2"
        exit 1
fi

#Connect
RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${SorryServer1}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

stop_lighttpd $SorryServer1

exit 0

