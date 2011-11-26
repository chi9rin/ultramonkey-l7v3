#ifndef __PARAMETER_IMPL_H__
#define __PARAMETER_IMPL_H__

#include "parameter_enum.h"
#include <iostream>
#include <string>
#include <map>
#include <boost/program_options.hpp>
#include "error_code.h"
#include "logger_enum.h"

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>


namespace l7vs
{
    struct  component{
        std::string section;
        void        (*function)();
        component(){ section = ""; function = NULL;}
        component( std::string str, void (*p_func)() ){ section = str; function = p_func;}
        component& operator=(const component& in_comp){ section = in_comp.section; function = in_comp.function;return *this;}
    };
    class ParameterImpl
    {
protected:
    typedef std::map< std::string, int >            int_map_type;
    typedef std::map< std::string, std::string >        string_map_type;
    typedef std::multimap< std::string, std::string >   multistring_map_type;

    //! parameter data of string
    string_map_type                         stringMap;
    //! parameter data of multi string
    multistring_map_type                        multistringMap;
    //! parameter data of int
    int_map_type                            intMap;
    //! create mutex
    boost::mutex                            create_mutex;
    //! read / write mutex
    boost::mutex                            param_mutex;
    //! componenttag to section name map
    std::map< PARAMETER_COMPONENT_TAG, std::string >
                                            tag_section_table_map;
    //! constractor
    ParameterImpl(){}
    //! destractor
    ~ParameterImpl(){}
public:
    //! instansgetter
    //! @return instance
    static ParameterImpl & get_instance(){
        static ParameterImpl instance;
        return instance;
    }
    //! initialize
    //! @return true success
    //! @return false failer
    bool    init();

    //! target component read
    //! @param[in]  component tag
    //! @return false failer
    //! @return true success
    bool    read_file( const PARAMETER_COMPONENT_TAG );

    //! target component read specified filename
    //! @param[in]  component tag
    //! @param[in]  file name
    //! @return false failer
    //! @return true success
    bool    read_specified_file( const PARAMETER_COMPONENT_TAG, const std::string& filenamez );

    //! int value getter
    //! @param[in]  component tag
    //! @param[in]  keystring
    //! @param[out] errorcode
    //! @return     int value
    int get_int( const PARAMETER_COMPONENT_TAG, const std::string&, error_code& );

    //! string value getter
    //! @param[in]  component tag
    //! @param[in]  keystring
    //! @param[out] errorcode
    //! @return     string value
    std::string get_string( const PARAMETER_COMPONENT_TAG, const std::string&, error_code& );

    //! multistring value getter
    //! @param[in]  component tag
    //! @param[in]  keystring
    //! @param[inout]   multistring map
    //! @param[out] errorcode
    //! @return last foundstring value
    std::string get_multistring( const PARAMETER_COMPONENT_TAG, const std::string&, multistring_map_type&, error_code& );

    void    setIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key, const int value);

    void    setStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key, const std::string& value);

        void    deleteIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key);
        void    deleteStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key);


    };
};
#endif  //__PARAMETER_IMPL_H__
