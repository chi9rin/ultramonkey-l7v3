#ifndef PARAMETER_WRAPPER
#define PARAMETER_WRAPPER

#include "parameter.h"

inline int parameter_is_int_exist(const PARAMETER_COMPONENT_TAG comp, const char* key)
{
	if (l7vs::Parameter::getInstance().isIntExist(comp, key)) {
		return 1;
	}
	return 0;
}

inline int parameter_is_char_exist(const PARAMETER_COMPONENT_TAG comp, const char* key)
{
	if (l7vs::Parameter::getInstance().isStringExist(comp, key)) {
		return 1;
	}
	return 0;
}

inline int parameter_get_int_value(const PARAMETER_COMPONENT_TAG comp, const char* key)
{
	return l7vs::Parameter::getInstance().getIntValue(comp, key);
}

inline const char* parameter_get_char_value(const PARAMETER_COMPONENT_TAG comp, const char* key)
{
	return (l7vs::Parameter::getInstance().getStringValue(comp, key)).c_str();
}
inline void parameter_set_int_value(const PARAMETER_COMPONENT_TAG comp, const char* key, const int value)
{
	l7vs::Parameter::getInstance().setIntValue(comp, key, value);
}

inline void parameter_set_char_value(const PARAMETER_COMPONENT_TAG comp, const char* key, const char* value)
{
	l7vs::Parameter::getInstance().setStringValue(comp, key, value);
}

inline void parameter_delete_int_value(const PARAMETER_COMPONENT_TAG comp, const char* key)
{
	l7vs::Parameter::getInstance().deleteIntValue(comp, key);
}

inline void parameter_delete_char_value(const PARAMETER_COMPONENT_TAG comp, const char* key)
{
	l7vs::Parameter::getInstance().deleteStringValue(comp, key);
}
inline int parameter_reread_file(const PARAMETER_COMPONENT_TAG comp)
{
	if (l7vs::Parameter::getInstance().rereadFile(comp)) {
		return 0;
	}
	return -1;
}
inline void parameter_register_function_pointer(const PARAMETER_COMPONENT_TAG comp, void(*p_func)())
{
	l7vs::Parameter::getInstance().registerFunctionPointer(comp, p_func);
}


#endif	//PARAMETER_WRAPPER
