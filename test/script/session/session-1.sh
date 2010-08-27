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
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -F"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:7
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r 127.0.0.1:7"
        exit 1
fi

# run echo server
chkconfig echo-stream on

#Connect
#RET=`cat materials/session-1-test.txt | nc 127.0.0.1 40001` # > $TMP_DIR/nc_expect_tmp
echo "hoge" | nc 127.0.0.1 40001 > $TMP_DIR/nc_expect_tmp


echo "$RET"
exit 1

if [ -n "`diff materials/session-1-test.txt $TMP_DIR/nc_expect_tmp`" ]
then
        echo "Test failed: diff materials/session-1-test.txt $TMP_DIR/nc_expect_tmp "
        exit 1
fi

exit 0

