#!/bin/bash

# Restore config files and log files.
LOG "restore_file start."

\mv -f ${TMP_DIR}/etc/hosts /etc/hosts 2> /dev/null
if [ $? -ne 0 ]
then
	LOG_ERR "Cannot move ${TMP_DIR}/etc/hosts file."
else
	LOG "/etc/hosts file was restored."
fi	

mv ${TMP_DIR}/l7vs/* ${L7VSD_CONF_DIR} 2> /dev/null
if [ $? -ne 0 ]
then
	LOG_ERR "Cannot move ${TMP_DIR}/l7vs/* files."
else
	LOG "${L7VSD_CONF_DIR}/* files were restored."
fi

mv ${TMP_DIR}/l7director/* ${L7DIRECTORD_CONF_DIR} 2> /dev/null
if [ $? -ne 0 ]
then
	LOG_ERR "Cannot move ${TMP_DIR}/l7director/* files."
else
	LOG "${L7DIRECTORD_CONF_DIR}/* files were restored."
fi

mv ${TMP_DIR}/log/* ${L7VS_LOG_DIR} 2> /dev/null
if [ $? -ne 0 ]
then
	LOG_ERR "Cannot move ${TMP_DIR}/log/* files."
else
	LOG "${L7VS_LOG_DIR}/* files were restored."
fi

LOG "restore_file end."
