#include <string>

namespace l7vs{
//! @class	error_code
//!	@brief	getValue error
//! @brief	this class is POD
class	error_code{
protected:
	bool		flag;	//!<	errorcode_flag
	std::string	msg;
public:
	error_code() : flag(false){}	//!< constractor
	error_code( bool inflg, std::string& instr ){
		flag = inflg;
		msg = instr;
	}
	bool	operator==( const bool in )const { return ( flag == in ); } //!< operator== orverload
	bool	operator!=( const bool in )const { return ( flag != in ); } //!< operator!= orverload
	bool	operator!() const { return !flag; } //!< operator! orverload
	typedef void (*unspecified_bool_type)();	//!< if return function
	static void unspecified_bool_true() {}		//!< if true orverload function
	operator unspecified_bool_type() const { return flag == 0 ? 0 : unspecified_bool_true; } //!< if() orverload
	const std::string&	get_message() const { return msg; }		//
};

}
