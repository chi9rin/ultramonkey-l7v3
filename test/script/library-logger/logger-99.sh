#!/bin/bash

# set config file
cp materials/logger-99-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

# set dumy log file
dd if=/dev/zero of=${L7VS_LOG_DIR}/l7vsd.log bs=1024 count=63
dd if=/dev/zero of=${L7VS_LOG_DIR}/l7vsd.log.1 bs=1024 count=63
dd if=/dev/zero of=${L7VS_LOG_DIR}/l7vsd.log.1 bs=1024 count=63

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

for (( i = 0; i < 9; i++ )){
	$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
	if [ $? -eq 0 ]
	then
        	echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        	exit 1
	fi
}

RET=`ls ${L7VS_LOG_DIR}/l7vsd.log* | wc -l`
if [ $RET -ne 3 ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsd.log*"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsd.log`
EXPECT="${L7VS_LOG_DIR}/l7vsd.log"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsd.log.1`
EXPECT="${L7VS_LOG_DIR}/l7vsd.log.1"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsd.log.1"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsd.log.2`
EXPECT="${L7VS_LOG_DIR}/l7vsd.log.2"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsd.log.2"
        exit 1
fi

exit 0

