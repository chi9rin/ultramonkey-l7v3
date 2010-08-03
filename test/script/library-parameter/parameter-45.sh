#!/bin/bash

# set config file
cp materials/parameter-45-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

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
        echo "Test failed: ps -p ${PID} -o pid,user,args,class,schede"
        exit 1
fi

exit 0

