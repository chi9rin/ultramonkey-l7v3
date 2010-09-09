#!/bin/bash
. ${SET_DEFAULT_CONF}
cat /dev/null > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

RET=`ps -ef | grep "l7directord start"| grep -v grep`
if [ -n "$RET" ]
then
        echo "Test failed: ps -ef | grep l7directord start | grep -v grep"
        exit 1
fi

RET=`$INIT_L7DIRECTORD reload 2>&1 | grep "l7directord process is not running."`
if [ -z "$RET" ]
then
        echo "Test failed: $INIT_L7DIRECTORD reload"
        exit 1
fi


exit 0


