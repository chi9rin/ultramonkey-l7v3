#!/bin/bash
. ${SET_DEFAULT_CONF}

\cp -p ./materials/l7directord-5-2-24-callback.sh ${L7DIRECTORD_CONF_DIR}/realrecovercallback.sh
sed "s|XXX|${L7DIRECTORD_CONF_DIR}/realrecovercallback.sh|g" ./materials/l7directord-5-2-24-recover-l7directord.cf > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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

#Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Start l7directord
$L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi
sleep 1
stop_lighttpd $RealServer1
sleep 20
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
sleep 20
RET=`cat ${L7VS_LOG_DIR}/callbacktest`
EXPECT="callback is executed."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/callbacktest"
        exit 1
fi

exit 0

