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

#Add 127 virtual service and 127 real server
for ((RSPORT=50001,VSPORT=40001; RSPORT<50128 || VSPORT<40128; RSPORT++,VSPORT++)); \
do $L7VSADM -A -t 127.0.0.1:$VSPORT -m sessionless; \
   $L7VSADM -a -t 127.0.0.1:$VSPORT -m sessionless -r 127.0.0.1:$RSPORT; done 
if [ $? -ne 0 ]
then
        echo "Test failed: for ((RSPORT=50001,VSPORT=40001; RSPORT<50128 || VSPORT<40128; RSPORT++,VSPORT++)); \
                           do $L7VSADM -A -t 127.0.0.1:$VSPORT -m sessionless; \
                              $L7VSADM -a -t 127.0.0.1:$VSPORT -m sessionless -r 127.0.0.1:$RSPORT; done"
	exit 1
fi

#Set SNMPAgent function enable and 0 into cache update interval
$L7VSADM -S -f 1 -i 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -f 1 -i 0"
	exit 1
fi

#l7vsVsTable count check
RET=`snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsTable | wc -l | awk '{print $1}'`

EXPECT="130"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsTable"
        exit 1
fi


#l7vsRsTable count check
RET=
RET=`snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable | wc -l | awk '{print $1}'`

EXPECT="130"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "snmptable -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsRsTable"
        exit 1
fi

pkill l7vsd 
usleep 1000000
exit 0
