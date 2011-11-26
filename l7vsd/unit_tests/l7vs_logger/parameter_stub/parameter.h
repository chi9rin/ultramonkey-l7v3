#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include <iostream>
#include <map>
#include "parameter_enum.h"
#include "parameter_impl.h"
#include "error_code.h"
#include "logger_enum.h"

//      publicメンバの「RetBool」「RetInt」「RetStr」は、デバッグ用に追加されたメンバです。
//      l7vs::Parameter::getInstance().RetBool = true;
//      l7vs::Parameter::getInstance().RetInt = 444;
//      l7vs::Parameter::getInstance().RetStr = "test string";
//      の様にすることで、任意の値を返すようになります。


namespace l7vs
{
    class ParameterImpl;
    class Parameter
    {
public:
    Parameter();                            //!< default constractor
    ~Parameter();                           //!< default destractor

    //! reload file function
    //! @param[in] parameter conponent tag
    //! @return true success file read
    //! @return false failure file read
    bool    read_file(const PARAMETER_COMPONENT_TAG);

    //! read specified configuration file function
    //! @param[in] parameter conponent tag
    //! @param[in] configuration filename
    //! @return true success file read
    //! @return false failure file read
    bool    read_specified_file(const PARAMETER_COMPONENT_TAG, const std::string& filename);

    //! parameter int value getter
    //! @param[in]  parametercategory
    //! @param[in]  parameter key
    //! @param[out] error code
    //! @return intvalue
    int     get_int(const PARAMETER_COMPONENT_TAG, const std::string&, error_code& );

    //! parameter string value getter
    //! @param[in]  parametercategory
    //! @param[in]  parameter key
    //! @param[out] error code
    //! @return string value
    std::string get_string(const PARAMETER_COMPONENT_TAG, const std::string&, error_code& );

    //! parameter multistring value getter
    //! @param[in]  parametercategory
    //! @param[in]  parameter key
    //! @param[inout] string map
    //! @param[out] error code
    //! @return string value
    std::string get_multistring(const PARAMETER_COMPONENT_TAG, const std::string&, std::multimap<std::string, std::string>&, error_code& );

    void    setIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key, const int value);
    void    setStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key, const std::string& value);

        void    deleteIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key);
        void    deleteStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key);


    };
};
    
#endif  //__PARAMETER_H__
