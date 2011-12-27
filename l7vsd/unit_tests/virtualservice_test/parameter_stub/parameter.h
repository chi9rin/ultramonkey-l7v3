#ifndef PARAMETER_H
#define PARAMETER_H

#include <iostream>
#include <map>
#include "parameter_enum.h"
#include "./parameter_impl.h"
#include "error_code.h"
#include "logger_enum.h"
#include "logger.h"

//      publicメンバの「RetBool」「RetInt」「RetStr」は、デバッグ用に追加されたメンバです。
//      l7vs::Parameter::getInstance().RetBool = true;
//      l7vs::Parameter::getInstance().RetInt = 444;
//      l7vs::Parameter::getInstance().RetStr = "test string";
//      の様にすることで、任意の値を返すようになります。

#ifndef PARAMETER_FILE
#define PARAMETER_FILE "/etc/l7vs/l7vs.cf"
#endif //PARAMETER_FILE


namespace l7vs
{
class ParameterImpl;
class Parameter
{
public:
        Parameter();                            //!< default constractor
        ~Parameter();                           //!< default destractor

        //! initialize(file designation)
        //! @param[in]    component tag
        //! @param[in]    file name
        //! @return true success
        //! @return false failer
        bool init(const PARAMETER_COMPONENT_TAG, const std::string &);

        //! reload file function
        //! @param[in]    parameter conponent tag
        //! @param[in]    configuration filename
        //!                   default:PARAMETER_FILE
        //! @return true  success file read
        //! @return false failure file read
        bool read_file(const PARAMETER_COMPONENT_TAG,
                       const std::string& = PARAMETER_FILE);

        //! parameter int value getter
        //! @param[in]    parametercategory
        //! @param[in]    parameter key
        //! @param[out]   error code
        //! @param[in]    configuration filename
        //!                   default:PARAMETER_FILE
        //! @return int value
        int get_int(const PARAMETER_COMPONENT_TAG,
                    const std::string &,
                    error_code &,
                    const std::string& = PARAMETER_FILE);

        //! parameter string value getter
        //! @param[in]    parametercategory
        //! @param[in]    parameter key
        //! @param[out]   error code
        //! @param[in]    configuration filename
        //!                   default:PARAMETER_FILE
        //! @return string value
        std::string get_string(const PARAMETER_COMPONENT_TAG,
                               const std::string &,
                               error_code &,
                               const std::string& = PARAMETER_FILE);

        //! parameter multistring value getter
        //! @param[in]    parametercategory
        //! @param[in]    parameter key
        //! @param[inout] string vector
        //! @param[out]   error code
        //! @param[in]    configuration filename
        //!                   default:PARAMETER_FILE
        //! @return string value
        void get_multistring(const PARAMETER_COMPONENT_TAG,
                             const std::string &,
                             std::vector<std::string>&,
                             error_code &,
                             const std::string& = PARAMETER_FILE);


        void    setIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string &key, const int value);
        void    setStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string &key, const std::string &value);

        void    deleteIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string &key);
        void    deleteStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string &key);


};
};

#endif  //PARAMETER_H__
