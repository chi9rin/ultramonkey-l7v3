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

#Add a virtual service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
	exit 1
fi

#Add 127 real server
for ((PORT=50001; PORT<50128; PORT++)); do $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:$PORT; done 
if [ $? -ne 0 ]
then
        echo "for ((PORT=50001; PORT<50128; PORT++)); 
	      do $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:$PORT; done"
	exit 1
fi

#Set SNMPAgent function enable and 0 into cache update interval
$L7VSADM -S -f 1 -i 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1 -i 0"
	exit 1
fi

#l7vsRsTable empty check
RET=`snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable | wc -l | awk '{print $1}'`

EXPECT="130"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable"
        exit 1
fi

exit 0
