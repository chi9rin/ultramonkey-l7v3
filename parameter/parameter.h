//
//	@file	parameter.h
//	@brief	ultramonke-l7 parameter class
//
//	copyright(c) sdy corporation.2008
//	mail: n.nakai at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include "parameter_enum.h"

namespace l7vs{
	class ParameterImpl;

	class Parameter{
	protected:
		static Parameter instance;
		Parameter();
		Parameter( const Parameter& );
		Parameter&	operator=( const Parameter& );
		~Parameter();
	public:
		bool	rereadFile(const PARAMETER_COMPONENT_TAG);
	
		inline static	Parameter& getInstance(){
			return	instance;
		}
		bool	isIntExist(	const PARAMETER_COMPONENT_TAG,
							const std::string&);
		bool	isStringExist(
							const PARAMETER_COMPONENT_TAG,
							const std::string&);
		int		getIntValue(const PARAMETER_COMPONENT_TAG,
							const std::string&);
		std::string	getStringValue(const PARAMETER_COMPONENT_TAG,
							const std::string&);
	};
};
	
#endif	//PARAMETER_H
