#ifndef ATOMIC_PTR_H
#define ATOMIC_PTR_H
#include <boost/utility.hpp>
#include <iostream>
template<class T> class atomic_ptr :boost::noncopyable
{
protected:
	T _p;
public:
	T get(){
		return _p;
	}
	atomic_ptr& operator=(T _q) {
		__sync_lock_test_and_set(&_p,_q);
		return *this;
	}
	atomic_ptr& operator++(int){
		_p++;
		return *this;
	}
	atomic_ptr& operator--(int){
		_p--;
		return *this;
	}
	bool operator<(T _q) const{
		return (_p < _q);
	}
	bool operator<=(T _q) const{
		return (_p <= _q);
	}
	bool operator>(T _q) const{
		return (_p > _q);
	}
	bool operator>=(T _q) const{
		return (_p >= _q);
	}
	bool operator==(T _q) const{
		return (_p == _q);
	}
	bool operator!=(T _q) const{
		return (_p != _q);
	}
};


template <>
class atomic_ptr<int>
{
protected:
	int _p;
public:
	int get(){
		return _p;
	}
	atomic_ptr& operator=(const int _q) {
		__sync_lock_test_and_set(&_p,_q);
		return *this;
	}
	int operator++(int){
		__sync_add_and_fetch(&_p,1);
		return _p;
	}
	int operator--(int){
		__sync_sub_and_fetch(&_p,1);
		return _p;
	}
	bool operator<(const int _q) const{
		return (_p < _q);
	}
	bool operator<=(const int _q) const{
		return (_p <= _q);
	}
	bool operator>(const int _q) const{
		return (_p > _q);
	}
	bool operator>=(const int _q) const{
		return (_p >= _q);
	}
	bool operator==(const int _q) const{
		return (_p == _q);
	}
	bool operator!=(const int _q) const{
		return (_p != _q);
	}

};

#endif
