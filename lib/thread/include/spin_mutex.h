#ifndef	SPIN_MUTEX_H
#define	SPIN_MUTEX_H
//
// (C) Copyright 2009- Norihisa Nakai
// Distributed under the Boost Software License, Version 1.0
// (See accompanying file LICENSE_1_0.txt http://www.boost.org/LICENSE_1_0.txt)


#include <boost/noncopyable.hpp>
#include <boost/thread/exceptions.hpp>
#include <error.h>
#include <pthread.h>

namespace ultramonkey{

class	spin_mutex : boost::noncopyable{
protected:
  pthread_spinlock_t	m;
public:
  spin_mutex(){
	int const res = pthread_spin_init( &m, NULL );
	if( res ) throw boost::thread_resource_error();
  }

  ~spin_mutex(){
	pthread_spin_destroy(&m);
  }

  void	lock(){	pthread_spin_lock(&m); }
  void	unlock(){ pthread_spin_unlock(&m); }
  bool	try_lock(){
	int const res = pthread_spin_trylock(&m);
	return !res;
  }
};

}	// namespace ultramonkey
#endif	//SPIN_MUTEX_H