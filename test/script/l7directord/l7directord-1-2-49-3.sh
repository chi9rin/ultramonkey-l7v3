#!/bin/bash
. ${SET_DEFAULT_CONF}

#test l7directord config file
RET=`$L7DIRECTORD --version | grep -A 1 "^l7directord, version [0-9]*\.[0-9]*\.[0-9]*" | sed -e 1d | grep "^Copyright (C) 20[0-9][0-9] NTT COMWARE CORPORATION"`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD --version"
        exit 1
fi

if [ -z "$RET" ]
then
        echo "Test failed: $L7DIRECTORD --version"
        exit 1
fi

exit 0

