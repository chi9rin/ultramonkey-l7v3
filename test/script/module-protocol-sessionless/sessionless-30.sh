#!/bin/bash

. ${SET_DEFAULT_CONF}

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000
RET=`$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -S /sorry12345 --sorry-uri /test 2>&1 | grep "PARSE ERROR"`
if [ "$RET" != "PARSE ERROR : protocol module argument error: Cannot set multiple option '-S/--sorry-uri'." ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -S /sorry12345 --sorry-uri /test"
        exit 1
fi

exit 0

