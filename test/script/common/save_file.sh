#!/bin/bash

#Save configfiles and logfiles.
LOG "save_file start."
if [ -n ${L7VSD_CONF_DIR} -a ${L7VSD_CONF_DIR} != "/"  ]
then
	if [ ! -d ${TMP_DIR}/l7vs ]
	then
	        mkdir -p ${TMP_DIR}/l7vs
	fi
	mv ${L7VSD_CONF_DIR}/* ${TMP_DIR}/l7vs/ 2> /dev/null
else
        false
fi
if [ $? -ne 0 ]
then
	LOG_ERR "l7vs config file move failed."
else
	LOG "l7vs config file moved ${TMP_DIR}/l7vs ." 
fi


if [ -n ${L7DIRECTORD_CONF_DIR} -a ${L7DIRECTORD_CONF_DIR} != "/" ]
then
	if [ ! -d ${TMP_DIR}/l7director ]
	then
	        mkdir -p ${TMP_DIR}/l7director
	fi
	mv ${L7DIRECTORD_CONF_DIR}/* ${TMP_DIR}/l7director 2> /dev/null
else
        false
fi
if [ $? -ne 0 ]
then
	LOG_ERR "l7director config file move failed."
else
	LOG "l7director config file moved ${TMP_DIR}/l7direcotr ."
fi


if [ -n ${L7VS_LOG_DIR} -a ${L7VS_LOG_DIR} != "/" ]
then
	if [ ! -d ${TMP_DIR}/log ]
	then
	        mkdir -p ${TMP_DIR}/log
	fi
	mv ${L7VS_LOG_DIR}/* ${TMP_DIR}/log 2> /dev/null
else
        false
fi
if [ $? -ne 0 ]
then
	LOG_ERR "UM-L7's log file move failed."
else
	LOG "UM-L7's log file moved ${TMP_DIR}log ."
fi

LOG "save_file end."
