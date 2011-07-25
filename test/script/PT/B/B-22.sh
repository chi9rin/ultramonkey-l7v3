#!/bin/bash

# set config file
cp materials/B-22-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -eq 0 ]
then
	echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsadm_test.log`
if [ -z "$RET" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm_test.log"
        exit 1
fi

exit 0

