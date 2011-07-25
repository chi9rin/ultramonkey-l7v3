#!/bin/bash

. ${SET_DEFAULT_CONF}

#Run nc server
nc -i 1 -l 9876 > ${TMP_DIR}/nc_tmp &
NC_PID=$!

#chkconfig echo-stream on

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless --forwarded-for
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless --forwarded-for"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:9876
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:9876"
        exit 1
fi

#Connect
echo -e -n "POST / HTTP/1.1\r
Host: 127.0.0.1\r
Content-length: 10\r
X-Forwarded-For: proxy1, proxy2\r
\r
0123456789POST / HTTP/1.1\r
Host: 127.0.0.1\r
Content-length: 10\r
X-Forwarded-For: proxy1, proxy2\r
\r
0123456789" | nc 127.0.0.1 40001
wait $NC_PID

echo -e -n "POST / HTTP/1.1\r
Host: 127.0.0.1\r
Content-length: 10\r
X-Forwarded-For: proxy1, proxy2, 127.0.0.1\r
\r
0123456789POST / HTTP/1.1\r
Host: 127.0.0.1\r
Content-length: 10\r
X-Forwarded-For: proxy1, proxy2, 127.0.0.1\r
\r
0123456789" > $TMP_DIR/nc_expect_tmp

if [ -n "`diff $TMP_DIR/nc_tmp $TMP_DIR/nc_expect_tmp`" ]
then
	echo "Test failed: diff $TMP_DIR/nc_tmp $TMP_DIR/nc_expect_tmp"
        exit 1
fi
exit 0

