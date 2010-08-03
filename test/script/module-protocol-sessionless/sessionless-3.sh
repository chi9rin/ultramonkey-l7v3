#!/bin/bash

. ${SET_DEFAULT_CONF}

#Run nc server
nc -i 1 -l 9876 > ${TMP_DIR}/nc_tmp &
NC_PID=$!
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

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:9876
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:9876"
        exit 1
fi

#Connect
echo "test" | nc 127.0.0.1 40001
wait $NC_PID

RET=`cat $TMP_DIR/nc_tmp`
if [ "${RET}" != "test" ]
then
        echo "Test failed: cat $TMP_DIR/nc_tmp"
        exit 1
fi

exit 0

