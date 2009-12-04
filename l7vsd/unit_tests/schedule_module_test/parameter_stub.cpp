#include "parameter_enum.h"
#include "parameter.h"
#include "parameter_impl.h"
#include "logger.h"

#if !defined(LOGGER_PROCESS_VSD) && !defined(LOGGER_PROCESS_ADM) && !defined(LOGGER_PROCESS_SNM)
#define LOGGER_PROCESS_VSD
#endif

#ifdef    LOGGER_PROCESS_SNM
l7vs::LOG_CATEGORY_TAG param_cat = l7vs::LOG_CAT_SNMPAGENT_PARAMETER;
#elif    LOGGER_PROCESS_ADM
l7vs::LOG_CATEGORY_TAG param_cat = l7vs::LOG_CAT_L7VSADM_PARAMETER;
#else
l7vs::LOG_CATEGORY_TAG param_cat = l7vs::LOG_CAT_L7VSD_PARAMETER;
#endif

/*!
 * Constructor of Parameter class
 */
l7vs::Parameter::Parameter(){
}

/*!
 * Destructor of Parameter class
 */
l7vs::Parameter::~Parameter(){
}

/*!
 * reload config file
 * @param[in]   comp    section TAG
 * @return    true = success read file / false = failure read file
 */
bool l7vs::Parameter::read_file(PARAMETER_COMPONENT_TAG comp){
    return true;
}


/*!
 * get integer data.
 * @param[in]    comp    section TAG
 * @param[in]    key    key string
 * @return        value
 */
int l7vs::Parameter::get_int(    const l7vs::PARAMETER_COMPONENT_TAG comp,
                                const std::string& key,
                                l7vs::error_code& err ){
    return 0;
}

/*!
 * get character data.
 * @param[in]    comp    section TAG
 * @param[in]    key    key string
 * @return    value
 */
std::string l7vs::Parameter::get_string(        const l7vs::PARAMETER_COMPONENT_TAG comp,
                                                const std::string& key,
                                                l7vs::error_code& err ){
    return key;
}
