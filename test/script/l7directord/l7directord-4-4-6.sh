#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-4-6-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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
sleep 3

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

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0601\] Service up detected. (Real server .127.0.0.1:50001')"`
if [ -z "$RET"  ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

exit 0

