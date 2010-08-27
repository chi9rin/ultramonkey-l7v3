#!/bin/bash

. ${SET_DEFAULT_CONF}

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000
$L7VSADM -A -t 127.0.0.1:40001 -m ip #-F
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -F"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m ip -r 127.0.0.1:12321
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r 127.0.0.1:12321"
        exit 1
fi

#Run nc server
nc -i 1 -l 12321 > ${TMP_DIR}/test_receive.dat &
NC_PID=$!

#Connect
nc 127.0.0.1 40001 < materials/session-10-test.dat
wait $NC_PID


if [ -n "`cmp materials/session-10-test.dat $TMP_DIR/test_receive.dat 2>&1`" ]
then
        echo "Test failed: cmp materials/session-10-test.dat $TMP_DIR/test_receive.dat"
        exit 1
fi

exit 0

