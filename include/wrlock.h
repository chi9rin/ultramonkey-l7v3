#ifndef	WRLOCK_H
#define	WRLOCK_H

#include <pthread.h>
#include <boost/noncopyable.hpp>

namespace l7vs{

class	wr_mutex : private boost::noncopyable {
protected:
	pthread_rwlock_t	mutex;
public:
	wr_mutex(){
		pthread_rwlock_init( &mutex, NULL );
	}
	~wr_mutex(){
		pthread_rwlock_destroy( &mutex );
	}
	void	rdlock(){
		pthread_rwlock_rdlock( &mutex );
	}
	void	wrlock(){
		pthread_rwlock_wrlock( &mutex );
	}
	void	unlock(){
		pthread_rwlock_unlock( &mutex );
	}
};

template< class T >
class	read_scoped_lock : private boost::noncopyable {
protected:
	T&	mutex;
public:
	explicit read_scoped_lock( T& m ) : mutex( m ){
		mutex.rdlock();
	}
	~read_scoped_lock(){
		mutex.unlock();
	}
};

typedef	read_scoped_lock<wr_mutex>	rd_scoped_lock;

template< class T >
class	readwrite_scoped_lock : private boost::noncopyable {
protected:
	T&	mutex;
public:
	explicit readwrite_scoped_lock( T& m ) : mutex( m ){
		mutex.wrlock();
	}
	~readwrite_scoped_lock(){
		mutex.unlock();
	}
};

typedef	readwrite_scoped_lock<wr_mutex>	rw_scoped_lock;

}	//namespace l7vs
#endif	// WRLOCK
