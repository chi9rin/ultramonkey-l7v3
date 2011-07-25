#!/bin/bash

# set config file
cp materials/l7vsd-15-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

PID=`pgrep l7vsd`
RET=`ps -p $PID -o pid,user,args,class,sched,rtprio | grep l7vsd | awk '{print $5}'`
if [ $RET -ne 0 ]
then
        echo "Test failed: ps -p $PID -o pid,user,args,class,sched,rtprio"
        exit 1
fi

exit 0

