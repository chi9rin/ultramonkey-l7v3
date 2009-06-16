#ifndef LOCKFREE_QUEUE_H
#define LOCFFREE_QUEUE_H

#include <boost/noncopyable.hpp>
#include <queue>
#include "wrlock.h"

namespace l7vs{

template<class Tvalue>
class lockfree_queue : protected boost::noncopyable {
protected:
	std::queue< Tvalue* >	stl_queue;
	wr_mutex		mutex;
public:
	explicit lockfree_queue(){ 
	}

	// destractor
	~lockfree_queue(){
	}
	
	void push(const Tvalue* value){
		mutex.wrlock();
		stl_queue.push( const_cast< Tvalue* >( value ) );
		mutex.unlock();
	}

	Tvalue* pop() {
		mutex.wrlock();
		Tvalue*	ptr = stl_queue.front();
		stl_queue.pop();
		mutex.unlock();
		return ptr;
	}

	bool empty() const{
		return stl_queue.empty();
	}
	size_t size() const{
		return stl_queue.size();
	}
};

} // namespace l7vs

#endif	// LOCKFREE_QUEUE_H
