/*!
 *    @file    utility.h
 *    @brief    utility macro header.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#ifndef  __UTILITY_H__
#define  __UTILITY_H__

//! gcc extension code output macro.
//! static "if" weight algorithm teach compiler.
//! compiler output CPU TBL code.
//! usage:
//!    if( likely( hoge ) ){ fastcall } else { slowcall }
//! if( unlikely( hoge ) ){ slowcall } else { fastcall }
#define likely(c)   __builtin_expect(!!(c), 1)
#define unlikely(c) __builtin_expect(!!(c), 0)

#endif  //__UTILITY_H__
