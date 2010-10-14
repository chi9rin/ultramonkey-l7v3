#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}


#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -S -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1"
        exit 1
fi



#set cache update interval
$L7VSADM -S -i 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -i 0"
        exit 1
fi

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b 172.16.58.126:80
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b 172.16.58.126:80"
        exit 1
fi

usleep 1000000

#Check Return Value
RET=`snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsTable | awk 'NR==4 {print $8}'`
EXPECT="\"172.16.58.126\""
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsTable"
        exit 1
fi

#set cache update interval
$L7VSADM -S -i 5
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -i 5"
        exit 1
fi
#Edit Service:sorry server is changed
$L7VSADM -E -t 127.0.0.1:40001 -m sessionless -b 172.16.58.125:80
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

usleep 6000000

#result:sorry server is changed
RET=`snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsTable | awk 'NR==4 {print $8}'`
EXPECT="\"172.16.58.125\""

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsTable"
        exit 1
fi

exit 0
