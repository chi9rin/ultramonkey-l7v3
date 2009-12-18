#include <vector>
#include <fstream>
#include <stdexcept>
#include "./parameter_impl.h"
#include "./logger_enum.h"
#include "parameter_enum.h"
#include "utility.h"
#include "logger.h"

//#include "lexical_cast.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/format.hpp>



#define	LINE_LENGTH	4096


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

#define    SESSION_POOL_NUM_PARAM    (256)

static bool create_map_flag = false;

//
//! Initialize ParameterImpl class
//! @return true    success
//! @return false   failer
bool l7vs::ParameterImpl::init(){

    boost::mutex::scoped_lock lock( create_mutex );

    if( create_map_flag ) return true;

    tag_section_table_map.clear();
    multistringMap.clear();
    intMap.clear();
    tag_section_table_map[PARAM_COMP_L7VSD]          = "l7vsd";
    tag_section_table_map[PARAM_COMP_COMMAND]        = "command"; 
    tag_section_table_map[PARAM_COMP_SESSION]        = "session";
    tag_section_table_map[PARAM_COMP_VIRTUALSERVICE] = "virtualservice";
    tag_section_table_map[PARAM_COMP_MODULE]         = "module";
    tag_section_table_map[PARAM_COMP_REPLICATION]    = "replication";
    tag_section_table_map[PARAM_COMP_LOGGER]         = "logger";
    tag_section_table_map[PARAM_COMP_L7VSADM]        = "l7vsadm";
    tag_section_table_map[PARAM_COMP_SNMPAGENT]      = "snmpagent";
    tag_section_table_map[PARAM_COMP_SSL]            = "ssl";
    create_map_flag = read_file( PARAM_COMP_ALL, PARAMETER_FILE );
    return create_map_flag;
}

//! Initialize ParameterImpl class(file designation)
//! @param[in]    COMPONENT TAG
//! @param[in]    file name
//! @return true  success
//! @return false failer
bool l7vs::ParameterImpl::init( const l7vs::PARAMETER_COMPONENT_TAG comp,
                                const std::string& file_name ){

    boost::mutex::scoped_lock lock( create_mutex );

    if( !create_map_flag ){
        // parameter_impl isn't initialized
        Logger::putLogFatal( logcat, 4, "NOT INITIALIZED", __FILE__, __LINE__ );
        return false;
    }

    // read target file
    bool read_result = read_file( comp, file_name );

    return read_result;
}

//! read config file
//! @param[in]    COMPONENT TAG
//! @param[in]    file name
//! @return true read success
//! @return false read false
bool l7vs::ParameterImpl::read_file( const l7vs::PARAMETER_COMPONENT_TAG comp,
                                     const std::string& file_name ){

    typedef std::vector< std::string >               split_vector_type;
    typedef std::pair< std::string, int >            int_pair_type;
    typedef std::pair< std::string, std::string >    string_pair_type;

    boost::mutex::scoped_lock lock( param_mutex );
    std::string      line;
    std::ifstream    ifs( file_name.c_str() );
    multistring_map_type  multistring_map;
    int_map_type     int_map;

    if( !ifs ){
        // don't open config file.
        std::stringstream buf;
        buf << "CONFIG FILE NOT OPEN : " << file_name;
        Logger::putLogFatal( logcat, 5, buf.str() , __FILE__, __LINE__ );
        return false;
    }

    std::string    section_string;
    while( std::getline( ifs, line ) ){
        // triming
        boost::algorithm::trim( line );
        // zero line skip
        if( line.size() == 0 ) continue;
        // comment line skip
        if( line[0] == '#' ) continue;
        // comment clear
        std::string::size_type pos = line.find( '#' );
        if( pos != std::string::npos ) line = line.substr( 0, pos );

        split_vector_type split_vec;
        boost::algorithm::split( split_vec,
                                 line,
                                 boost::algorithm::is_any_of( "=" ) );
        // [section(split_vec[0])]
        if( split_vec.size() == 1 ){
            // get section_string
            if( split_vec[0].at(0) == '[' &&
                split_vec[0].at( split_vec[0].size()-1 ) == ']' ){
                // erase [
                section_string = 
                             split_vec[0].substr( 1, split_vec[0].size() - 2 );
            }
            else{
                boost::format formatter( "section tag false : %1%" );
                formatter % split_vec[0];
                Logger::putLogFatal( logcat, 6, formatter.str(),
                                     __FILE__, __LINE__ );
                return false;
            }
        }
        // key(split_vec[0]) = value(split_vec[1])
        else if( split_vec.size() == 2 ){
            // non get section_string
            if( section_string.size() == 0 ){
                boost::format formatter(
                    "don't match first section. key = %1%, value = %2%" );
                formatter % split_vec[0] % split_vec[1];
                Logger::putLogFatal( logcat, 7, formatter.str(),
                                     __FILE__, __LINE__ );
                return false;
            }
            // create section.key
            boost::algorithm::trim( split_vec[0] );
            boost::algorithm::trim( split_vec[1] );
            std::string key = file_name + "#";
            key += section_string;
            key += ".";
            key += split_vec[0];
            // insert string value
            if( split_vec[1].at(0) == '\"' &&
                split_vec[1].at( split_vec[1].size()-1 ) == '\"' ){
                // create string value
                std::string strvalue =
                               split_vec[1].substr( 1, split_vec[1].size()-2 );
                // check same key
                if( multistring_map.end() != multistring_map.find(key)){
                    // Only SSL component can designate more than one value.
                    // Other component overwrite tmp multimap

                    // get compornent tag from section_string
                    for( std::map<PARAMETER_COMPONENT_TAG, std::string>::iterator section_itr = tag_section_table_map.begin();
                         section_itr != tag_section_table_map.end();
                         ++section_itr ){
                        if( section_itr->second == section_string &&
                            section_itr->first != PARAM_COMP_SSL ){
                            multistring_map.erase( key );
                            break;
                        }
                    }
                }
                // tmp multimap insert
                multistring_map.insert( string_pair_type( key, strvalue ) );
            }
            // insert int value
            else{
                try{
                    // create int value
                    int intvalue = boost::lexical_cast<int>( split_vec[1] );

                    // check same key
                    if( int_map.end() != int_map.find( key ) ){
                        // Every component overwrite
                        int_map.erase( key );
                    }
                    // tmp map insert
                    std::pair< int_map_type::iterator, bool > ret =
                        int_map.insert( int_pair_type( key, intvalue ) );
                    // insert error
                    if( !ret.second ){
                        boost::format formatter(
                   "section.key is duplicate. section.key = %1%, value = %2%" );
                        formatter % key % intvalue;
                        Logger::putLogError( logcat, 1, formatter.str(),
                                             __FILE__, __LINE__ );
                    }
                }
                // exception handing
                catch( boost::bad_lexical_cast& cast ){
                    boost::format formatter( "value is not numeric : %1%" );
                    formatter % split_vec[1];
                    Logger::putLogFatal( logcat, 8, formatter.str(),
                                         __FILE__, __LINE__ );
                }
            }
        }
        // format error
        else{
            boost::format formatter( "line is not support line = %1%" );
            formatter % line;
            Logger::putLogError( logcat, 2, formatter.str(),
                                 __FILE__, __LINE__ );
        }
    }

    //convert temporaly map to global map.
    if( comp == PARAM_COMP_ALL ){
        intMap.clear();
        // all tmp int map copy
        BOOST_FOREACH( int_pair_type p, int_map ){
            intMap.insert( p );
        }
        multistringMap.clear();
        // all temp multistring map copy
        BOOST_FOREACH( string_pair_type p, multistring_map ){
            multistringMap.insert( p );
        }
    }
    // comp error
    else if( comp == PARAM_COMP_NOCAT ){
        Logger::putLogError( logcat, 3,
                             "parameter_component_none is not suport",
                             __FILE__, __LINE__ );
    }
    // TAG designation
    else{
        // get section
        std::map<PARAMETER_COMPONENT_TAG, std::string>::iterator section_itr =
                                            tag_section_table_map.find( comp );
        std::string section = section_itr->second;
        // tmp int map copy
        for( int_map_type::iterator itr = intMap.begin();
             itr != intMap.end(); ++itr ){
            split_vector_type file_vec;
            boost::algorithm::split( file_vec, itr->first,
                                     boost::algorithm::is_any_of( "#" ) );
            split_vector_type split_vec;
            boost::algorithm::split( split_vec, file_vec[1],
                                     boost::algorithm::is_any_of( "." ) );
            // target section
            if( (file_vec[0] + split_vec[0]) == (file_name + section) ){
                // erase section
                intMap.erase( itr );
                if( intMap.empty() ){
                    break;
                }
            }
        }
        BOOST_FOREACH( int_pair_type p, int_map ){
            split_vector_type file_vec;
            boost::algorithm::split( file_vec, p.first,
                                     boost::algorithm::is_any_of( "#" ) );
            split_vector_type split_vec;
            boost::algorithm::split( split_vec, file_vec[1],
                                     boost::algorithm::is_any_of( "." ) );
            // target section
            if( split_vec[0] == section ){
                // insert
                std::pair<int_map_type::iterator, bool> ret = intMap.insert(p);
                // insert error
                if( !ret.second ){
                    boost::format formatter(
                        "not insert key = %1%, value = %2% " );
                    formatter % p.first % p.second;
                    Logger::putLogError( logcat, 4, formatter.str(),
                                         __FILE__, __LINE__ );
                }
            }
        }

        // tmp multistring map copy
        for( multistring_map_type::iterator itr = multistringMap.begin();
             itr != multistringMap.end(); ++itr ){
            split_vector_type file_vec;
            boost::algorithm::split( file_vec, itr->first,
                                     boost::algorithm::is_any_of( "#" ) );
            split_vector_type split_vec;
            boost::algorithm::split( split_vec, file_vec[1],
                                     boost::algorithm::is_any_of( "." ) );
            // target section
            if( (file_vec[0] + split_vec[0]) == (file_name + section) ){
                // erase section
                multistringMap.erase( itr );
                if( multistringMap.empty() )
                {
                    break;
                }
            }
        }
        BOOST_FOREACH( string_pair_type p, multistring_map ){
            split_vector_type file_vec;
            boost::algorithm::split( file_vec, p.first,
                                     boost::algorithm::is_any_of( "#" ) );
            split_vector_type split_vec;
            boost::algorithm::split( split_vec, file_vec[1],
                                     boost::algorithm::is_any_of( "." ) );
            // target section
            if( split_vec[0] == section ){
                // insert
                multistringMap.insert( p );
            }
        }
    }
    return true;
}

//! get a integer parameter
//! @param[in]    comp          section TAG
//! @param[in]    key           key string
//! @param[out]   err           error_code
//! @param[in]    file_name     configuration file name
//! @return       integer value
int l7vs::ParameterImpl::get_int( const l7vs::PARAMETER_COMPONENT_TAG comp,
                                  const std::string& key,
                                  l7vs::error_code& err,
                                  const std::string& file_name ){

                                    
    if ( key == "session_thread_pool_size" ) {
        // テスト用の値として、l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULTに設定
        // されている値を返却するよう処理を修正する。
        return(256);
    }

    boost::mutex::scoped_lock lock( param_mutex );

    // error_code clear
    err.setter(false, "");
    // find section
    std::map<PARAMETER_COMPONENT_TAG, std::string>::iterator
        section_table_iterator = tag_section_table_map.find( comp );
    // create search key
    std::string search_key = file_name +"#";
    search_key += section_table_iterator->second;
    search_key += ".";
    search_key += key;
    int_map_type::iterator intmap_iterator = intMap.find( search_key );
    if( intmap_iterator != intMap.end() ) {
        return intmap_iterator->second;
    } else {
        err.setter( true, "don't find key" );
    }
    return 0;
}

//! get a string parameter
//! @param[in]  comp        section TAG
//! @param[in]  key         key string
//! @param[out] err         error_code
//! @param[in]  file_name   configuration file name
//! @return     string value
std::string l7vs::ParameterImpl::get_string(
                                 const l7vs::PARAMETER_COMPONENT_TAG comp,
                                 const std::string& key,
                                 l7vs::error_code& err,
                                 const std::string& file_name ){

    std::vector<std::string> retvec;
    // get multistring
    get_multistring( comp, key, retvec, err, file_name );

    if( !err ){
        // return string
        return retvec[0];
    }
    return std::string("");
}

//! get multistring parameter
//! @param[in]    comp       section TAG
//! @param[in]    key        key string
//! @param[inout] retvec     string vector
//! @param[out]   err        error_code
//! @param[in]    file_name  configuration file name
//! @return       void
void l7vs::ParameterImpl::get_multistring( const PARAMETER_COMPONENT_TAG comp,
                                           const std::string& key,
                                           std::vector<std::string>& retvec,
                                           l7vs::error_code& err,
                                           const std::string& file_name )
{

    boost::mutex::scoped_lock lock( param_mutex );
    // error_code clear
    err.setter(false, "");
    // find section
    std::map<PARAMETER_COMPONENT_TAG, std::string>::iterator
        section_table_iterator = tag_section_table_map.find( comp );
    // create search key
    std::string search_key = file_name +"#";
    search_key += section_table_iterator->second;
    search_key += ".";
    search_key += key;
    retvec.clear();
    for (multistring_map_type::iterator itr = multistringMap.begin();
         itr != multistringMap.end(); ++itr ) {
        // hit search_key
        if (itr->first == search_key) {
            retvec.push_back(itr->second);
        }
    }
    // doesn't exist.
    if (retvec.empty()) {
        err.setter(true, "don't find key");
    }
    return;
}



void    l7vs::ParameterImpl::setIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key, const int value)
{
	std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator	section_table_iterator = tag_section_table_map.find( comp );

        intMap[ section_table_iterator->second + "." + key] = value;

}

void    l7vs::ParameterImpl::setStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key, const std::string& value)
{
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator      section_table_iterator = tag_section_table_map.find( comp );
        std::string key_str = section_table_iterator->second + "." + key;

        multistringMap.insert( make_pair( key_str , value ) );
        //multistringMap[ section_table_iterator->second + "." + key ] = value;

}

void    l7vs::ParameterImpl::deleteIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator      section_table_iterator = tag_section_table_map.find( comp );
        intMap.erase( section_table_iterator->second + "." + key );
}

void    l7vs::ParameterImpl::deleteStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
        std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator      section_table_iterator = tag_section_table_map.find( comp );
        multistringMap.erase( section_table_iterator->second + "." + key );
}


