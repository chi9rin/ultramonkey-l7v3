#!/bin/bash

# set config file
cp materials/logger-116-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

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

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -eq 0 ]
then
       	echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
       	exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsadm.log`
EXPECT="${L7VS_LOG_DIR}/l7vsadm.log"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0
