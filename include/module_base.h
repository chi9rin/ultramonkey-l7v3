//
//	@file	module_base.h
//	@brief	shared object module abstract class
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	MODULE_BASE_H
#define	MODULE_BASE_H


namespace l7vsd{

class module_base
{
protected:
	std::string	name;
public:
	module_base(){}
	virtual ~module_base() = 0;
	virtual	bool	is_tcp() = 0;
	virtual	bool	is_udp() = 0;
	virtual	std::strung&	get_name(){return &string;};
};

};	//namespace l7vsd

#endif //MODULE_BASE_H
