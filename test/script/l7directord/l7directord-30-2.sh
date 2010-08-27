#!/bin/bash
. ${SET_DEFAULT_CONF}

#test l7directord config file
RET=`$L7DIRECTORD -v`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD -v"
        exit 1
fi
EXPECT="l7directord, version 3.0.0-0
Copyright (C) 2009 NTT COMWARE CORPORATION"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD -v"
        exit 1
fi

exit 0

