#!/bin/bash

LOG_DIR="${LOG_BASE_DIR}/${DATE}"
LOG_FILE="${LOG_DIR}/${DATE}.log"


function LOG_ {
	echo "`date +'%Y/%m/%d %H:%M:%S'` [$1] $2 $3" >> ${LOG_FILE}
}
function LOG_FATAL {
        LOG_ FATAL "$1" "umtest stoped."
	echo "$1 umtest stoped."
}
function LOG_ERR {
        LOG_ ERROR "$1"
}
function LOG_WARN {
        LOG_ WARN "$1"
}
function LOG {
        LOG_ INFO "$1"
}


mkdir -p ${LOG_DIR}
if [ $? -eq 0 ]
then
	LOG "Make log directory ${LOG_DIR} ."
else
	LOG_FATAL "Can not make log directory."
fi


