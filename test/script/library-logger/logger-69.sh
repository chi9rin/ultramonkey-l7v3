#!/bin/bash

# set config file
PARM=`date -d '1 minute' +'%m/%d %H:%M'`

sed "s|l7vsd_rotation_timing_value = \"XXX\"|l7vsd_rotation_timing_value = \"$PARM\"|" materials/logger-69-l7vs.cf > ${L7VSD_CONF_DIR}/l7vs.cf

# set dumy log file
dd if=/dev/zero of=${L7VS_LOG_DIR}/l7vsd.log bs=1024 count=63

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

for (( i = 0; i < 10; i++ )){
        $L7VSADM -A -t 127.0.0.1:40001 -m sessionless
        if [ $? -eq 0 ]
        then
                echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
                exit 1
        fi
}

RET=`ls ${L7VS_LOG_DIR}/l7vsd.log* | wc -l`
if [ $RET -ne 2 ]
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

exit 0

