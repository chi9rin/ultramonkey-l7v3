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

#include <string>

namespace l7vs{

class module_base{
public:
	typedef	boost::function< LOG_LEVEL_TAG(void) >
								getloglevel_func_type;
	typedef	boost::function< void ( const LOG_LEVEL_TAG, const unsigned int, const std::string) >
								logger_func_type;
	typedef	boost::function< void ( const std::string&, unsigned int* ) >
								replicationpaymemory_func_type;
protected:
	//! module name string
	std::string	name;

	//! logger method
	getloglevel_func_type			getloglevel;
	logger_func_type				putLogFatal;
	logger_func_type				putLogError;
	logger_func_type				putLogWarn;
	logger_func_type				putLogInfo;
	logger_func_type				putLogDebug;

	//! replication memory peyment method
	replicationpaymemory_func_type	replication_pay_memory;

	//! replication area lock function object
	boost::function< void( void ) >	replication_area_lock;
	//! replication area unlock function object
	boost::function< void( void ) >	replication_area_unlock;

public:
	module_base(){}
	virtual ~module_base() = 0;
	virtual	bool	is_tcp() = 0;
	virtual	bool	is_udp() = 0;
	virtual	std::string&	get_name(){return name;};

	virtual	void	init_logger_functions(
							getloglevel_func_type	ingetloglevel,
							logger_func_type		inputLogFatal,
							logger_func_type		inputLogError,
							logger_func_type		inputLogWarn,
							logger_func_type		inputLogInfo,
							logger_func_type		inputLogDebug ) = 0; 

	virtual	void	init_replication_functions(
							replicationpaymemory_func_type  inreplication_pay_memory,
							boost::function< void( void ) > inlock_func,
							boost::function< void( void ) > inunlock_func ) = 0;

	//replication用インターフェイス
	//これが呼ばれたら、replication領域にデータを書き込む
	//注意：内部でループは不要です。
	//　　　ループとタイマー制御はvirtual_serviceで行います。
	//　　　virtual_serviceは設定時間に1回replication_interruptをCallします。
	//　　　replication_interrupt呼ばれたら1回データ書き込みを行います。
	virtual	void	replication_interrupt() = 0;
};

}	//namespace l7vsd

#endif //MODULE_BASE_H
