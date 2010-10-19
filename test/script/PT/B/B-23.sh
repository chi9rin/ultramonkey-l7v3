#!/bin/bash

# set config file
cp materials/B-23-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

# set dummy log file
dd if=/dev/zero of=${L7VS_LOG_DIR}/l7vsadm.log bs=1024 count=51199

for (( i = 0; i < 5; i++ )){
	$L7VSADM
	if [ $? -eq 0 ]
	then
        	echo "Test failed: $L7VSADM"
	        exit 1
	fi
}

RET=`ls ${L7VS_LOG_DIR}/l7vsadm.log* | wc -l`
if [ $RET -ne 2 ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsadm.log*"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsadm.log`
EXPECT="${L7VS_LOG_DIR}/l7vsadm.log"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsadm.log.1`
EXPECT="${L7VS_LOG_DIR}/l7vsadm.log.1"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsadn.log.1"
        exit 1
fi

exit 0

