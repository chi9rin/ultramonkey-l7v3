#include "parameter_enum.h"
#include "parameter.h"
#include "parameter_impl.h"
#include "logger_wrapper.h"

#if !defined(LOGGER_PROCESS_VSD) && !defined(LOGGER_PROCESS_ADM) && !defined(LOGGER_PROCESS_SNM)
#define LOGGER_PROCESS_VSD
#endif

#ifdef	LOGGER_PROCESS_SNM
LOG_CATEGORY_TAG param_cat = LOG_CAT_SNMPAGENT_PARAMETER;
#elif	LOGGER_PROCESS_ADM
LOG_CATEGORY_TAG param_cat = LOG_CAT_L7VSADM_PARAMETER;
#else
LOG_CATEGORY_TAG param_cat = LOG_CAT_L7VSD_PARAMETER;
#endif

/*!
 * Constructor of Parameter class
 */
l7vs::Parameter::Parameter(){
	if( !ParameterImpl::getInstance().init() ){
		LOGGER_PUT_LOG_FATAL( param_cat, 1, "Parameter initialize failure" );
		switch( param_cat ){
		case LOG_CAT_L7VSADM_PARAMETER:
			break;
		default:
			exit(1);
		}
	}
}

/*!
 * Destructor of Parameter class
 */
l7vs::Parameter::~Parameter(){
}

/*!
 * reload config file
 * @param[in]   comp    section TAG
 * @return	true = success read file / false = failure read file
 */
bool l7vs::Parameter::rereadFile(PARAMETER_COMPONENT_TAG comp){
	return ParameterImpl::getInstance().rereadFile(comp);
}


/*!
 * get integer data.
 * @param[in]	comp	section TAG
 * @param[in]	key	key string
 * @return		value
 */
int l7vs::Parameter::getIntValue(	const l7vs::PARAMETER_COMPONENT_TAG comp,
									const std::string& key,
									l7vs::parameter::error_code& err ){
	return ParameterImpl::getInstance().getIntValue(comp, key, err);
}

/*!
 * get character data.
 * @param[in]	comp	section TAG
 * @param[in]	key	key string
 * @return	value
 */
std::string l7vs::Parameter::getStringValue(	const l7vs::PARAMETER_COMPONENT_TAG comp,
												const std::string& key
												l7vs::parameter::error_code& err ){
	return ParameterImpl::getInstance().getStringValue(comp, key, err);
}
