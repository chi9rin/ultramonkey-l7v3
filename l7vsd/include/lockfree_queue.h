#ifndef LOCKFREE_QUEUE_H
#define LOCKFREE_QUEUE_H

#include <boost/noncopyable.hpp>

#include "utility.h"

namespace l7vs{

template<class Tvalue>
class lockfree_queue : protected boost::noncopyable {
protected:
	struct node_type {
		Tvalue*			value;
		node_type() : value( NULL ) {}
	};
	mutable volatile size_t	counter;
	volatile size_t			headloc;
	volatile size_t			tailloc;

	volatile node_type*		node;
	const size_t			element_num;
public:
	// constractor
	explicit lockfree_queue(size_t num = 65535) : counter(0),element_num( num ){
		node = new node_type[element_num];
		headloc = tailloc = 0;
	}

	// destractor
	~lockfree_queue(){
		delete [] node;
	}

	//pusher
	bool push(const Tvalue* value){
		size_t tail,nexttail;
		//transaction st
		while(true){
			start:
			tail = tailloc;
			nexttail = get_num_next(tail);
			if ( unlikely( node[tail].value ) ){
				//return false;
				//do spin case of full queue
				goto start;
			}
			if ( likely( __sync_bool_compare_and_swap(&tailloc,tail,nexttail) ) ) break;
		}
		//transaction ed
		node[tail].value = const_cast<Tvalue*>( value );
		__sync_add_and_fetch( &counter, 1 );
		return true;
	}

	//poper
	Tvalue* pop() {
		size_t head,nexthead;
		Tvalue* rtnvalue;
		//transaction st
		while(true){
			head = headloc;
			nexthead = get_num_next(head);
			if( unlikely( !(node[head].value) ) ){
				if( unlikely( headloc==tailloc ) ){
					//false
					return NULL;
				}
			}else{
				if( likely( __sync_bool_compare_and_swap(&headloc,head,nexthead) ) ){
					rtnvalue = node[head].value;
					break;
				}
			}
		}
		//transaction ed
		__sync_sub_and_fetch( &counter, 1 );
		node[head].value = NULL;
		return rtnvalue;
	}

	//size
	size_t size() const{ return counter; }

	//empty
	bool empty() const{ return !counter; }

private:
	//get next value head and tail cyclic over elemental number
	size_t get_num_next( const size_t num ){
		if( unlikely( num >= element_num ) ){
			return 0;
		}else{
			return num + 1;
		}
	}
};

} // namespace l7vs

#endif	// LOCKFREE_QUEUE_H
