#!/bin/bash



if [ -n "`pgrep l7vsd`" ]
then
${INIT_L7VSD} stop
if [ $? -ne 0 ]
then
LOG_FATAL "Can not stop l7vsd."
exit 1
fi
fi

if [ -n "`pgrep l7directord`" ]
then
${INIT_L7DIRECTORD} stop > /dev/null
if [ -n "`pgrep l7directord`" ]
then
LOG_FATAL "Can not stop l7directord."
exit 1
fi
fi

LOG "stop_um_ps.sh ............OK"
