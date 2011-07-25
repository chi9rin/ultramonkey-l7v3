#!/bin/bash
. ${SET_DEFAULT_CONF}
touch  ${L7DIRECTORD_CONF_DIR}/1.cf
touch  ${L7DIRECTORD_CONF_DIR}/2.cf
touch  ${L7DIRECTORD_CONF_DIR}/3.cf
echo 1 >  /var/run/l7directord.l7directord.pid
echo 1 >  /var/run/l7directord.1.pid
echo 1 >  /var/run/l7directord.2.pid
echo 1 >  /var/run/l7directord.3.pid

# Start l7directord
RET=`$INIT_L7DIRECTORD try-restart 2>&1 | grep l7directord`
EXPECT="Try restarting l7directord ...  l7directord stale pid file /var/run/l7directord.l7directord.pid for /etc/ha.d/conf/l7directord.cf
l7directord process is not running."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $INIT_L7DIRECTORD try-restart"
        exit 1
fi

RET=`$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/1.cf try-restart 2>&1`
EXPECT="l7directord stale pid file /var/run/l7directord.1.pid for /etc/ha.d/conf/1.cf
l7directord process is not running."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/1.cf try-restart"
        exit 1
fi

RET=`$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/2.cf try-restart 2>&1`
EXPECT="l7directord stale pid file /var/run/l7directord.2.pid for /etc/ha.d/conf/2.cf
l7directord process is not running."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/2.cf try-restart"
        exit 1
fi

RET=`$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/3.cf try-restart 2>&1`
EXPECT="l7directord stale pid file /var/run/l7directord.3.pid for /etc/ha.d/conf/3.cf
l7directord process is not running."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/3.cf try-restart"
        exit 1
fi

exit 0

