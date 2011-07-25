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

# Check PID file
RET=`ps aux | grep "$L7DIRECTORD start" | grep -v grep | awk '{print $2}'`
EXPECT=`cat /var/run/l7directord.l7directord.pid`
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ps aux | grep \"$L7DIRECTORD start\" | grep -v grep | awk '{print $2}' != cat /var/run/l7directord.l7directord.pid"
        exit 1
fi

RET=`ps aux | grep "$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/1.cf" | grep -v grep | awk '{print $2}'`
EXPECT=`cat /var/run/l7directord.1.pid`
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/1.cf | grep -v grep | awk '{print \$2}' != cat /var/run/l7directord.1.pid"
        exit 1
fi

RET=`ps aux | grep "$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/2.cf" | grep -v grep | awk '{print $2}'`
EXPECT=`cat /var/run/l7directord.2.pid`
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/2.cf | grep -v grep | awk '{print \$2}' != cat /var/run/l7directord.2.pid"
        exit 1
fi

RET=`ps aux | grep "$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/3.cf" | grep -v grep | awk '{print $2}'`
EXPECT=`cat /var/run/l7directord.3.pid`
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/3.cf | grep -v grep | awk '{print \$2}' != cat /var/run/l7directord.3.pid"
        exit 1
fi

exit 0

