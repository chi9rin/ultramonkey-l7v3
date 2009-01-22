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
#include "parameter_impl.h"
#include "logger.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/format.hpp>

#ifndef PARAMETER_FILE
	#define PARAMETER_FILE "/etc/l7vs/l7vs.cf"
#endif //PARAMETER_FILE

#if !defined(LOGGER_PROCESS_VSD) && !defined(LOGGER_PROCESS_ADM) && !defined(LOGGER_PROCESS_SNM)
	#define LOGGER_PROCESS_VSD
#endif

#ifdef  LOGGER_PROCESS_SNM
	l7vs::LOG_CATEGORY_TAG logcat = l7vs::LOG_CAT_SSLPROXY_PARAMETER;
#elif	LOGGER_PROCESS_SNM
	l7vs::LOG_CATEGORY_TAG logcat = l7vs::LOG_CAT_SNMPAGENT_PARAMETER;
#elif   LOGGER_PROCESS_ADM
	l7vs::LOG_CATEGORY_TAG logcat = l7vs::LOG_CAT_L7VSADM_PARAMETER;
#else
	l7vs::LOG_CATEGORY_TAG logcat = l7vs::LOG_CAT_L7VSD_PARAMETER;
#endif

static bool	create_map_flag = false;

//
//! Initialize ParameterImpl class
//!	@return	true	success
//! @return false	failer
bool	l7vs::ParameterImpl::init(){
	boost::mutex::scoped_lock	lock( create_mutex );
	if( create_map_flag ) return true;

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
	tag_section_table_map[PARAM_COMP_SSLPROXY]		= "sslproxy";
	create_map_flag	= read_file( PARAM_COMP_ALL );
	return	create_map_flag;
}

//! read config file
//! @param[in]	COMPONENT TAG
//! @return true read success
//! @return false read false
bool	l7vs::ParameterImpl::read_file( const l7vs::PARAMETER_COMPONENT_TAG comp ){

	typedef std::vector< std::string > 				split_vector_type;
	typedef	std::pair< std::string, int > 			int_pair_type;
	typedef	std::pair< std::string, std::string >	string_pair_type;

	boost::mutex::scoped_lock	lock( param_mutex );
	std::string		line;
	std::ifstream	ifs( PARAMETER_FILE );
	string_map_type	string_map;
	int_map_type	int_map;

	if( !ifs ) return false;	// don't open config files.

	std::string	section_string;
	while( std::getline( ifs, line ) ){
		boost::algorithm::trim( line );		// triming
		if( line.size() == 0 ) continue;	// zero line skip
		if( line[0] == '#' ) continue;		// comment line skip
		std::string::size_type	pos = line.find( '#' );	// comment is clear
		if( pos != std::string::npos ) line = line.substr( 0, pos );

		split_vector_type split_vec;
		boost::algorithm::split( split_vec, line, boost::algorithm::is_any_of( "=" ) );
		if( split_vec.size() == 1 ){ // section
			if( split_vec[0].at(0) == '[' && split_vec[0].at( split_vec[0].size()-1 ) == ']' )
				section_string = split_vec[0].substr( 1, split_vec[0].size() - 2 );
			else{
				boost::format	formatter( "section tag false : %1%" );
				formatter % split_vec[0];
				Logger::putLogFatal( logcat, 0, formatter.str(), __FILE__, __LINE__ );
				return false;
			}
		}
		else if( split_vec.size() == 2 ){ // split_vec[0] = key, split_vec[1]=value
			if( section_string.size() == 0 ){
				boost::format	formatter("don't match first section. key = %1%, value = %2%" );
				formatter % split_vec[0] % split_vec[1];
				Logger::putLogFatal( logcat, 0, formatter.str(), __FILE__, __LINE__ );
				return false;
			}
			boost::algorithm::trim( split_vec[0] ); //trim keys
			boost::algorithm::trim( split_vec[1] ); //trim values
			std::string	key = section_string;//create section.key
			key += ".";
			key += split_vec[0];
			if( split_vec[1].at(0) == '\"' && split_vec[1].at( split_vec[1].size()-1 ) == '\"' ){ //string value
				std::string strvalue = split_vec[1].substr( 1, split_vec[1].size()-2 ); //erase double cautation
				std::pair< string_map_type::iterator, bool > ret =
					string_map.insert( string_pair_type( key, strvalue ) );	//tmp map insert
				if( !ret.second ){	//insert error
					boost::format	formatter( "section.key is duplicate. section.key = %1%, value = %2%" );
					formatter % key % strvalue;
					Logger::putLogError( logcat, 0, formatter.str(), __FILE__, __LINE__ );
				}
			}
			else{	// int value
				try{
					int	intvalue = boost::lexical_cast<int>( split_vec[1] );
					std::pair< int_map_type::iterator, bool > ret =
						int_map.insert( int_pair_type( key, intvalue ) ); // tmp map insert
					if( !ret.second ){
						boost::format	formatter( "section.key is duplicate. section.key = %1%, value = %2%" );
						formatter % key % intvalue;
						Logger::putLogError( logcat, 0, formatter.str(), __FILE__, __LINE__ );
					}
				}
				catch( boost::bad_lexical_cast& cast ){
					boost::format	formatter( "value is not numeric : %1%" );
					formatter % split_vec[1];
					Logger::putLogFatal( logcat, 0, formatter.str() , __FILE__, __LINE__ );
				}
			}
		}
		else{
			boost::format	formatter( "line is not support line = %1%" );
			formatter % line;
			Logger::putLogError( logcat, 0, formatter.str(), __FILE__, __LINE__ );
		}
	}

	//convert temporaly map to global map.
	if( comp == PARAM_COMP_ALL ){
		intMap.clear();
		BOOST_FOREACH( int_pair_type p, int_map ){ // all temp int map copy
			intMap.insert( p );
		}
		stringMap.clear();
		BOOST_FOREACH( string_pair_type p, string_map ){ //all temp string map copy
			stringMap.insert( p );
		}
	}
	else if( comp == PARAM_COMP_NOCAT ){	// comp error!
		Logger::putLogError( logcat, 0, "parameter_component_none is not suport", __FILE__, __LINE__ );
	}
	else{
		std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator section_itr = tag_section_table_map.find( comp );
		std::string section = section_itr->second;
		for( int_map_type::iterator itr = intMap.begin();
			 itr != intMap.end();
			 ++itr ){
			split_vector_type split_vec;
			boost::algorithm::split( split_vec, itr->first, boost::algorithm::is_any_of( "." ) );
			if( split_vec[0] == section ){
				intMap.erase( itr );
				itr = intMap.begin();
			}
		}
		BOOST_FOREACH( int_pair_type p, int_map ){
			split_vector_type	split_vec;
			boost::algorithm::split( split_vec, p.first, boost::algorithm::is_any_of( "." ) );
			if( split_vec[0] == section ){
				std::pair< int_map_type::iterator, bool >	ret = intMap.insert( p );
				if( !ret.second ){
					boost::format	formatter( "not insert key = %1%, value = %2% " );
					formatter % p.first % p.second;
					Logger::putLogError( logcat, 0, formatter.str(), __FILE__, __LINE__ );
				}
			}
		}

		for( string_map_type::iterator itr = stringMap.begin();
			 itr != stringMap.end();
			 ++itr ){
			split_vector_type split_vec;
			boost::algorithm::split( split_vec, itr->first, boost::algorithm::is_any_of( "." ) );
			if( split_vec[0] == section ){
				stringMap.erase( itr );
				itr = stringMap.begin();
			}
		}
		BOOST_FOREACH( string_pair_type p, string_map ){
			split_vector_type	split_vec;
			boost::algorithm::split( split_vec, p.first, boost::algorithm::is_any_of( "." ) );
			if( split_vec[0] == section ){
				std::pair< string_map_type::iterator, bool >	ret = stringMap.insert( p );
				if( !ret.second ){
					boost::format	formatter( "not insert key = %1%, value = %2% " );
					formatter % p.first % p.second;
					Logger::putLogError( logcat, 0, formatter.str(), __FILE__, __LINE__ );
				}
			}
		}
	}
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
	boost::mutex::scoped_lock	lock( param_mutex );
	std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator	section_table_iterator = tag_section_table_map.find( comp );
	int_map_type::iterator intmap_iterator = intMap.find( section_table_iterator->second + "." + key );
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
	boost::mutex::scoped_lock	lock( param_mutex );
	std::map< PARAMETER_COMPONENT_TAG, std::string >::iterator	section_table_iterator = tag_section_table_map.find( comp );
	string_map_type::iterator strmap_iterator = stringMap.find( section_table_iterator->second + "." + key );
	if( strmap_iterator != stringMap.end() )
		return strmap_iterator->second;
	else
		err.set_flag( true );
	return std::string("");
}
