/*!
 *    @file    utility.h
 *    @brief    read - write spin lock class.
 *    @brief    this lock used CPU power upon blocking.
 *    @brief    don't used contextswitch. little clitical section use.
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

#ifndef    WRLOCK_H
#define    WRLOCK_H

#include <pthread.h>
#include <boost/noncopyable.hpp>

namespace l7vs{

//! read - write mutex class
//! non copy class. nonPOD.
//! don't double lock this mutex.
class    wr_mutex : private boost::noncopyable {
protected:
    pthread_rwlock_t    mutex;    //! mutex
public:
    //! default constractor
    wr_mutex(){
        pthread_rwlock_init( &mutex, NULL );
    }
    //! destractor.
    ~wr_mutex(){
        pthread_rwlock_destroy( &mutex );
    }
    //! reaqd lock function. non-blocking read threads.
    //! be blocked write thread mutex.
    void    rdlock(){
        pthread_rwlock_rdlock( &mutex );
    }
    //! read - write lock function. blocking all rdlock and wrlock
    void    wrlock(){
        pthread_rwlock_wrlock( &mutex );
    }
    //! unlock funtion. unlock rdlock and wrlock.
    void    unlock(){
        pthread_rwlock_unlock( &mutex );
    }
};


//!    readable scoped lock class
//!    usage:
//!        l7vs::rw_mutex    mtx;
//!        {    <---- readable lock zone start
//!            l7vs]::rd_scoped_lock    lock( mtx );
//!        }    <---- readable lock zone end.
template< class T >
class    read_scoped_lock : private boost::noncopyable {
protected:
    T&    mutex;    //! mutex reference.
public:
    //!    constractor. use explicit keyword.
    explicit read_scoped_lock( T& m ) : mutex( m ){
        mutex.rdlock();
    }
    //! destractor.
    ~read_scoped_lock(){
        mutex.unlock();
    }
};

//! using wr_mutex using template typedef.
typedef    read_scoped_lock<wr_mutex>    rd_scoped_lock;


//! read - write scoped lock.
//! usage:
//!    l7vs::wr_mutex    mtx;
//!    {    <---- read - write lock zone start. all using mutex blocking.
//!        l7vs::wr_scoped_lock    lock( mtx );
//!    }
template< class T >
class    readwrite_scoped_lock : private boost::noncopyable {
protected:
    T&    mutex;                //! mutex reference.
public:
    //! default constractor.
    explicit readwrite_scoped_lock( T& m ) : mutex( m ){
        mutex.wrlock();
    }
    //! default destractor
    ~readwrite_scoped_lock(){
        mutex.unlock();
    }
};

//! using wr_mutex using template typedef.
typedef    readwrite_scoped_lock<wr_mutex>    rw_scoped_lock;

}    //namespace l7vs
#endif    // WRLOCK
