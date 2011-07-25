#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

PATH=$PATH:$COMMON_SCRIPT_DIR

# start l7vsd
start_l7vsd || exit 1

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -F
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -F"
        exit 1
fi
$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001"
        exit 1
fi

# echo server
echo_server -p 50001 &

# send request
response=`test_client2 -p 40001 -s 1 materials/session-6-request.txt`
if [ "$response" != "`cat materials/session-6-response.txt`" ]; then
	echo "Test failed: response diff error"
	exit 1
fi
exit 0
