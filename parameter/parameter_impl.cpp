#include <vector>
#include <fstream>
#include <stdexcept>
#include "parameter_impl.h"
#include "logger_wrapper.h"

//#include "lexical_cast.h"
#include <boost/lexical_cast.hpp>

#ifndef PARAMETER_FILE
#define PARAMETER_FILE "/etc/l7vs/l7vs.cf"
#endif //PARAMETER_FILE

#define	LINE_LENGTH	4096

#if !defined(LOGGER_PROCESS_VSD) && !defined(LOGGER_PROCESS_ADM) && !defined(LOGGER_PROCESS_SNM)
#define LOGGER_PROCESS_VSD
#endif

#ifdef  LOGGER_PROCESS_SNM
LOG_CATEGORY_TAG parameter_cat = LOG_CAT_SNMPAGENT_PARAMETER;
#elif   LOGGER_PROCESS_ADM
LOG_CATEGORY_TAG parameter_cat = LOG_CAT_L7VSADM_PARAMETER;
#else
LOG_CATEGORY_TAG parameter_cat = LOG_CAT_L7VSD_PARAMETER;
#endif

void	loadLoggerConf(){
	l7vs::Logger::getInstance().loadConf();
}

/*!
 * ParameterImpl class Constructor
 */
l7vs::ParameterImpl::ParameterImpl()
{
}

/*!
 * Initialize ParameterImpl class
 */
bool	l7vs::ParameterImpl::init()
{
	bool	retbool = true;
	//clrear map objects
	compTable.clear();
	stringMap.clear();
	intMap.clear();

	//add member to component table
	compTable[PARAM_COMP_ALL]	= component("all", NULL);
	compTable[PARAM_COMP_NOCAT]	= component("nocategory", NULL);
	compTable[PARAM_COMP_L7VSD]	= component("l7vsd", NULL);
	compTable[PARAM_COMP_IOMUX]	= component("iomux", NULL);
	compTable[PARAM_COMP_LSOCK]	= component("lsock", NULL);
	compTable[PARAM_COMP_CONN]	= component("conn", NULL);
	compTable[PARAM_COMP_DEST]	= component("dest", NULL);
	compTable[PARAM_COMP_SERVICE]	= component("service", NULL);
	compTable[PARAM_COMP_MODULE]	= component("module", NULL);
	compTable[PARAM_COMP_REPLICATION]	= component("replication", NULL);
	compTable[PARAM_COMP_LOGGER]	= component("logger", loadLoggerConf);
	compTable[PARAM_COMP_L7VSADM]	= component("l7vsadm", NULL);
	compTable[PARAM_COMP_SNMPAGENT]	= component("snmpagent", NULL);

	//read all parameters
	retbool	= rereadFile( PARAM_COMP_ALL );

	return	retbool;
}

/*!
 * checks that an argument is a numeric.
 * @param[in]	const std::string
 * @return		is numeric = true / not numeric = false
 */
bool	l7vs::ParameterImpl::isNumeric(const std::string& val)
{
	try {
//		l7vs::lexical_cast<int>(val);
		boost::lexical_cast<int>(val);
	}
//	catch (std::exception& ex) {
	catch (boost::bad_lexical_cast& ex) {
		return false;
	}
	return true;
}

/*!
 * delete blank before and behind a sentence
 * @param[in]	const std::string
 * @return		std::string
 */
std::string	l7vs::ParameterImpl::removebrank( const std::string& val )
{
	std::string	str = val;
	//remove brank(head of the sentence and end of the sentence)
	std::string::iterator its;
	its = str.begin();
	while( its != str.end() ){
		if( ( ' ' == *its ) || ( '\t' == *its ) ){
			str.erase(its);
		}else break;
	}
	if( "" == str )return str;
	//remove brank(head of the sentence and end of the sentence)
	its = str.end() - 1;
	while( its != str.begin() ){
		if( ( ' ' == *its ) || ( '\t' == *its ) ){
			str.erase(its);
			its--;
		}else break;
	}
	return str;
}

/*!
 * Read Parameter from file
 * @param[in]	void
 * @return		success = true / fail = false
 */
bool	l7vs::ParameterImpl::readParameterFile( const std::string& val )
{
	bool	retval = true;
	char	readbuf[LINE_LENGTH];

	LOGGER_PUT_LOG_INFO( parameter_cat,1, "read parameter file : %s", val.c_str() );

	std::vector<std::string>	paramlines;
	try{
		paramlines.clear();
		preparse.clear();

		std::ifstream	ifs( val.c_str() );
		if( ifs.fail() ){
			return false;
		}
		while( !ifs.eof() ){
			memset( readbuf, 0, LINE_LENGTH );
			ifs.getline( readbuf, ( LINE_LENGTH - 1 ) );
			std::string	str = readbuf;
			str = removebrank( str );
			//remove coment line
			if( '#' == str[0] )continue;
			//remove brank
			if( !str.empty() )paramlines.push_back( str );
		}
		std::string	sectionname = compTable[PARAM_COMP_NOCAT].section;
		for( std::vector<std::string>::iterator it = paramlines.begin();
			it != paramlines.end(); it++ ){
			//pre-parse
			std::string str = *it;
			if( '[' == str[0] ){
				std::string	tmpstr = str.substr( str.rfind("]")+1 );
				tmpstr = removebrank( tmpstr.substr( 0, tmpstr.find( "#" ) ) );
				if( !tmpstr.empty() )continue;
				//section string validity check
				//invalid "#","[","]","\" character
				tmpstr = removebrank( str.substr( str.find( "[" )+1, str.rfind( "]", str.length() )-1 ) );
				if( std::string::npos != tmpstr.find( "[" ) )tmpstr = compTable[PARAM_COMP_NOCAT].section;
				if( std::string::npos != tmpstr.find( "]" ) )tmpstr = compTable[PARAM_COMP_NOCAT].section;
				if( std::string::npos != tmpstr.find( "\\" ) )tmpstr = compTable[PARAM_COMP_NOCAT].section;
				if( std::string::npos != tmpstr.find( "#" ) )tmpstr = compTable[PARAM_COMP_NOCAT].section;
				if( std::string::npos != tmpstr.find( "=" ) )tmpstr = compTable[PARAM_COMP_NOCAT].section;
				sectionname = tmpstr;
			}else{
				preparse.insert( std::pair<std::string,std::string>( sectionname,str ) );
			}
		}
	}
	catch( ... ){
		preparse.clear();
		retval	= false;
	}
	if( preparse.empty() )retval = false;
	return	retval;
}

/*!
 * read parameter and parse
 * @param[in]	void
 * @return		success = true / fail = false
 */
bool	l7vs::ParameterImpl::rereadFile(PARAMETER_COMPONENT_TAG comp)
{
	bool	retbool = true;

	LOGGER_PUT_LOG_INFO( parameter_cat,2, "read parameter : COMPONENT = %s", compTable[comp].section.c_str() );

	if( !readParameterFile( PARAMETER_FILE ) )return false;

	std::multimap<std::string,std::string>::iterator	it;
	std::multimap<std::string,std::string>::iterator	ite;
	if( PARAM_COMP_ALL == comp ){
		it = preparse.begin();
	}else{
		it = preparse.find( compTable[comp].section );
	}
	ite = preparse.end();
		
	for( std::multimap<std::string,std::string>::iterator its = it; its !=ite; its++ ){
		//be made an invalid line if there is no = in a setting line.
		if( std::string::npos == its->second.find( "=" ) )continue;
		std::string	key = removebrank( its->second.substr( 0, its->second.find( "=" ) ) );
		//Key string validity check
		if( key.empty() )continue;
		//invalid "#","[","]","\" character
		if( std::string::npos != key.find( "[" ))continue;
		if( std::string::npos != key.find( "]" ))continue;
		if( std::string::npos != key.find( "\\" ))continue;
		if( std::string::npos != key.find( "#" ) )continue;

		key	= its->first + "." + key;

		std::string	value = removebrank( its->second.substr( its->second.find( "=" )+1, std::string::npos ) );
		//Value string validity check
		if( '"' == value[0] ) {
			if( std::string::npos == value.find( "\"", 1 ) )continue;
			std::string tmpstr = removebrank( value.substr( value.find( "\"", 1 )+1, value.find( "#", value.find( "\"", 1 )+1  ) ) );
			if( !tmpstr.empty() )continue;
			tmpstr = removebrank( value.substr( 1, value.find( "\"", 1 )-1 ) );



			stringMap[key] = tmpstr;
		}else{
			value = value.substr( 0, value.find_first_of( "#", 0 ) );
			if( std::string::npos != value.find( "\\" ))continue;
			if( std::string::npos != value.find( "=" ))continue;
			if( value.empty() )continue;
			if( isNumeric( value ) ){
				try{
//					intMap[key] = l7vs::lexical_cast<int>( value );
					intMap[key] = boost::lexical_cast<int>( value );
				}
//				catch(std::exception& ex){
				catch (boost::bad_lexical_cast& ex) {
					stringMap[key] = value;
				}
			}else{
				stringMap[key] = value;
			}
		}
	}

	//set parameter
	std::map<PARAMETER_COMPONENT_TAG, component>::iterator itr = compTable.find( comp );
	if( itr != compTable.end() ){
		try{
			if( NULL != itr->second.function )itr->second.function();
		}
		catch( const std::logic_error& err ){
			LOGGER_PUT_LOG_ERROR( parameter_cat,1, "set parameter function failure at category: %s", itr->second.section.c_str() );
			retbool	= false;
		}
	}
	if( PARAM_COMP_ALL == comp ){
		for( itr = compTable.begin(); itr != compTable.end(); ++itr ){
			try{
				if( NULL != itr->second.function )itr->second.function();
			}
			catch( const std::logic_error& err ){
				LOGGER_PUT_LOG_ERROR( parameter_cat,2, "set parameter function failure at category: %s", itr->second.section.c_str() );
				retbool	= false;
			}
		}
	}

	return	retbool;
}

/*!
 * Existence of a integer parameter is checked
 * @param[in]	PARAMETER_COMPONENT_TAG
 * @param[in]	const std::string
 * @return		exist = true / not exist = false
 */
bool	l7vs::ParameterImpl::isIntExist(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
	bool	retval = false;
	std::string comp_key = compTable[comp].section + "." + key;
	if( intMap.end() != intMap.find( comp_key ) ){
		retval = true;
	}
	return	retval;
}

/*!
 * Existence of a string parameter is checked
 * @param[in]	PARAMETER_COMPONENT_TAG
 * @param[in]	const std::string
 * @return		exist = true / not exist = false
 */
bool	l7vs::ParameterImpl::isStringExist(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
	bool	retval = false;
	std::string comp_key = compTable[comp].section + "." + key;
	if( stringMap.end() != stringMap.find( comp_key ) ){
		retval = true;
	}
	return	retval;
}

/*!
 * get a integer parameter
 * @param[in]	PARAMETER_COMPONENT_TAG
 * @param[in]	const std::string
 * @return		integer
 */
int	l7vs::ParameterImpl::getIntValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
	std::string comp_key = compTable[comp].section + "." + key;
	return intMap.find( comp_key )->second;
}

/*!
 * get a string parameter
 * @param[in]	PARAMETER_COMPONENT_TAG
 * @param[in]	const std::string
 * @return		std::string
 */
std::string	l7vs::ParameterImpl::getStringValue(const PARAMETER_COMPONENT_TAG comp, const std::string& key)
{
	std::string	retstr = "";
	std::string	comp_key = compTable[comp].section + "." + key;
	std::map<std::string,std::string>::iterator it = stringMap.find( comp_key );
	if( it != stringMap.end() )
		retstr = stringMap.find( comp_key )->second;
	return	retstr;
}


