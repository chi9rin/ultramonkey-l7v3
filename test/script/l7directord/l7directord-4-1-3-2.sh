#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-1-3-2-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

exit 1
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
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 5
 
tcpdump -i eth0 port 50001
exit 1

RET=`ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print $8,$9}'`
EXPECT="/usr/sbin/l7directord start
l7directord: http:127.0.0.1:50001:UP
l7directord: http:127.0.0.1:50002:UP"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ps -ef | grep l7directord | grep -v grep | grep -v umtest.sh | awk '{print \$8,\$9}'"
        exit 1
fi

exit 0

