#!/bin/bash

# set config file
cp materials/logger-146-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Add Service
$L7VSD
if [ $? -eq 0 ]
then
	echo "Test failed: $L7VSD"
        exit 1
fi

exit 0
