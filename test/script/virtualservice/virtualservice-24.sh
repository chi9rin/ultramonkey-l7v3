#!/bin/bash

. ${SET_DEFAULT_CONF}
\cp -f materials/virtualservice-24-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

PATH=$PATH:$COMMON_SCRIPT_DIR

#Add Service
start_l7vsd || exit 1

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

# pool size = 2
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m sessionless"
        exit 1
fi
$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t localhost:40001 -m sessionless -r 127.0.0.1:50001"
        exit 1
fi

# connect 2 client
test_client 127.0.0.1 40001 > /dev/null 2>&1 &
test_client 127.0.0.1 40001 > /dev/null 2>&1 &

# and more (but will be waited)
test_client 127.0.0.1 40001 > /dev/null 2>&1 &

sleep 1

# kill first client
pkill -o test_client

# then last client will be accept
sleep 1

logfile=/var/log/l7vs/l7vsd.log
active_result=`grep active_session $logfile | awk '{print $8}' | uniq`
active_expect="0
1
2
1
2"

# kill all client
killall test_client

if [ "$active_result" != "$active_expect" ]; then
	echo "Test failed: Mismatch active session threads."
	exit 1
fi
exit 0

