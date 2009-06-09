#ifndef LOCKFREE_QUEUE_H
#define LOCFFREE_QUEUE_H

#include <boost/noncopyable.hpp>

namespace l7vs{

template<class Tvalue>
class lockfree_queue : protected boost::noncopyable {
protected:
	struct node_type {
		Tvalue*					value;
		volatile node_type*  	next;
		node_type() : value( NULL ) , next( NULL ){}
	};
	
	volatile size_t		 			counter;
	volatile node_type* volatile 	headloc;
	volatile node_type* volatile	tailloc;
	node_type*			new_node;
public:
	// constractor
	lockfree_queue() : counter(0){ 
		new_node = new node_type();
		headloc = tailloc = new_node;
	}

	// destractor
	~lockfree_queue(){
		while( counter-- ){
			Tvalue* popval;
			pop(popval);
		}
		delete new_node;
	}
	
	void push(const Tvalue* value){
		volatile	node_type* _new_node	= new node_type();
		volatile	node_type* _tail;
		volatile	node_type* _next;

		_new_node->value = const_cast<Tvalue*>( value );

		// transaction st
		while(true){
			_tail = tailloc;
			_next = _tail->next;

			if( _tail == tailloc )
				if( !_next && __sync_bool_compare_and_swap( &_tail->next, _next, _new_node ) ) break;
			else
				__sync_bool_compare_and_swap( &tailloc, _tail, _next );
		}
		// transaction ed

		__sync_bool_compare_and_swap( &tailloc,_tail,_new_node );
		__sync_add_and_fetch( &counter, 1 );
	}

	void pop(Tvalue*& (_value) ){
		volatile	node_type*	_head_node;
		volatile	node_type*	_tail_node;
		volatile	node_type*	_next_node;

		//transaction st
		while(true){
			_head_node = headloc;
			_tail_node = tailloc;

			if (_head_node == headloc){
				if( _head_node ==_tail_node ){
					if(_head_node->next == NULL){
						return;
					}
					_next_node = _head_node->next;
					__sync_bool_compare_and_swap(&tailloc,_tail_node,_next_node);
				}
				else{
					_next_node = _head_node->next;
					if( __sync_bool_compare_and_swap(&headloc,_head_node,_next_node )){
						_value = _next_node->value;
						break;
					}
				}
			}
		}
		//transaction ed

		delete _head_node;
		__sync_sub_and_fetch( &counter, 1 );
		return;
	}

	bool empty(){
		if( tailloc == headloc ) return true;
		return false;
	}
	size_t size(){
		return counter;
	}
};

} // namespace l7vs

#endif	// LOCKFREE_QUEUE_H
