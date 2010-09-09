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

# edit config file
echo "logfile = ${TMP_DIR}/l7directord.log" > ${L7DIRECTORD_CONF_DIR}/specified.cf

#Restart l7directord
$L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf reload
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD ${L7DIRECTORD_CONF_DIR}/specified.cf reload"
        exit 1
fi
usleep 100000

RET=`ls ${TMP_DIR}/l7directord.log`
if [ -z "$RET" ]
then
        echo "Test failed: ls ${TMP_DIR}/l7directord.log"
        exit 1
fi

exit 0

