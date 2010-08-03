#!/bin/bash

. ${SET_DEFAULT_CONF}

STR=`for ((i = 0 ; i < 127; i++ )){
	echo -n "a"
}`

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000
RET=`$L7VSADM -A -t 127.0.0.1:40001 -m sessionless --sorry-uri /$STR 2>&1 | grep "PARSE ERROR"`
if [ "$RET" != "PARSE ERROR : protocol module argument error: '-S/--sorry-uri' option value '/$STR' is too long." ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless --sorry-uri /$STR"
        exit 1
fi

RET=`$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -S /$STR 2>&1 | grep "PARSE ERROR"`
if [ "$RET" != "PARSE ERROR : protocol module argument error: '-S/--sorry-uri' option value '/$STR' is too long." ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -S /$STR"
        exit 1
fi

exit 0

