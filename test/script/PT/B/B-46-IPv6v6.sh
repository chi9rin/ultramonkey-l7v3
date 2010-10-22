#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

VS1="[::1]"
RS1="[::1]"

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
usleep 1000000
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

usleep 100000
#Check Return Value
RET=`snmpget -v 2c -c public localhost ULTRAMONKEY-L7-MIB::l7vsRsAddr.1`
echo "$RET"
EXPECT="ULTRAMONKEY-L7-MIB::l7vsRsAddr.1 = STRING: \"::1\""
echo "$EXPECT"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpget -v 2c -c public localhost ULTRAMONKEY-L7-MIB::l7vsRsAddr.1"
        exit 1
fi

exit 0

