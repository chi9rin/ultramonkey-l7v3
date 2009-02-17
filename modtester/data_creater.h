#ifndef	DATACREATER_H
#define	DATACREATER_H

#include <boost/array.hpp>
#include <string>


//caition! BUFFERSIZEを跨ぐものはできないので、別途用意する必要がある
struct	data_creater{
	enum	mode_tag{
		HTTPDATA = 0,
		SSL		 = 1
	};
	mode_tag	mode;
	std::string	get_cookie_name(){ return "monkey"; }
	data_creater( mode_tag in = HTTPDATA );
	void	create_data( boost::array<char, MAX_BUFFER_SIZE>& buf,
						 size_t&	length,
						 std::string& data_name );
};


#endif	//DATACREATER_H
