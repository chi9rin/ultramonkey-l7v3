#!/bin/bash

. ${SET_DEFAULT_CONF}

# Run sorryserver (send in 5 sec)
PATH=$PATH:$COMMON_SCRIPT_DIR
test_server -p 20000 -t 5 ./materials/virtualservice-40-response.txt &

# temp file
tmp_file="/tmp/`uuidgen`.log"

cleanup() {
	killall test_server
	killall test_client2
	rm $tmp_file
}

#Add Service
start_l7vsd || exit 1

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b 127.0.0.1:20000
if [ $? -ne 0 ]; then
	cleanup
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b 127.0.0.1:20000"
        exit 1
fi
$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:30000 -w 0
if [ $? -ne 0 ]; then
	cleanup
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:30000 -w 0"
        exit 1
fi
$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:40000 -w 0
if [ $? -ne 0 ]; then
	cleanup
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:40000 -w 0"
        exit 1
fi

test_client2 -p 40001 > $tmp_file &

sleep 1

$L7VSADM -e -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:30000 -w 1
if [ $? -ne 0 ]; then
	cleanup
        echo "Test failed: $L7VSADM -e -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:30000 -w 1"
        exit 1
fi
$L7VSADM -e -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:40000 -w 1
if [ $? -ne 0 ]; then
	cleanup
        echo "Test failed: $L7VSADM -e -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:40000 -w 1"
        exit 1
fi

# sorry server connection must be left
ps aux | grep "test_client2" | grep -v grep > /dev/null 2>&1
if [ $? -ne 0 ]; then
	cleanup
	echo "Test failed: sorry server connection not found."
	exit 1
fi

sleep 5

diff $tmp_file ./materials/virtualservice-40-response.txt > /dev/null 2>&1
if [ $? -ne 0 ]; then
	cleanup
	echo "Test failed: Mismatch sorryserver response."
	exit 1
fi

cleanup
exit 0

