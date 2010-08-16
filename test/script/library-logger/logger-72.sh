#!/bin/bash

# set config file
sed "s|l7vsd_log_filename = \"XXX\"|l7vsd_log_filename = \"$TMP_DIR/l7vslog/l7vsd.log\"|" materials/logger-72-l7vs.cf > ${L7VSD_CONF_DIR}/l7vs.cf

if [ -d $TMP_DIR/l7vslog ]
then
	rm -rf $TMP_DIR/l7vslog
fi

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

RET=`ls ${TMP_DIR}/l7vslog/l7vsd.log* | wc -l`
if [ $RET -ne 1 ]
then
        echo "Test failed: ls ${TMP_DIR}/l7vslog/l7vsd.log*"
        exit 1
fi

RET=`ls ${TMP_DIR}/l7vslog/l7vsd.log`
EXPECT="${TMP_DIR}/l7vslog/l7vsd.log"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${TMP_DIR}/l7vslog/l7vsd.log"
        exit 1
fi

exit 0

