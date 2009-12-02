#include "parameter_enum.h"
#include "parameter.h"
#include "parameter_impl.h"
#include "logger.h"

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
	return true;
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
	int		value = 0;

	if ( 1000 <= get_int_stubmode ) {
		err.setter( true, "don't find key" );
	} else if ( "interval" == key && 1 != ( get_int_stubmode % 100 ) ){
		value = get_int_table[0];
	} else if ( "cmponent_size_00" == key && 2 != ( get_int_stubmode % 100 ) ){
		value = get_int_table[1];
	} else if ( "cmponent_size_01" == key && 3 != ( get_int_stubmode % 100 ) ){
		value = get_int_table[2];
	} else if ( "cmponent_size_02" == key && 4 != ( get_int_stubmode % 100 ) ){
		value = get_int_table[3];
	} else if ( "compulsorily_interval" == key && 5 != ( get_int_stubmode % 100 ) ){
		value = get_int_table[4];
	} else if ( 100 > get_int_stubmode ) {
		err.setter( true, "don't find key" );
	}

	return value;
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
	std::string	str( "" );


	if ( 1000 <= get_string_stubmode ) {
		err.setter( true, "don't find key" );
	} else if ( "ip_addr" == key && 1 != ( get_string_stubmode % 100 ) ){
		str = get_string_table[0];
	} else if ( "service_name" == key && 2 != ( get_string_stubmode % 100 ) ){
		str = get_string_table[1];
	} else if ( "nic" == key && 3 != ( get_string_stubmode % 100 ) ){
		str = get_string_table[2];
	} else if ( "cmponent_id_00" == key && 4 != ( get_string_stubmode % 100 ) ){
		str = get_string_table[3];
	} else if ( "cmponent_id_01" == key && 5!= ( get_string_stubmode % 100 ) ){
		str = get_string_table[4];
	} else if ( "cmponent_id_02" == key && 6 != ( get_string_stubmode % 100 ) ){
		str = get_string_table[5];
	} else if ( 100 > get_string_stubmode ) {
		err.setter( true, "don't find key" );
	}

	return str;
}
