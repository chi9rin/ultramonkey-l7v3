#ifndef ATOMIC_H
#define ATOMIC_H
#include <boost/utility.hpp>

namespace l7vs{

template<class T> class atomic :boost::noncopyable{
protected:
	T volatile _p;
public:
	T get(){ return _p; }
	atomic& operator=(const T& _q) {
		__sync_lock_test_and_set(&_p,_q);
		return *this;
	}
	atomic& operator++(){
		_p++;
		return *this;
	}
	atomic& operator--(){
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


//int
template <>
class atomic<int>{
protected:
	int volatile _p;
public:
	int get(){
		return _p;
	}
	atomic& operator=(const int _q) {
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

//unsigned longlong
template <>
class atomic<unsigned long long>{
protected:
	unsigned long long volatile _p;
public:
	unsigned long long get(){
		return _p;
	}
	atomic& operator=(const unsigned long long _q) {
		__sync_lock_test_and_set(&_p,_q);
		return *this;
	}
	unsigned long long operator++(int){
		__sync_add_and_fetch(&_p,1);
		return _p;
	}
	unsigned long long operator--(int){
		__sync_sub_and_fetch(&_p,1);
		return _p;
	}
	bool operator<(const unsigned long long _q) const{
		return (_p < _q);
	}
	bool operator<=(const unsigned long long _q) const{
		return (_p <= _q);
	}
	bool operator>(const unsigned long long _q) const{
		return (_p > _q);
	}
	bool operator>=(const unsigned long long _q) const{
		return (_p >= _q);
	}
	bool operator==(const unsigned long long _q) const{
		return (_p == _q);
	}
	bool operator!=(const unsigned long long _q) const{
		return (_p != _q);
	}
};

} //namespace l7vsd
#endif
