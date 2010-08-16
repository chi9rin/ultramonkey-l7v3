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
$L7VSADM -A -t [::1]:40001 -m sessionless
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -A -t [::1]:40001 -m sessionless"
        exit 1
fi

$L7VSADM -a -t [::1]:40001 -m sessionless -r 127.0.0.1:50001
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -a -t [::1]:40001 -m sessionless -r 127.0.0.1:50001"
        exit 1
fi

RET=`wget -t 1 -qO- http://[::1]:40001/`
if [ $? -eq 0 ]
then
        echo "Test failed: wget -t 1 -qO- http://[::1]:40001/"
        exit 1
fi

if [ -n "${RET}" ]
then
        echo "Test failed: wget -t 1 -qO- http://[::1]:40001/"
        exit 1
fi

exit 0

