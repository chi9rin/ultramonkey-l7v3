#!/bin/bash

# set config file
cp materials/l7vsadm-1-2-8-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

TIME1=`date '+%s'`
RET=`flock -n -x $L7VSADM l7vsadm 2>&1`
TIME2=`date '+%s'`
 
EXPECT="COMMON ERROR : L7vsadm file lock timeout. (l7vsadm is already executing)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: flock -n -x $L7VSADM l7vsadm"
        exit 1
fi

if [ $((TIME1+5)) -ne $TIME2 ]
then
        echo "Test failed: flock -n -x $L7VSADM l7vsadm"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log | grep "\[WARN\] l7vsadm_common ADM33100001 Illegal cmd_interval parameter value. Use default value."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0

