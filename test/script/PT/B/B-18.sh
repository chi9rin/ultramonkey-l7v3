#!/bin/bash

# set config file
PARM=`LANG=en date -d '1 minute' +'%a %H:%M' | tr A-Z a-z`

sed "s|l7vsd_rotation_timing_value = \"XXX\"|l7vsd_rotation_timing_value = \"$PARM\"|" materials/B-18-l7vs.cf > ${L7VSD_CONF_DIR}/l7vs.cf

# set dummy log file
touch ${L7VS_LOG_DIR}/l7vsd.log

#Add Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi

$INIT_L7VSD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD stop"
        exit 1
fi

#Add Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi
usleep 100000

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

sleep 60

for (( i = 0; i < 9; i++ )){
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

