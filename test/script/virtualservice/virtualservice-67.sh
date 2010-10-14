#!/bin/bash

. ${SET_DEFAULT_CONF}

# pool size = 2
\cp -f materials/virtualservice-67-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

# timeout = 2
\cp -f materials/virtualservice-67-sslproxy.target.cf ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf

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

# Start l7vsd
start_l7vsd || exit 1

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf
if [ $? -ne 0 ]; then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi
$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001
if [ $? -ne 0 ]; then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001"
        exit 1
fi

openssl s_client -ign_eof -connect 127.0.0.1:40001 > /dev/null 2>&1 &
openssl s_client -ign_eof -connect 127.0.0.1:40001 > /dev/null 2>&1 &
openssl s_client -ign_eof -connect 127.0.0.1:40001 > /dev/null 2>&1 &

sleep 3

act_conn=`l7vsadm -l -n | grep 50001 | awk '{print $5}'`
if [ "$act_conn" != "2" ]; then
	echo "Test failed: active connection must be 2 but $act_conn"
	exit 1
fi

s_client=`ps aux | grep s_client | grep -v grep | wc -l`
if [ $s_client -ne 3 ]; then
	echo "Test failed: three s_client processes must be exist."
	exit 1
fi

exit 0
