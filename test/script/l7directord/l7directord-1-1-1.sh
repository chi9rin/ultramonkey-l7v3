#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-1-1-1-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf


# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 1


RET=`grep -Ev "^\[" ${L7VS_LOG_DIR}/l7directord.log`
if [ -n "$RET" ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | wc -l`
if [ $RET -eq 0 ]
then
        echo "Test failed: wc -l ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

exit 0

