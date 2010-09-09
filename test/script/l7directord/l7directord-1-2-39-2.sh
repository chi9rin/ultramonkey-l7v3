#!/bin/bash
. ${SET_DEFAULT_CONF}

#Start l7directord
$L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi
usleep 100000


RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -z "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

RET=`$INIT_L7DIRECTORD status 2>&1 | egrep "l7directord for ./etc/ha.d/conf/l7directord.cf. is running with pid:"`
if [ -z "$RET" ]
then
        echo "Test failed: $INIT_L7DIRECTORD status"
        exit 1
fi

exit 0

