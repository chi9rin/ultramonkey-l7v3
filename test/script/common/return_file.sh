#!/bin/bash

#Return configfiles and logfiles.
LOG "return_file start."

mv ${TMP_DIR}/l7vs/* ${L7VSD_CONF_DIR} 2> /dev/null
if [ $? -ne 0 ]
then
	LOG_ERR "l7vs config file move failed."
else
        LOG "l7vs config file return ${L7VSD_CONF_DIR} ."
fi

mv ${TMP_DIR}/l7director/* ${L7DIRECTORD_CONF_DIR} 2> /dev/null
if [ $? -ne 0 ]
then
	LOG_ERR "l7director config file move failed."
else
        LOG "l7director config file return ${L7DIRECTORD_CONF_DIR} ."
fi

mv ${TMP_DIR}/log/* ${L7VS_LOG_DIR} 2> /dev/null
if [ $? -ne 0 ]
then
	LOG_ERR "UM-L7's log file move failed."
else
        LOG "UM-L7's log file return ${L7VS_LOG_DIR} ."
fi

LOG "return_file end."
