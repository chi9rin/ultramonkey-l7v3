#ifndef SESSION_RESULT_MESSAGE_H
#define SESSION_RESULT_MESSAGE_H

#include <string>

namespace l7vs{
	struct session_result_message{
		bool flag;
		std::string message;

		bool operator==(const session_result_message& in){
			return ((flag == in.flag) && (message == in.message));
		}
		bool operator!=(const session_result_message& in){
			return ((flag != in.flag) || (message != in.message));
		}

	};// struct session_result_message
}// namespace l7vs

#endif//SESSION_RESULT_MESSAGE_H
 
 
