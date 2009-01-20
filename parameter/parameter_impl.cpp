//
//!	@file	paramter_impl.cpp
//!	@brief	paramater impliment file.
//	coipyright (C) NTTComware corporation. 2008 - 2009
//	mail: n.nakai at sdy.co.jp
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <vector>
#include <fstream>
#include <stdexcept>
#include "parameter_impl.h"
#include "logger_enum.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#ifndef PARAMETER_FILE
	#define PARAMETER_FILE "/etc/l7vs/l7vs.cf"
#endif //PARAMETER_FILE

#if !defined(LOGGER_PROCESS_VSD) && !defined(LOGGER_PROCESS_ADM) && !defined(LOGGER_PROCESS_SNM)
	#define LOGGER_PROCESS_VSD
#endif

#ifdef  LOGGER_PROCESS_SNM
	l7vs::LOG_CATEGORY_TAG parameter_cat = l7vs::LOG_CAT_SNMPAGENT_PARAMETER;
#elif   LOGGER_PROCESS_ADM
	l7vs::LOG_CATEGORY_TAG parameter_cat = l7vs::LOG_CAT_L7VSADM_PARAMETER;
#else
	l7vs::LOG_CATEGORY_TAG parameter_cat = l7vs::LOG_CAT_L7VSD_PARAMETER;
#endif

//
//! Initialize ParameterImpl class
//!	@return	true	success
//! @return false	failer
bool	l7vs::ParameterImpl::init(){
	bool	retbool = true;
	//clrear map objects
	tag_section_table_map.clear();
	stringMap.clear();
	intMap.clear();

	tag_section_table_map[PARAM_COMP_L7VSD] 		= "l7vsd";
	tag_section_table_map[PARAM_COMP_COMMAND] 		= "command"; 
	tag_section_table_map[PARAM_COMP_SESSION]		= "session";
	tag_section_table_map[PARAM_COMP_VIRTUALSERVICE]= "virtualservice";
	tag_section_table_map[PARAM_COMP_MODULE]		= "module";
	tag_section_table_map[PARAM_COMP_REPLICATION]	= "replication";
	tag_section_table_map[PARAM_COMP_LOGGER]		= "logger";
	tag_section_table_map[PARAM_COMP_L7VSADM]		= "l7vsadm";
	tag_section_table_map[PARAM_COMP_SNMPAGENT]		= "snmpagent";
	//read all parameters
	retbool	= read_file( PARAM_COMP_ALL );

	return	retbool;
}

//! read config file
//! @param[in]	COMPONENT TAG
//! @return true read success
//! @return false read false
bool	l7vs::ParameterImpl::read_file( const l7vs::PARAMETER_COMPONENT_TAG comp ){

	typedef std::vector< std::string> split_vector_type;

	boost::mutex::scoped_lock( parameter_mutex );
	std::string	line;
	std::ifstream	ifs( PARAMETER_FILE );
	std::map< std::string, std::string >	string_map;
	std::map< std::string, int >			int_map;

	if( !ifs ) return false;	// don't open config files.

	while( std::getline( ifs, line ) ){
		std::string	section_string;
		boost::algorithm::trim( line );	// triming
		if( line.size() == 0 ) break;		// zero line skip
		if( line[0] == '#' ) break;		// comment line skip
		split_vector_type split_vec;
		boost::algorithm::split( split_vec, line, boost::algorithm::is_any_of( "=" ) );
		if( split_vec.size() == 1 ){ // section
			if( split_vec[0].at(0) == '[' && split_vec[0].at( split_vec[0].size() ) == ']' )
				section_string = split_vec[0].substr( 1, split_vec[0].size() - 1 );
			else return false;
		}
		else if( split_vec.size() == 2 ){ // split_vec[0] = key, split_vec[1]=value
			boost::algorithm::trim( split_vec[0] );
			boost::algorithm::trim( split_vec[1] );
			std::string	key = section_string;
			key += ".";
			key += split_vec[0];
			if( split_vec[1].at(0) =- '\"' && split_vec[1].at( split_vec[1].size()-1 ) == '\"' ){
				std::string strvalue = split_vec[1].substr( 1, split_vec[0].size() -1 );
				std::pair< std::map< std::string, std::string>::iterator, bool > ret =
					string_map.insert( std::pair<std::string, std::string>( key, strvalue ) );
				if( !ret.second ){	//insert error
					//hogehoge
				}
			}
			else{
				try{
					int	intvalue = boost::lexical_cast<int>( split_vec[1] );
					std::pair< std::map<std::string, int>::iterator, bool > ret =
						int_map.insert( std::pair<std::string,int>( key, intvalue ) );
					if( !ret.second ){
						//map insert error!
					}
				}
				catch( boost::bad_lexical_cast& cast ){
					return false;
				}
			}
		}
		else{
			return false;
		}
	}

	//convert temporaly map to global map.
	return true;
}


//! get a integer parameter
//! @param[in]	PARAMETER_COMPONENT_TAG
//! @param[in]	const std::string
//!	@param[out]	error code
//! @return		integer value
int	l7vs::ParameterImpl::get_int(	const l7vs::PARAMETER_COMPONENT_TAG comp,
									const std::string& key,
									l7vs::parameter::error_code& err ){
	boost::mutex::scoped_lock( param_mutex );
	std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator	section_table_iterator = tag_section_table_map.find( comp );
	std::map<std::string, int>::iterator intmap_iterator = intMap.find( section_table_iterator->second + "." + key );
	if( intmap_iterator != intMap.end() )
			return intmap_iterator->second;
	else
		err.set_flag( true );

	return 0;
}

//! get a string parameter
//! @param[in]	PARAMETER_COMPONENT_TAG
//! @param[in]	const std::string
//! @param[out]	error code
//! @return		string value

std::string	l7vs::ParameterImpl::get_string( const l7vs::PARAMETER_COMPONENT_TAG comp,
											 const std::string& key,
											 l7vs::parameter::error_code& err ){
	boost::mutex::scoped_lock( param_mutex );
	std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator	section_table_iterator = tag_section_table_map.find( comp );
	std::map<std::string, std::string>::iterator strmap_iterator = stringMap.find( section_table_iterator->second + "." + key );
	if( strmap_iterator != stringMap.end() )
		return strmap_iterator->second;
	else
		err.set_flag( true );
	return std::string("");
}
