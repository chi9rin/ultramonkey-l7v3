//
//	utility macro header.
//	(C) copyright n.nakai@sdy.co.jp
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
#ifndef  __UTILITY_H__
#define  __UTILITY_H__


//! gcc extention code output macro.
//! static "if" weight algolithm teach compiler.
//! compiler output CPU TBL code.
//! usage:
//!	if( likely( hoge ) ){ fasecall } else { slowcall }
//! if( unlikely( hoge ) ){ slowcall } else { fastcall }
#define likely(c)   __builtin_expect(!!(c), 1)
#define unlikely(c) __builtin_expect(!!(c), 0)

#endif  //__ULITITY_H__
