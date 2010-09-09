#!/bin/bash
. ${SET_DEFAULT_CONF}
touch  ${L7DIRECTORD_CONF_DIR}/1.cf
touch  ${L7DIRECTORD_CONF_DIR}/2.cf
touch  ${L7DIRECTORD_CONF_DIR}/3.cf

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

$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/1.cf start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/1.cf start"
        exit 1
fi

$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/2.cf start 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/2.cf start"
        exit 1
fi

$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/3.cf start 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/3.cf start"
        exit 1
fi

# Stop l7directord
$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi

$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/1.cf stop
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/1.cf stop"
        exit 1
fi

$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/2.cf stop
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/2.cf stop"
        exit 1
fi

$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/3.cf stop
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/3.cf stop"
        exit 1
fi

# Check PID file
RET=`ls /var/run/l7directord.l7directord.pid`
if [ -n "$RET" ]
then
        echo "Test failed: ls /var/run/l7directord.l7directord.pid"
        exit 1
fi

RET=`ls /var/run/l7directord.1.pid`
if [ -n "$RET" ]
then
        echo "Test failed: ls /var/run/l7directord.1.pid"
        exit 1
fi

RET=`ls /var/run/l7directord.2.pid`
if [ -n "$RET" ]
then
        echo "Test failed: ls /var/run/l7directord.2.pid"
        exit 1
fi

RET=`ls /var/run/l7directord.3.pid`
if [ -n "$RET" ]
then
        echo "Test failed: ls /var/run/l7directord.3.pid"
        exit 1
fi

exit 0

