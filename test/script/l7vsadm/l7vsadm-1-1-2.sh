#!/bin/bash

# set config file
cp materials/l7vsadm-1-1-2-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

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

if [ $((TIME1 + 4)) -ne $TIME2 ]
then
        echo "Test failed: flock -n -x $L7VSADM l7vsadm"
        exit 1
fi

exit 0

