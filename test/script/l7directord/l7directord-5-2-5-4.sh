#!/bin/bash
. ${SET_DEFAULT_CONF}

\cp -p ./materials/l7directord-5-2-5-execute.cf ${L7DIRECTORD_CONF_DIR}/execute.cf
sed "s|XXX|../../../../..${L7DIRECTORD_CONF_DIR}/execute.cf|g" ./materials/l7directord-5-2-5-l7directord.cf > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Run http server
FallbackServer=FallbackServer
FallbackServer_ADDR=127.0.0.1
FallbackServer_PORT=50001
start_lighttpd -s $FallbackServer -a $FallbackServer_ADDR -p $FallbackServer_PORT

if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd FallbackServer"
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
if [ "${RET}" != "${FallbackServer}" ]
then
        echo "Test failed: $WGET -t 1 -qO- http://127.0.0.1:40001/"
        exit 1
fi

exit 0

