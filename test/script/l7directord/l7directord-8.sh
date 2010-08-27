#!/bin/bash
. ${SET_DEFAULT_CONF}
cat /dev/null > ${L7DIRECTORD_CONF_DIR}/specified.cf

#Start l7directord
$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf start"
        exit 1
fi
usleep 100000


RET=`ps -ef | grep "l7directord ${L7DIRECTORD_CONF_DIR}/specified.cf start"| grep -v grep`
if [ -z "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord ${L7DIRECTORD_CONF_DIR}/specified.cf start | grep -v grep"
        exit 1
fi

$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf stop
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf stop"
        exit 1
fi
usleep 100000


RET=`ps -ef | grep "l7directord ${L7DIRECTORD_CONF_DIR}/specified.cf start"| grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord ${L7DIRECTORD_CONF_DIR}/specified.cf start | grep -v grep"
        exit 1
fi

exit 0

