#!/bin/bash

. ${SET_DEFAULT_CONF}

# start l7vsd
start_l7vsd || exit 1

$L7VSADM -A -t 127.0.0.1:44431 -m sslid
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:44431 -m sslid"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:44431 -m sslid -r 127.0.0.1:20000
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:44431 -m sslid -r 127.0.0.1:20000"
        exit 1
fi

PATH=$PATH:$COMMON_SCRIPT_DIR

# temp file
send_bin="/tmp/`uuidgen`.bin"
recv_hex="/tmp/`uuidgen`.hex"

cleanup() {
	killall hex_echo_server
	rm $send_bin
	rm $recv_hex
}

# hex echo server
hex_echo_server -p 20000 &

# convert hex to binary
cat materials/sslid-10-data.txt | hex2bin > $send_bin
ls -l $send_bin

# send request
test_client2 -p 44431 -s 1 $send_bin > $recv_hex

send_data=`perl -pe "s/\W//g" materials/sslid-10-data.txt`
recv_data=`perl -pe "s/\W//g" $recv_hex`

cleanup

if [ "$send_data" != "$recv_data" ]; then
	echo "Test failed: send and recv data are not matched."
	exit 1
fi

exit 0

