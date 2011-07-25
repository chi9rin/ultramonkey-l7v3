#!/bin/bash
. ${SET_DEFAULT_CONF}
cat /dev/null > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Start l7directord
$INIT_L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
usleep 100000

# edit config file
echo "foobar" > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Restart l7directord
RET=`$INIT_L7DIRECTORD reload 2>&1`
EXPECT="Configuration error. Reading file \`/etc/ha.d/conf/l7directord.cf' at line 1: Unknown config \`foobar'."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD reload"
        exit 1
fi

RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -z "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

exit 0

