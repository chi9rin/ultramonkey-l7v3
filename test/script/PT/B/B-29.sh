#!/bin/bash

# set config file
PARM=`date -d '1 minute' +'%M'`

sed "s|l7vsadm_rotation_timing_value = \"XXX\"|l7vsadm_rotation_timing_value = \"$PARM\"|" materials/B-29-l7vs.cf > ${L7VSD_CONF_DIR}/l7vs.cf

# set dummy log file
touch ${L7VS_LOG_DIR}/l7vsadm.log

sleep 60

$L7VSADM
if [ $? -eq 0 ]
then
        echo "Test failed: $L7VSADM"
        exit 1
fi

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
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsadm.log.1"
        exit 1
fi

# set dummy log file
dd if=/dev/zero of=${L7VS_LOG_DIR}/l7vsadm.log bs=1024 count=51119

for (( i = 0; i < 10; i++ )){
        $L7VSADM
        if [ $? -eq 0 ]
        then
                echo "Test failed: $L7VSADM"
                exit 1
        fi
}

RET=`ls ${L7VS_LOG_DIR}/l7vsadm.log* | wc -l`
if [ $RET -ne 3 ]
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
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsadm.log.1"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vsadm.log.2`
EXPECT="${L7VS_LOG_DIR}/l7vsadm.log.2"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vsadm.log.2"
        exit 1
fi

exit 0

