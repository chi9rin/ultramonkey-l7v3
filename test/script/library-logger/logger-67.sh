#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}
# set dummy log file
touch ${L7VS_LOG_DIR}/l7vs-access.log

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

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -L 1 -a /var/log/l7vs/l7vs-access.log --ac-rotate-type "date" --ac-rotate-max-backup-index "10" --ac-rotate-rotation-timing "date" --ac-rotate-rotation-timing-value "`date -d '1 minute' +'%H:%M'`"
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -L 1 -a /var/log/l7vs/l7vs-access.log --ac-rotate-type date --ac-rotate-max-backup-index 10 --ac-rotate-rotation-timing date --ac-rotate-rotation-timing-value `date -d '1 minute' +'%H:%M'`"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi

sleep 60
#Connect
for (( i = 0; i < 8; i++ )){
	RET=`wget -t 1 -qO- http://127.0.0.1:40001/`
	if [ "${RET}" != "${RealServer1}" ]
	then
	        echo "Test failed: wget -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
	fi
}

RET=`ls ${L7VS_LOG_DIR}/l7vs-access.log* | wc -l`
if [ $RET -ne 2 ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vs-access.log*"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vs-access.log`
EXPECT="${L7VS_LOG_DIR}/l7vs-access.log"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vs-access.log"
        exit 1
fi

RET=`ls ${L7VS_LOG_DIR}/l7vs-access.log.1`
EXPECT="${L7VS_LOG_DIR}/l7vs-access.log.1"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ls ${L7VS_LOG_DIR}/l7vs-access.log.1"
        exit 1
fi

exit 0

