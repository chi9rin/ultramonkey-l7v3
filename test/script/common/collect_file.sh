#!/bin/bash

#Collect config files and log files.
LOG "collect_file start."

if [ -n ${L7VSD_CONF_DIR} -a ${L7VSD_CONF_DIR} != "/" ]
then
	mv ${L7VSD_CONF_DIR}/* ${TAR_DIR} 2> /dev/null
else
	false
fi
if [ $? -ne 0 ]
then
	LOG_ERR "l7vs config file move failed."
else
	LOG "l7vs config file moved ${TAR_DIR} ." 
fi


if [ -n ${L7DIRECTORD_CONF_DIR} -a ${L7DIRECTORD_CONF_DIR} != "/" ]
then
        mv ${L7DIRECTORD_CONF_DIR}/* ${TAR_DIR} 2> /dev/null
else
        false
fi
if [ $? -ne 0 ]
then
	LOG_ERR "l7director config file move failed."
else
	LOG "l7director config file moved ${TAR_DIR} ."
fi

if [ -n ${L7VS_LOG_DIR} -a ${L7VS_LOG_DIR} != "/" ]
then
        mv ${L7VS_LOG_DIR}/* ${TAR_DIR} 2> /dev/null
else
        false
fi
if [ $? -ne 0 ]
then
	LOG_ERR "UM-L7's log file move failed."
else
	LOG "UM-L7's log file moved ."
fi

LOG "collect_file end."
