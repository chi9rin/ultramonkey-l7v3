#!/bin/bash
. ${SET_DEFAULT_CONF}

#test l7directord config file
RET=`$L7DIRECTORD 2>&1`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD"
        exit 1
fi
EXPECT="Usage: l7directord {start|stop|restart|try-restart|reload|status|configtest}
Try \`l7directord --help' for more information."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD"
        exit 1
fi

exit 0

