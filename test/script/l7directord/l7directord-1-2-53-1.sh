#!/bin/bash
. ${SET_DEFAULT_CONF}

#test l7directord config file
RET=`$L7DIRECTORD -f start 2>&1`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7DIRECTORD -f start"
        exit 1
fi
EXPECT="Unknown option: f
Usage: l7directord {start|stop|restart|try-restart|reload|status|configtest}
Try \`l7directord --help' for more information."
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD -f start"
        exit 1
fi

exit 0

