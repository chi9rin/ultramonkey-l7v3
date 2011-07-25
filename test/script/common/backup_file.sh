#!/bin/bash

# Backup config files and log files.
LOG "backup_file start."

if [ -e /etc/hosts ]
then
	if [ ! -d ${TMP_DIR}/etc ]
	then 
		mkdir -p ${TMP_DIR}/etc
	fi
	\cp -f /etc/hosts ${TMP_DIR}/etc
else
	false
fi
if [ $? -ne 0 ]
then
	LOG_ERR "Cannot move /etc/hosts file."
else
	LOG "/etc/hosts file was moved to ${TMP_DIR}/etc/hosts." 
fi

if [ -n ${L7VSD_CONF_DIR} -a ${L7VSD_CONF_DIR} != "/" ]
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
	LOG_ERR "Cannot move ${L7VSD_CONF_DIR}/* files."
else
	LOG "${L7VSD_CONF_DIR}/* files were moved to ${TMP_DIR}/l7vs directory." 
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
	LOG_ERR "Cannot move ${L7DIRECTORD_CONF_DIR}/* files."
else
	LOG "${L7DIRECTORD_CONF_DIR}/* files were moved to ${TMP_DIR}/l7director directory."
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
	LOG_ERR "Cannot move ${L7VS_LOG_DIR}/* files."
else
	LOG "${L7VS_LOG_DIR}/* files were moved to ${TMP_DIR}log directory."
fi

LOG "backup_file end."
