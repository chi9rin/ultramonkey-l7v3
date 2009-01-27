//
//!	@file	session_result_message.h
//!	@brief	session result message class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef SESSION_RESULT_MESSAGE_H
#define SESSION_RESULT_MESSAGE_H

#include <string>

namespace l7vs{
//!	@struct	session_result_message
//! @brief	this struct is session class use result message.
	struct session_result_message{
		//! result flag
		bool flag;
		//! message
		std::string message;
		//! == operator
		//! @param[in]	equal check object.
		//! @return 	true is equal
		//! @return 	false is not equal
		bool operator==(const session_result_message& in){
			return ((flag == in.flag) && (message == in.message));
		}
		//! != operator
		//! @param[in]	inequal check object.
		//! @return 	true is inequal
		//! @return 	false is not inequal
		bool operator!=(const session_result_message& in){
			return ((flag != in.flag) || (message != in.message));
		}

	};// struct session_result_message
}// namespace l7vs

#endif//SESSION_RESULT_MESSAGE_H
 
 
