#!/bin/bash

# set config file
cp materials/parameter-41-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

PID=`pgrep l7vsd`
RET=`ps -p ${PID} -o pid,user,args,class,sched | awk 'NR==2 {print $4,$5}'`
EXPECT="TS 0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: ps -p ${PID} -o pid,user,args,class,sched"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "\[WARN\] l7vsd_mainthread VSD30600006 task scheduler priority parameter not found."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

exit 0

