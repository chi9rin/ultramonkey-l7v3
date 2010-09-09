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

RET=`$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf status 2>&1 | egrep "l7directord for .${L7DIRECTORD_CONF_DIR}/specified.cf. is running with pid:"`
if [ -z "$RET" ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf status"
        exit 1
fi

exit 0

