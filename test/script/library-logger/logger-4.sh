#!/bin/bash

# set config file
cp materials/logger-4-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

RET=`$L7VSADM -A -t 127.0.0.1:40001 -m sessionless 2>&1`
EXPECT="add vs error : virtual service already exist."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log | grep "\[WARN\]" | wc -l`
if [ ${RET} -le 0 ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0

