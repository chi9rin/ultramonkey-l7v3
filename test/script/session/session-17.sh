#!/bin/bash


# set config file
. ${SET_DEFAULT_CONF}
\cp -f materials/session-17-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

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

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001"
        exit 1
fi

nc -l 50001 &
RSPID=$!

$TEST_CLIENT 127.0.0.1 40001 &
CLPID=$!
usleep 100000
kill -KILL $RSPID
usleep 100000

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "active_session.size =" | awk '{print $6,$7,$8}'`
EXPECT="active_session.size = 0
active_session.size = 1
active_session.size = 1
active_session.size = 0
active_session.size = 0"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

kill -KILL $CLPID

exit 0

