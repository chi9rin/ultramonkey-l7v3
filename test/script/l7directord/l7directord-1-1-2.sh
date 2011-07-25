#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-1-1-2-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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

RealServer2=RealServer2
RealServer2_ADDR=127.0.0.1
RealServer2_PORT=50002
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT 
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer2"
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

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 15


RET=`grep -Ev "^\[" ${L7VS_LOG_DIR}/l7directord.log`
if [ -n "$RET" ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | wc -l`
if [ $RET -eq 0 ]
then
        echo "Test failed: wc -l ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

exit 0

