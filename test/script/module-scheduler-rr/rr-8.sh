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
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -s rr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -s rr"
        exit 1
fi

#Connect
RET=`LANG=C $WGET -t 1 -O- http://127.0.0.1:40001/ 2>&1`
if [ $? -ne 1 ]
then
        echo "Test failed: LANG=C $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

RET=`echo "$RET" | grep "Connecting to .* connected."`
if [ -z "${RET}" ]
then
	echo"Test failed: echo $RET | grep Connecting to .* connected."
        exit 1
fi

exit 0

