#ifndef __LEXICAL_CAST_H__
#define __LEXICAL_CAST_H__

#include <string>
#include <sstream>

namespace l7vs
{
	class bad_lexical_cast : public std::bad_cast
	{
	public:
		bad_lexical_cast() {}
	};

	template <typename T>
	T lexical_cast(const std::string& s)
	{
		std::stringstream ss;
		T d;
		ss << s.c_str();
		while( !ss.eof() ){
			ss >> d;
			if(ss.fail()) {
				throw bad_lexical_cast();
			}
		}
		return d;
	}
}

#endif	//__LEXICAL_CAST_H__
