#ifndef	SCOPED_LOCK_H
#define	SCOPED_LOCK_H
//
// (C) Copyright 2009- Norihisa Nakai
// Distributed under the Boost Software License, Version 1.0
// (See accompanying file LICENSE_1_0.txt http://www.boost.org/LICENSE_1_0.txt)

#include "spin_mutex.h"
#include "rw_mutex.h"

namespace ultramonkey{

template < class T >
class	scoped_lock : boost::noncopyable {
protected:
  T&		mtx;
public:
   explicit scoped_lock( T& mutex ) : mtx( mutex ){
	mtx.lock();
  }

  ~scoped_lock(){
	mtx.unlock();
  }
};

typedef	scoped_lock< spin_mutex >	spin_scoped_lock;

class	rd_scoped_lock : boost::noncopyable{
protected:
  rw_mutex&			mtx;
public:
  explicit rd_scoped_lock( rw_mutex& mutex ) : mtx( mutex ){
	mtx.rdlock();
  }
  ~rd_scoped_lock(){
	mtx.unlock();
  }
};

class	wr_scoped_lock {
protected:
  rw_mutex&			mtx;
public:
  wr_scoped_lock( rw_mutex& mutex ) : mtx( mutex ){
	mtx.wrlock();
  }
  ~wr_scoped_lock(){
	mtx.unlock();
  }
};

}	//namespace ultramonkey

#endif	// SCOPED_LOCK_H