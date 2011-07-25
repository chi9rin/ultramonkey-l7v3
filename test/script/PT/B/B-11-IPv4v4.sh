#!/bin/bash

VS1="127.0.0.1"
RS1="127.0.0.1"

# set config file
. ${SET_DEFAULT_CONF}

#Start L7VSD Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Print The Status of Virtual Service 
$L7VSADM -l
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi


#Add Virtual Service
$L7VSADM -A -t $VS1:8080 -m sessionless -s rr
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS1:8080 -m sessionless -s rr"
        exit 1
fi


#Add Real Service
$L7VSADM -a -t $VS1:8080 -m sessionless -r $RS1:80
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS1:8080 -m sessionless -r $RS1:80"
        exit 1
fi

#Check The Status of Virtual Service 
RET=`$L7VSADM -l`
EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:webcache sessionless rr
  -> localhost:http               Masq    1      0          0         "

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi

#Change The Weight of Real Service
$L7VSADM -e -t $VS1:8080 -m sessionless -r $RS1:80 -w 100
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi

#Print The Status of Virtual Service  And Check The Weight of Real Service
RET=`$L7VSADM -l`

EXPECT="Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:webcache sessionless rr
  -> localhost:http               Masq    100    0          0         "

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -l"
        exit 1
fi


exit 0

