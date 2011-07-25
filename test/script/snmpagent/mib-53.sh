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

#add vs
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi
#add rs
$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 172.16.58.126:80 -w 50
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -b 172.16.58.126:80 -w 50"
        exit 1
fi
snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable 2>&1

#Check Return Value
RET=`snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable | awk 'NR==4 {print $6}'`
EXPECT="50"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable"
        exit 1
fi

#set cache update interval
$L7VSADM -S -i 5
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -i 5"
        exit 1
fi
#Edit rs:weight is changed
$L7VSADM -e -t 127.0.0.1:40001 -m sessionless -r 172.16.58.126:80 -w 100
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -e -t 127.0.0.1:40001 -m sessionless -r 172.16.58.126:80 -w 100"
        exit 1
fi

usleep 6000000

#result:weight is changed
RET=`snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable | awk 'NR==4 {print $6}'`
EXPECT="100"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable"
        exit 1
fi



exit 0
