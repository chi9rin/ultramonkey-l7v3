#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

VS1="127.0.0.1"
RS1="127.0.0.1"

#Start L7VSD Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi
usleep 100000

#enable snmpagent
$L7VSADM -S -f 1 -i 0
if [ $? -ne 0 ]
then
        echo "Test failed: L7VSADM -S -f 1 -i 0"
        exit 1
fi
usleep 100000
#Add Virtual Service
$L7VSADM -A -t $VS1:40001 -m sessionless -s rr 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS1:40001 -m sessionless -s rr"
        exit 1
fi

#Add Real Service
$L7VSADM -a -t $VS1:40001 -m sessionless -r $RS1:50001
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS1:40001 -m sessionless -r $RS1:50001"
        exit 1
fi

#Check Return Value
RET=`snmpget -v 2c -c public localhost ULTRAMONKEY-L7-MIB::l7vsVsAddr.1 2>&1`

usleep 100000
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsAddr.1 = STRING: \"$VS1\""

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpget -v 2c -c public localhost ULTRAMONKEY-L7-MIB::l7vsVsAddr.1"
        exit 1
fi

exit 0

