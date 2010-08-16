#include "parameter_enum.h"
#include "parameter.h"
#include "parameter_impl.h"


#if !defined(LOGGER_PROCESS_VSD) && !defined(LOGGER_PROCESS_ADM) && !defined(LOGGER_PROCESS_SNM)
#define LOGGER_PROCESS_VSD
#endif

#ifdef	LOGGER_PROCESS_SNM
l7vs::LOG_CATEGORY_TAG param_cat = l7vs::LOG_CAT_SNMPAGENT_PARAMETER;
#elif	LOGGER_PROCESS_ADM
l7vs::LOG_CATEGORY_TAG param_cat = l7vs::LOG_CAT_L7VSADM_PARAMETER;
#else
l7vs::LOG_CATEGORY_TAG param_cat = l7vs::LOG_CAT_L7VSD_PARAMETER;
#endif

/*!
 * Constructor of Parameter class
 */
l7vs::Parameter::Parameter(){
	if( !ParameterImpl::get_instance().init() ){
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
bool l7vs::Parameter::read_file(PARAMETER_COMPONENT_TAG comp){
	ParameterImpl&	impl = ParameterImpl::get_instance();
	return impl.read_file( comp );
}

/*!
 * read specified configuration file function
 * @param[in]   comp		section TAG
 * @param[in]   filename	ssl configuration filename
 * @return	true = success read file / false = failure read file
 */
bool l7vs::Parameter::read_specified_file(PARAMETER_COMPONENT_TAG comp, const std::string& filename){
	ParameterImpl&	impl = ParameterImpl::get_instance();
	return impl.read_specified_file( comp, filename );
}

/*!
 * get integer data.
 * @param[in]	comp	section TAG
 * @param[in]	key	key string
 * @return		value
 */
int l7vs::Parameter::get_int(	const l7vs::PARAMETER_COMPONENT_TAG comp,
								const std::string& key,
								l7vs::error_code& err ){
	ParameterImpl&	impl = ParameterImpl::get_instance();
	return impl.get_int( comp, key, err );
}

/*!
 * get character data.
 * @param[in]	comp	section TAG
 * @param[in]	key	key string
 * @return	value
 */
std::string l7vs::Parameter::get_string(		const l7vs::PARAMETER_COMPONENT_TAG comp,
												const std::string& key,
												l7vs::error_code& err ){
	ParameterImpl&	impl = ParameterImpl::get_instance();
	return impl.get_string( comp, key, err );
}

/*!
 * get character map data.
 * @param[in]	comp	section TAG
 * @param[in]	key	key string
 * @param[inout]	retmap	string map
 * @param[out]	errr	error code
 * @return	value
 */
std::string l7vs::Parameter::get_multistring(		const l7vs::PARAMETER_COMPONENT_TAG comp,
												const std::string& key,
												std::multimap<std::string, std::string>& retmap,
												l7vs::error_code& err ){
	ParameterImpl&	impl = ParameterImpl::get_instance();
	return impl.get_multistring( comp, key, retmap, err );
}

void    l7vs::Parameter::setIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key, const int value)
{

	ParameterImpl&	impl = ParameterImpl::get_instance();
	return impl.setIntValue( comp, key, value );

}

void    l7vs::Parameter::setStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key, const std::string& value)
{
        ParameterImpl&  impl = ParameterImpl::get_instance();
        return impl.setStringValue( comp, key, value );

}

void    l7vs::Parameter::deleteIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
        ParameterImpl&  impl = ParameterImpl::get_instance();
        return impl.deleteIntValue( comp, key );

}


void    l7vs::Parameter::deleteStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
        ParameterImpl&  impl = ParameterImpl::get_instance();

        return impl.deleteStringValue( comp, key );
}



