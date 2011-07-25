#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-4-12-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -l ${L7VS_LOG_DIR}/l7directord-4-4-12-access.log

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
sleep 30

$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord-4-4-12-access.log | grep "GET / HTTP/1.1" | wc -l`
if [ $RET -ne 3 ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord-4-4-12-access.log"
        exit 1
fi


for HOST in `cat ${L7VS_LOG_DIR}/l7directord-4-4-12-access.log | grep "GET / HTTP/1.1" | awk '{ print $2}'`
do
        if [ "$HOST" != "webserver" ]
        then
		echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord-4-4-12-access.log"
        fi
done

exit 0

