#!/bin/bash

# set config file
cp materials/logger-80-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

if [ -d $TMP_DIR/l7vslog ]
then
        rm -rf $TMP_DIR/l7vslog
fi

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -L 1 -a ${TMP_DIR}/l7vslog/l7vs-access.log
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

#Connect
RET=`wget -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${RealServer1}" ]
then
        echo "Test failed: wget -t 1 -qO- http://127.0.0.1:40001/"
	exit 1
fi

RET=`ls ${TMP_DIR}/l7vslog/l7vs-access.log* | wc -l`
if [ $RET -ne 1 ]
then
        echo "Test failed: ls ${TMP_DIR}/l7vslog/l7vs-access.log*"
        exit 1
fi

RET=`ls ${TMP_DIR}/l7vslog/l7vs-access.log`
EXPECT="${TMP_DIR}/l7vslog/l7vs-access.log"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${TMP_DIR}/l7vslog/l7vs-access.log"
        exit 1
fi

exit 0

