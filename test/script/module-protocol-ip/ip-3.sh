#!/bin/bash
. ${SET_DEFAULT_CONF}

# Run server
PATH=$PATH:$COMMON_SCRIPT_DIR
test_server -p 20000 ./materials/ip-3.txt &

# Start l7vsd
start_l7vsd
if [ $? -ne 0 ]; then
	pkill test_server
        exit 1
fi

vs="127.0.0.1:10000"
real="127.0.0.1:20000"

$L7VSADM -A -t $vs -m ip
if [ $? -ne 0 ]; then
	pkill test_server
        echo "Test failed: Cannot add ip service."
        exit 1
fi
$L7VSADM -a -t $vs -m ip -r $real
if [ $? -ne 0 ]; then
	pkill test_server
        echo "Test failed: Cannot add real server."
        exit 1
fi

# tmp file
tmp_file="/tmp/`uuidgen`.log"
wget -q -O - http://$vs/ > $tmp_file
if [ $? -ne 0 ]; then
	pkill test_server
	rm $tmp_file
        echo "Test failed: wget error."
        exit 1
fi

diff_result=`diff $tmp_file ./materials/ip-3.txt`
if [ $? -ne 0 ]; then
	pkill test_server
	rm $tmp_file
        echo "Test failed: contents was not matched"
        exit 1
fi

pkill test_server
rm $tmp_file
exit 0
