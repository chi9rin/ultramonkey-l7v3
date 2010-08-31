#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-136-1-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Run http server
SorryServer=SorryServer
SorryServer_ADDR=127.0.0.1
SorryServer_PORT=50001
start_lighttpd -s $SorryServer -a $SorryServer_ADDR -p $SorryServer_PORT

if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer"
        exit 1
fi


#Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Start l7directord
$L7DIRECTORD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD start"
        exit 1
fi
sleep 1

#Connect
RET=`$WGET -t 1 -qO- http://127.0.0.1:40001/`
if [ "${RET}" != "${SorryServer}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

exit 0

