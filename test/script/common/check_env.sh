# UltraMonkey-L7がインストールされているか確認
#!/bin/bash

if [ ! -e ${L7VSD} ]
then
LOG_FATAL "${L7VSD} not exist. "
exit 1
fi

if [ ! -e ${L7VSADM} ]
then
LOG_FATAL "${L7VSADM} not exist."
exit 1
fi

if [ ! -e ${L7DIRECTORD} ]
then
LOG_FATAL "${L7DIRECTORD} not exist."
exit 1
fi

if [ ! -e ${INIT_L7VSD} ]
then
LOG_FATAL "${INIT_L7VSD} not exist. "
exit 1
fi

if [ ! -e ${INIT_L7DIRECTORD} ]
then
LOG_FATAL "${INIT_L7DIRECTORD} not exist."
exit 1
fi

if [ ! -d ${L7VSD_CONF_DIR} ]
then
LOG_WARN "${L7VSD_CONF_DIR} not exist. Create ${L7VSD_CONF_DIR} ."
mkdir ${L7VSD_CONF_DIR}
fi

if [ ! -d ${L7DIRECTORD_CONF_DIR} ]
then
LOG_WARM "${L7DIRECTORD_CONF_DIR} not exist. Create ${L7DIRECTORD_CONF_DIR} ."
mkdir ${L7DIRECTORD_CONF_DIR}
fi

if [ ! -d ${L7VS_LOG_DIR} ]
then
LOG_WARN "${L7VS_LOG_DIR} not exist. Create ${L7VS_LOG_DIR} ."
mkdir ${L7VS_LOG_DIR}
fi

LOG "check_env.sh ........OK"
