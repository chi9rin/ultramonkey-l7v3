#ifndef	RW_MUTEX_H
#define	RW_MUTEX_H
//
// (C) Copyright 2009- Norihisa Nakai
// Distributed under the Boost Software License, Version 1.0
// (See accompanying file LICENSE_1_0.txt http://www.boost.org/LICENSE_1_0.txt)

#include <boost/noncopyable.hpp>
#include <boost/thread/exceptions.hpp>
#include <error.h>
#include <pthread.h>

namespace ultramonkey{

class	rw_mutex : boost::noncopyable{
protected:
  pthread_rwlock_t	m;
public:
  rw_mutex(){
	int const res = pthread_rwlock_init( &m, NULL );
	if( res ) throw boost::thread_resource_error();
  }

  ~rw_mutex(){
	pthread_rwlock_destroy(&m);
  }

  void	rdlock(){ pthread_rwlock_rdlock( &m ); }
  void	wrlock(){ pthread_rwlock_wrlock( &m ); }
  bool	try_rdlock(){
	int const res = pthread_rwlock_tryrdlock( &m );
	return res;
  }
  bool	try_wrlock(){
	int const res = pthread_rwlock_trywrlock(&m);
	return res;
  }
  void	unlock(){
	pthread_rwlock_unlock( &m );
  }

};


}	//namespace ultramonkey

#endif	// RW_MUTEX_H