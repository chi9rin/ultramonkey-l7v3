//
//	@file	parameter_imp;.h
//	@brief	ultramonke-l7 parameter impliment class
//
//	copyright(c) sdy corporation.2008
//	mail: n.nakai at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PARAMETER_IMPL_H
#define PARAMETER_IMPL_H

#include "parameter_enum.h"
#include <iostream>
#include <string>
#include <map>


namespace l7vs
{
	struct	component{
		std::string	section;
		void		(*function)();
		component(){ section = ""; function = NULL;}
		component( std::string str, void (*p_func)() ){ section = str; function = p_func;}
		component& operator=(const component& in_comp){ section = in_comp.section; function = in_comp.function;return *this;}
	};
	class ParameterImpl
	{
	private:
		std::string	removebrank( const std::string& );
		bool	readParameterFile( const std::string& );
		bool	isNumeric( const std::string& );

		std::multimap<std::string,std::string>	preparse;

		std::map<PARAMETER_COMPONENT_TAG, component>	compTable;
		std::map<std::string, std::string>	stringMap;
		std::map<std::string, int>		intMap;

	protected:
		ParameterImpl();
		ParameterImpl( const ParameterImpl& );
		ParameterImpl& operator=( const ParameterImpl& );

	public:
		static ParameterImpl & getInstance(){
			static ParameterImpl instance;
			return instance;
		}
		bool	init();
		bool	rereadFile( const PARAMETER_COMPONENT_TAG );
	
		bool	isIntExist( const PARAMETER_COMPONENT_TAG, const std::string& );
		bool	isStringExist( const PARAMETER_COMPONENT_TAG, const std::string& );
		int	getIntValue( const PARAMETER_COMPONENT_TAG, const std::string& );
		std::string	getStringValue( const PARAMETER_COMPONENT_TAG, const std::string& );

		void	registerFunctionPointer( const PARAMETER_COMPONENT_TAG, void (*)() );
	};
}
#endif	//__PARAMETER_IMPL_H__
