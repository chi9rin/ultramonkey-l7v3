#ifndef LOCKFREE_QUEUE_H
#define LOCKFREE_QUEUE_H

#include <boost/noncopyable.hpp>

namespace l7vs{

template<class Tvalue>
class lockfree_queue : protected boost::noncopyable {
protected:
	struct node_type {
		Tvalue*					value;
		volatile node_type*  	next;
		volatile bool			dummy;
		node_type() : value( NULL ) , next( NULL ) , dummy ( false ) {}
	};
	mutable volatile size_t	counter;
	volatile node_type*	headloc;
	volatile node_type*	tailloc;

	node_type*		new_node;
public:
	// constractor
	explicit lockfree_queue() : counter(0){ 
		new_node = new node_type();
		headloc = tailloc = new_node;
		new_node->dummy = true;
	}

	// destractor
	~lockfree_queue(){
		while( counter-- ){
			pop();
		}
		delete headloc;
	}
	
	void push(const Tvalue* value){
		volatile	node_type* _new_node	= new node_type();
		volatile	node_type* _tail;
		volatile	node_type* _next;

		_new_node->value = const_cast<Tvalue*>( value );

		// transaction st
		while(1){
			_tail = tailloc;
			_next = tailloc->next;
			if(_tail == tailloc){
				if(!_next){
					if(__sync_bool_compare_and_swap(&tailloc->next,_next,_new_node)) break;
				}
			}else{
				__sync_bool_compare_and_swap(&tailloc,_tail,_next);
			}
		}
		// tracsaction ed
		__sync_bool_compare_and_swap( &tailloc,tailloc,_new_node );
		__sync_add_and_fetch( &counter, 1 );
	}

	Tvalue* pop() {
		volatile node_type*		_head_node;
		volatile node_type*		_next_node;
		Tvalue*					rtn_value;

		//transaction st
		while(1){
			_head_node = headloc;
			_next_node = headloc->next;

			if(headloc ==tailloc){
				//false
				return NULL;
			}else{
				if(__sync_bool_compare_and_swap(&headloc,_head_node,_next_node)){
					rtn_value = _next_node->value;
					__sync_lock_test_and_set(&_next_node->dummy,true);
					break;
				}
			}
		}
		//transaction ed
		while(!_head_node->dummy){
		}
		delete _head_node;
		__sync_sub_and_fetch( &counter, 1 );
		return rtn_value;
	}

	bool empty() const{
		if( tailloc == headloc ) return true;
		return false;
	}
	size_t size() const{
		return counter;
	}
};

} // namespace l7vs

#endif	// LOCKFREE_QUEUE_H
