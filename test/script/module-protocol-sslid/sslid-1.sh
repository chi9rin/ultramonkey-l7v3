#!/bin/bash

. ${SET_DEFAULT_CONF}

#Run http server
SSLRealServer1=SSLRealServer1
SSLRealServer1_ADDR=127.0.0.1
SSLRealServer1_PORT=54431
start_lighttpd -s $SSLRealServer1 -a $SSLRealServer1_ADDR -p $SSLRealServer1_PORT -S
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SSLRealServer1"
        exit 1
fi

SSLRealServer2=SSLRealServer2
SSLRealServer2_ADDR=127.0.0.1
SSLRealServer2_PORT=54432
start_lighttpd -s $SSLRealServer2 -a $SSLRealServer2_ADDR -p $SSLRealServer2_PORT -S
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SSLRealServer2"
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
$L7VSADM -A -t 127.0.0.1:44431 -m sslid
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m sslid"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:44431 -m sslid -r ${SSLRealServer1_ADDR}:${SSLRealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:44431 -m sslid -r ${SSLRealServer1_ADDR}:${SSLRealServer1_PORT}"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:44431 -m sslid -r ${SSLRealServer2_ADDR}:${SSLRealServer2_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:44431 -m sslid -r ${SSLRealServer2_ADDR}:${SSLRealServer2_PORT}"
        exit 1
fi

#Connect
RET=`curl -k --sslv3 https://127.0.0.1:44431/ https://127.0.0.1:44431/`
EXPECT=`echo -e "${SSLRealServer1}\n${SSLRealServer1}"`
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: curl -k --sslv3 https://127.0.0.1:44431/ https://127.0.0.1:44431/"
        exit 1
fi
usleep 100000

RET=`$L7VSADM -l -n | grep "${SSLRealServer1_ADDR}:${SSLRealServer1_PORT}" | awk '{print $6}'`
if [ "${RET}" != "2" ]
then
        echo "Test failed: $L7VSADM -l -n | grep ${RealServer1_ADDR}:${RealServer1_PORT} | awk '{print $6}'"
        exit 1
fi

RET=`$L7VSADM -l -n | grep "${SSLRealServer2_ADDR}:${SSLRealServer2_PORT}" | awk '{print $6}'`
if [ "${RET}" != "0" ]
then
        echo "Test failed: $L7VSADM -l -n | grep ${RealServer2_ADDR}:${RealServer2_PORT} | awk '{print $6}'"
        exit 1
fi

stop_lighttpd $SSLRealServer1
stop_lighttpd $SSLRealServer2

exit 0

