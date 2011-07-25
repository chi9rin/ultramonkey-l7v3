#!/bin/bash

# set config file
cp materials/B-14-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi
usleep 100000

# Out put log
$L7VSADM -A -t 127.0.0.1:50000 -m sessionless
$L7VSADM -A -t 127.0.0.1:50000 -m sessionless

RET=`cat ${L7VS_LOG_DIR}/l7vsd_test.log`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd_TEST.log"
        exit 1
fi

exit 0

