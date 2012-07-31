#include <vector>
#include <fstream>
#include <stdexcept>
#include "parameter_impl.h"
#include "logger_enum.h"
#include "parameter_enum.h"

//#include "lexical_cast.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/format.hpp>



#define LINE_LENGTH     4096


#ifndef PARAMETER_FILE
#define PARAMETER_FILE "/etc/l7vs/l7vs.cf"
#endif //PARAMETER_FILE

#if !defined(LOGGER_PROCESS_VSD) && !defined(LOGGER_PROCESS_ADM) && !defined(LOGGER_PROCESS_SNM)
#define LOGGER_PROCESS_VSD
#endif

#ifdef  LOGGER_PROCESS_SNM
l7vs::LOG_CATEGORY_TAG logcat = l7vs::LOG_CAT_SNMPAGENT_PARAMETER;
#elif   LOGGER_PROCESS_ADM
l7vs::LOG_CATEGORY_TAG logcat = l7vs::LOG_CAT_L7VSADM_PARAMETER;
#else
l7vs::LOG_CATEGORY_TAG logcat = l7vs::LOG_CAT_L7VSD_PARAMETER;
#endif

static bool     create_map_flag = false;

//
//! Initialize ParameterImpl class
//!     @return true    success
//! @return false       failer
bool    l7vs::ParameterImpl::init()
{
        boost::mutex::scoped_lock       lock(create_mutex);
        if (create_map_flag) return true;

        tag_section_table_map.clear();
        stringMap.clear();
        intMap.clear();
        tag_section_table_map[PARAM_COMP_L7VSD]                 = "l7vsd";
        tag_section_table_map[PARAM_COMP_COMMAND]               = "command";
        tag_section_table_map[PARAM_COMP_SESSION]               = "session";
        tag_section_table_map[PARAM_COMP_VIRTUALSERVICE] = "virtualservice";
        tag_section_table_map[PARAM_COMP_MODULE]                = "module";
        tag_section_table_map[PARAM_COMP_REPLICATION]   = "replication";
        tag_section_table_map[PARAM_COMP_LOGGER]                = "logger";
        tag_section_table_map[PARAM_COMP_L7VSADM]               = "l7vsadm";
        tag_section_table_map[PARAM_COMP_SNMPAGENT]             = "snmpagent";
        tag_section_table_map[PARAM_COMP_SSL]                   = "ssl";
        create_map_flag = read_file(PARAM_COMP_ALL);
        return  create_map_flag;
}

//! read config file
//! @param[in]  COMPONENT TAG
//! @return true read success
//! @return false read false
bool    l7vs::ParameterImpl::read_file(const l7vs::PARAMETER_COMPONENT_TAG comp)
{
        return read_specified_file(comp, PARAMETER_FILE);
}

//! read specified config file
//! @param[in]  COMPONENT TAG
//! @param[in]  file name
//! @return true read success
//! @return false read false
bool    l7vs::ParameterImpl::read_specified_file(const l7vs::PARAMETER_COMPONENT_TAG comp, const std::string &filename)
{


        typedef std::vector< std::string >                              split_vector_type;
        typedef std::pair< std::string, int >                   int_pair_type;
        typedef std::pair< std::string, std::string >   string_pair_type;

        boost::mutex::scoped_lock       lock(param_mutex);
        std::string             line;
        std::ifstream   ifs(filename.c_str());
        string_map_type string_map;
        multistring_map_type    multistring_map;
        int_map_type    int_map;

        if (!ifs) {
                std::stringstream buf;
                std::cout << "CONFIG FILE NOT OPEN : " << filename << "\n";
                //Logger::putLogFatal( logcat, 2, buf.str() , __FILE__, __LINE__ );
                return false;   // don't open config files.
        }

        std::string     section_string;
        while (std::getline(ifs, line)) {
                boost::algorithm::trim(line);            // triming
                if (line.size() == 0) continue;  // zero line skip
                if (line[0] == '#') continue;            // comment line skip
                std::string::size_type  pos = line.find('#');    // comment is clear
                if (pos != std::string::npos) line = line.substr(0, pos);

                split_vector_type split_vec;
                boost::algorithm::split(split_vec, line, boost::algorithm::is_any_of("="));
                if (split_vec.size() == 1) { // section
                        if (split_vec[0].at(0) == '[' && split_vec[0].at(split_vec[0].size() - 1) == ']')
                                section_string = split_vec[0].substr(1, split_vec[0].size() - 2);
                        else {
                                boost::format   formatter("section tag false : %1%");
                                formatter % split_vec[0];
                                std::cout << formatter.str() << "\n";
                                //Logger::putLogFatal( logcat, 3, formatter.str(), __FILE__, __LINE__ );
                                return false;
                        }
                } else if (split_vec.size() == 2) { // split_vec[0] = key, split_vec[1]=value
                        if (section_string.size() == 0) {
                                boost::format   formatter("don't match first section. key = %1%, value = %2%");
                                formatter % split_vec[0] % split_vec[1];
                                std::cout << formatter.str() << "\n";
                                //Logger::putLogFatal( logcat, 4, formatter.str(), __FILE__, __LINE__ );
                                return false;
                        }
                        boost::algorithm::trim(split_vec[0]);   //trim keys
                        boost::algorithm::trim(split_vec[1]);   //trim values
                        std::string     key = section_string;//create section.key
                        key += ".";
                        key += split_vec[0];
                        if (split_vec[1].at(0) == '\"' && split_vec[1].at(split_vec[1].size() - 1) == '\"') { //string value
                                std::string strvalue = split_vec[1].substr(1, split_vec[1].size() - 2); //erase double cautation
                                if (comp == PARAM_COMP_SSL) {
                                        multistring_map.insert(string_pair_type(key, strvalue));          //tmp map insert
                                } else {
                                        std::pair< string_map_type::iterator, bool > ret =
                                                string_map.insert(string_pair_type(key, strvalue));       //tmp map insert
                                        if (!ret.second) {       //insert error
                                                boost::format   formatter("section.key is duplicate. section.key = %1%, value = %2%");
                                                formatter % key % strvalue;
                                                std::cout << formatter.str() << "\n";
                                                //Logger::putLogError( logcat, 1, formatter.str(), __FILE__, __LINE__ );
                                        }
                                }
                        } else { // int value
                                try {
                                        int     intvalue = boost::lexical_cast<int>(split_vec[1]);
                                        std::pair< int_map_type::iterator, bool > ret =
                                                int_map.insert(int_pair_type(key, intvalue));     // tmp map insert
                                        if (!ret.second) {
                                                boost::format   formatter("section.key is duplicate. section.key = %1%, value = %2%");
                                                formatter % key % intvalue;
                                                std::cout << formatter.str() << "\n";
                                                //Logger::putLogError( logcat, 2, formatter.str(), __FILE__, __LINE__ );
                                        }
                                } catch (boost::bad_lexical_cast &cast) {
                                        boost::format   formatter("value is not numeric : %1%");
                                        formatter % split_vec[1];
                                        std::cout << formatter.str() << "\n";
                                        //Logger::putLogFatal( logcat, 5, formatter.str() , __FILE__, __LINE__ );
                                }
                        }
                } else {
                        boost::format   formatter("line is not support line = %1%");
                        formatter % line;
                        std::cout << formatter.str() << "\n";
                        //Logger::putLogError( logcat, 3, formatter.str(), __FILE__, __LINE__ );
                }
        }

        //convert temporaly map to global map.
        if (comp == PARAM_COMP_ALL) {
                intMap.clear();
                BOOST_FOREACH(int_pair_type p, int_map) {  // all temp int map copy
                        intMap.insert(p);
                }
                stringMap.clear();
                BOOST_FOREACH(string_pair_type p, string_map) {  //all temp string map copy
                        stringMap.insert(p);
                }
        } else if (comp == PARAM_COMP_NOCAT) {   // comp error!
                std::cout << "parameter_component_none is not suport \n";
                //Logger::putLogError( logcat, 4, "parameter_component_none is not suport", __FILE__, __LINE__ );
        } else {
                std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator section_itr = tag_section_table_map.find(comp);
                std::string section = section_itr->second;
                for (int_map_type::iterator itr = intMap.begin();
                     itr != intMap.end();
                     ++itr) {
                        split_vector_type split_vec;
                        boost::algorithm::split(split_vec, itr->first, boost::algorithm::is_any_of("."));
                        if (split_vec[0] == section) {
                                intMap.erase(itr);
                                itr = intMap.begin();
                        }
                }
                BOOST_FOREACH(int_pair_type p, int_map) {
                        split_vector_type       split_vec;
                        boost::algorithm::split(split_vec, p.first, boost::algorithm::is_any_of("."));
                        if (split_vec[0] == section) {
                                std::pair< int_map_type::iterator, bool >       ret = intMap.insert(p);
                                if (!ret.second) {
                                        boost::format   formatter("not insert key = %1%, value = %2% ");
                                        formatter % p.first % p.second;
                                        std::cout << formatter.str() << "\n";
                                        //Logger::putLogError( logcat, 5, formatter.str(), __FILE__, __LINE__ );
                                }
                        }
                }

                for (string_map_type::iterator itr = stringMap.begin();
                     itr != stringMap.end();
                     ++itr) {
                        split_vector_type split_vec;
                        boost::algorithm::split(split_vec, itr->first, boost::algorithm::is_any_of("."));
                        if (split_vec[0] == section) {
                                stringMap.erase(itr);
                                itr = stringMap.begin();
                        }
                }
                BOOST_FOREACH(string_pair_type p, string_map) {
                        split_vector_type       split_vec;
                        boost::algorithm::split(split_vec, p.first, boost::algorithm::is_any_of("."));
                        if (split_vec[0] == section) {
                                std::pair< string_map_type::iterator, bool >    ret = stringMap.insert(p);
                                if (!ret.second) {
                                        boost::format   formatter("not insert key = %1%, value = %2% ");
                                        formatter % p.first % p.second;
                                        std::cout << formatter.str() << "\n";
                                        //Logger::putLogError( logcat, 6, formatter.str(), __FILE__, __LINE__ );
                                }
                        }
                }

                multistringMap.clear();
                BOOST_FOREACH(string_pair_type p, multistring_map) {  //all temp multistring map copy
                        multistringMap.insert(p);
                }
        }

        return true;
}

//! get a integer parameter
//! @param[in]  PARAMETER_COMPONENT_TAG
//! @param[in]  const std::string
//!     @param[out]     error code
//! @return             integer value
int     l7vs::ParameterImpl::get_int(const l7vs::PARAMETER_COMPONENT_TAG comp,
                                     const std::string &key,
                                     l7vs::error_code &err)
{
        boost::mutex::scoped_lock       lock(param_mutex);
        err.setter(false, "");
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator      section_table_iterator = tag_section_table_map.find(comp);
        int_map_type::iterator intmap_iterator = intMap.find(section_table_iterator->second + "." + key);
        if (intmap_iterator != intMap.end()) {
                return intmap_iterator->second;
        } else {
                err.setter(true, "don't find key");
        }
        return 0;
}

//! get a string parameter
//! @param[in]  PARAMETER_COMPONENT_TAG
//! @param[in]  const std::string
//! @param[out] error code
//! @return             string value
std::string     l7vs::ParameterImpl::get_string(const l7vs::PARAMETER_COMPONENT_TAG comp,
                const std::string &key,
                l7vs::error_code &err)
{
        boost::mutex::scoped_lock       lock(param_mutex);
        err.setter(false, "");
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator      section_table_iterator = tag_section_table_map.find(comp);
        string_map_type::iterator strmap_iterator = stringMap.find(section_table_iterator->second + "." + key);
        if (strmap_iterator != stringMap.end()) {
                return strmap_iterator->second;
        } else {
                err.setter(true, "don't find key");
        }
        return std::string("");
}

//! get multistring parameter
//! @param[in]    PARAMETER_COMPONENT_TAG
//! @param[in]    const std::string
//! @param[inout] std::multimap<std::string, std::string>&
//! @param[out]   error code
//! @return       last found string value
std::string l7vs::ParameterImpl::get_multistring(const PARAMETER_COMPONENT_TAG comp,
                const std::string &key,
                multistring_map_type &retmap,
                l7vs::error_code &err)
{
        boost::mutex::scoped_lock lock(param_mutex);
        err.setter(false, "");
        std::string retstr = "";
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator section_table_iterator = tag_section_table_map.find(comp);
        std::string comp_key = section_table_iterator->second + "." + key;
        retmap.clear();
        for (multistring_map_type::iterator itr = multistringMap.begin(); itr != multistringMap.end(); ++itr) {
                if (itr->first == comp_key) {
                        retmap.insert(std::pair<std::string, std::string>(key, itr->second));
                        retstr = itr->second;
                }
        }
        if (retstr == "") {
                err.setter(true, "don't find key");
        }
        return retstr;
}




void    l7vs::ParameterImpl::setIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string &key, const int value)
{
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator      section_table_iterator = tag_section_table_map.find(comp);

        intMap[ section_table_iterator->second + "." + key] = value;

}

void    l7vs::ParameterImpl::setStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string &key, const std::string &value)
{
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator      section_table_iterator = tag_section_table_map.find(comp);

        stringMap[ section_table_iterator->second + "." + key ] = value;

}

void    l7vs::ParameterImpl::deleteIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string &key)
{
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator      section_table_iterator = tag_section_table_map.find(comp);
        intMap.erase(section_table_iterator->second + "." + key);
}

void    l7vs::ParameterImpl::deleteStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string &key)
{
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator      section_table_iterator = tag_section_table_map.find(comp);
        stringMap.erase(section_table_iterator->second + "." + key);
}


