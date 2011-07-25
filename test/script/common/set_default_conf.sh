#!/bin/bash

UML7_DEFAULT_CONF_DIR="${CONF_DIR}/ultramonkey-l7"

cp -bf ${UML7_DEFAULT_CONF_DIR}/l7directord.cf ${L7DIRECTORD_CONF_DIR} > /dev/null 2>&1
cp -bf ${UML7_DEFAULT_CONF_DIR}/l7vs.cf ${L7VSD_CONF_DIR} > /dev/null 2>&1
cp -bfr ${UML7_DEFAULT_CONF_DIR}/sslproxy ${L7VSD_CONF_DIR} > /dev/null 2>&1
