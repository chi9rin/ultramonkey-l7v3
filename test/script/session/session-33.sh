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

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b 127.0.0.1:9999 -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b 127.0.0.1:9999 -f 1"
        exit 1
fi

#Connect
RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`

if [ $? -eq 0 ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

if [ -n "${RET}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

exit 0


