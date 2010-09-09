#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-4-3-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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

RET=`ps -ef | grep "l7directord: connect:127.0.0.1:50001:UP"`
if [ -z "$RET"  ]
then
        echo "Test failed: ps -ef | grep l7directord: connect:127.0.0.1:50001:UP"
        exit 1
fi

RET=`ps -ef | grep "l7directord: connect:127.0.0.1:50002:UP"`
if [ -z "$RET"  ]
then
        echo "Test failed: ps -ef | grep l7directord: connect:127.0.0.1:50002:UP"
        exit 1
fi

$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0601\] Service up detected. (Real server .127.0.0.1:50001')"`
if [ -z "$RET"  ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0601\] Service up detected. (Real server .127.0.0.1:50002')"`
if [ -z "$RET"  ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

exit 0

