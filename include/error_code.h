#ifndef	ERROR_CODE_H
#define	ERROR_CODE_H
//
// (C) Copyright 2009- Norihisa Nakai
// Distributed under the Boost Software License, Version 1.0
// (See accompanying file LICENSE_1_0.txt http://www.boost.org/LICENSE_1_0.txt)

#include <string>

namespace ultramonkey{

//! @class	error_code
//!	@brief	getValue error
//! @brief	this class is POD
class	error_code{
protected:
	bool		flag;	//!<	errorcode flag
	std::string	msg;	//!<	errorcode message
public:
	error_code() : flag(false){}	//!< constractor
	//! setter constractor
	//! @param[in]	flags
	//! @param[in]	error message
	error_code( const bool inflg, const std::string& instr ) : flag( inflag ), msg( instr ){}
	bool	operator==( const bool in ) const { return ( flag == in ); } //!< operator== orverload
	bool	operator!=( const bool in ) const { return ( flag != in ); } //!< operator!= orverload
	bool	operator!() const { return !flag; } //!< operator! orverload
	typedef void (*unspecified_bool_type)();	//!< if return function
	static void unspecified_bool_true() {}		//!< if true orverload function
	operator unspecified_bool_type() const { return flag == 0 ? 0 : unspecified_bool_true; } //!< if() orverload
	const std::string&	get_message() const { return msg; }		//!< message getter
	//! error setter
	//! @param[in]	flags
	//! @param[in]	error message
	void	setter( const bool flg, const std::string& instr ){
		flag = flg;
		msg = instr;
	}
};

}	// ultramonkey

#endif // ERROR_CODE_H
