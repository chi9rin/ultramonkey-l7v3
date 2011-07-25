#!/bin/bash

# set config file
cp materials/parameter-2-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

RET=`flock -n -x $L7VSADM l7vsadm 2>&1`
EXPECT="COMMON ERROR: L7vsadm file lock timeout. (l7vsadm is already executing)"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: flock -n -x /usr/sbin/l7vsadm l7vsadm"
        exit 1
fi

(time flock -n -x $L7VSADM l7vsadm) 2> $TMP_DIR/parameter-2.tmp
RET=`cat ${L7VS_LOG_DIR}/l7vsadm.log | grep "L7vsadm file lock error." | wc -l`

# cmd_count = 5 but do "flock -n -x $L7VSADM l7vsadm" twice. 5 + 5 = 10.

if [ "${RET}" != "10" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsadm.log"
        exit 1
fi

exit 0

