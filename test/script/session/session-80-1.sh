#!/bin/bash
. ${SET_DEFAULT_CONF}

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -l ${L7VS_LOG_DIR}/session-80-real1-access.log
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

# Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

# Add service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z $L7VSD_CONF_DIR/sslproxy/sslproxy.target.cf
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} --masq
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT} --masq"
        exit 1
fi

#Connect
RET=`curl --retry 0 -k --interface 127.0.0.5 https://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
        echo "curl --retry 0 -k --interface 127.0.0.5 https://127.0.0.1:40001/"
        exit 1
fi

#Check access logs
RET=`cat ${L7VS_LOG_DIR}/session-80-real1-access.log | awk '{print $1}'`
EXPECT="127.0.0.1"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/session-80-real1-access.log"
        exit 1
fi

exit 0

