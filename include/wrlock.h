//
//
//	read - write spin lock class.
//	this lock used CPU power upon blocking.
//	don't used contextswitch. little clitical section use.
//	(C) copyright n.nakai@sdy.co.jp
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
#ifndef	WRLOCK_H
#define	WRLOCK_H

#include <pthread.h>
#include <boost/noncopyable.hpp>

namespace l7vs{

//! read - write mutex class
//! non copy class. nonPOD.
//! don't double lock this mutex.
class	wr_mutex : private boost::noncopyable {
protected:
	pthread_rwlock_t	mutex;	//! mutex
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
	void	rdlock(){
		pthread_rwlock_rdlock( &mutex );
	}
	//! read - write lock function. blocking all rdlock and wrlock
	void	wrlock(){
		pthread_rwlock_wrlock( &mutex );
	}
	//! unlock funtion. unlock rdlock and wrlock.
	void	unlock(){
		pthread_rwlock_unlock( &mutex );
	}
};


//!	readable scoped lock class
//!	usage:
//!		l7vs::rw_mutex	mtx;
//!		{	<---- readable lock zone start
//!			l7vs]::rd_scoped_lock	lock( mtx );
//!		}	<---- readable lock zone end.
template< class T >
class	read_scoped_lock : private boost::noncopyable {
protected:
	T&	mutex;	//! mutex reference.
public:
	//!	constractor. use explicit keyword.
	explicit read_scoped_lock( T& m ) : mutex( m ){
		mutex.rdlock();
	}
	//! destractor.
	~read_scoped_lock(){
		mutex.unlock();
	}
};

//! using wr_mutex using template typedef.
typedef	read_scoped_lock<wr_mutex>	rd_scoped_lock;


//! read - write scoped lock.
//! usage:
//!	l7vs::wr_mutex	mtx;
//!	{	<---- read - write lock zone start. all using mutex blocking.
//!		l7vs::wr_scoped_lock	lock( mtx );
//!	}
template< class T >
class	readwrite_scoped_lock : private boost::noncopyable {
protected:
	T&	mutex;				//! mutex reference.
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
typedef	readwrite_scoped_lock<wr_mutex>	rw_scoped_lock;

}	//namespace l7vs
#endif	// WRLOCK
