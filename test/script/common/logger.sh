#!/bin/bash

function LOG_ {
echo "`date +'%Y/%m/%d %H:%M:%S'` [$1] $2 $3" >> ${LOG_FILE}
}
function LOG_FATAL {
LOG_ FATAL "$1" "Test stoped."
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


if [ ! -e ${LOG_DIR} ]
then
mkdir -p ${LOG_DIR}
fi

