#!/bin/bash
. ${SET_DEFAULT_CONF}
# Set l7directorc config file
echo "virtual=127.0.0.1:50000" > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 2

$L7VSADM -C
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -C"
        exit 1
fi
usleep 100000

# Stop l7directord
$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi
sleep 2

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0303\] Failed to command ./usr/sbin/l7vsadm -D -t 127.0.0.1:50000 -m sessionless 2>&1' with return: 65280"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0203\] Failed to delete virtual service from l7vsd: .127.0.0.1:50000 sessionless ', output: .del vs error : virtual service not found.'"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

exit 0
