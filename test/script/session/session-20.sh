#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

PATH=$PATH:$COMMON_SCRIPT_DIR

# start l7vsd
start_l7vsd || exit 1

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b 127.0.0.1:20000 -f 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b 127.0.0.1:20000 -f 0"
        exit 1
fi
$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:30000
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:30000"
        exit 1
fi

# temp file
tmp_file="/tmp/`uuidgen`.log"

cleanup() {
	killall test_server
	killall test_client2
	rm $tmp_file
}

# sorry server
test_server -p 20000 materials/session-20-sorryresponse.txt &

# real server
test_server -p 30000 -t 3 materials/session-20-realresponse.txt &

# send request
test_client2 -p 40001 > $tmp_file &

sleep 1

# enable sorry flag
$L7VSADM -E -t 127.0.0.1:40001 -m sessionless -b 127.0.0.1:20000 -f 1
if [ $? -ne 0 ]
then
	cleanup
        echo "Test failed: $L7VSADM -E -t 127.0.0.1:40001 -m sessionless -b 127.0.0.1:20000 -f 1"
        exit 1
fi

sleep 1

grep realserver $tmp_file > /dev/null 2>&1
if [ $? -ne 0 ]; then
	cleanup
	echo "Test failed: Not connected to realserver."
	exit 1
fi

grep sorryserver $tmp_file > /dev/null 2>&1
if [ $? -ne 0 ]; then
	cleanup
	echo "Test failed: Not connected to sorryserver."
	exit 1
fi

cleanup
exit 0
