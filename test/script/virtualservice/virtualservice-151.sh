#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}
\cp -f materials/virtualservice-151-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf
\cp -f materials/virtualservice-151-sslproxy.target.cf ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf

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

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

for (( i = 0 ; i < 3; i++ )){
	RET=`wget -qO- https://127.0.0.1:40001/ --no-check-certificate`
	if [ "${RET}" != "${RealServer1}" ]
	then
	        echo "wget -qO- https://127.0.0.1:40001/ --no-check-certificate"
	        exit 1
	fi
}

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "SSL session cache information: Session number\[2\]"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

sleep 10

RET=`wget -qO- https://127.0.0.1:40001/ --no-check-certificate`
if [ "${RET}" != "${RealServer1}" ]
then
        echo "wget -qO- https://127.0.0.1:40001/ --no-check-certificate"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "SSL session cache information: Session number\[0\]"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

stop_lighttpd $RealServer1

exit 0

