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
l7vs::Parameter::Parameter()
{
	if( !ParameterImpl::getInstance().init() ){
		LOGGER_PUT_LOG_FATAL( param_cat, 1, "Parameter initialize failure" );
		switch( param_cat ){
		case LOG_CAT_L7VSADM_PARAMETER:
			break;
		default:
//			throw 1;	//don't throw exception at singleton constructor...
			exit(1);
		}
	}
}

/*!
 * Destructor of Parameter class
 */
l7vs::Parameter::~Parameter()
{
}

/*!
 * reload config file
 * @param[in]   comp    section TAG
 * @return	true = success read file / false = failure read file
 */
bool
l7vs::Parameter::rereadFile(PARAMETER_COMPONENT_TAG comp)
{
	return ParameterImpl::getInstance().rereadFile(comp);
}

/*!
 * check whether integer data exists.
 * @param[in]	comp	section TAG
 * @param[in]	key	key string
 * @return	true = exist setting value / false = non exist setting value
 */
bool
l7vs::Parameter::isIntExist(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
	return ParameterImpl::getInstance().isIntExist(comp, key);
}

/*!
 * check whether character data exists.
 * @param[in]	comp	section TAG
 * @param[in]	key	key string
 * @return	true = exist setting value / false = non exist setting value
 */
bool
l7vs::Parameter::isStringExist(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
	return ParameterImpl::getInstance().isStringExist(comp, key);
}

/*!
 * get integer data.
 * @param[in]	comp	section TAG
 * @param[in]	key	key string
 * @return	value
 */
int
l7vs::Parameter::getIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
	return ParameterImpl::getInstance().getIntValue(comp, key);
}

/*!
 * get character data.
 * @param[in]	comp	section TAG
 * @param[in]	key	key string
 * @return	value
 */
std::string
l7vs::Parameter::getStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
	return ParameterImpl::getInstance().getStringValue(comp, key);
}

/*!
 * set-parameter function pointer relates component-tag
 * @param[in]	comp	section TAG
 * @param[in]	p_func	function pointer
 */
void
l7vs::Parameter::registerFunctionPointer(const PARAMETER_COMPONENT_TAG comp, void(*p_func)())
{
	ParameterImpl::getInstance().registerFunctionPointer(comp, p_func);
}

