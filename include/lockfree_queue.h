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
		node_type() : value( NULL ) , next( NULL ){}
	};
	
	mutable volatile size_t	counter;
	volatile node_type*  	headloc;
	volatile node_type* 	tailloc;
	node_type*		new_node;
public:
	// constractor
	explicit lockfree_queue() : counter(0){ 
		new_node = new node_type();
		headloc = tailloc = new_node;
	}

	// destractor
	~lockfree_queue(){
		while( counter-- ){
			pop();
		}
		delete new_node;
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

		__sync_bool_compare_and_swap( &tailloc,_tail,_new_node );
		__sync_add_and_fetch( &counter, 1 );
	}

	Tvalue* pop() {
		volatile	node_type*	_head_node;
		volatile	node_type*	_tail_node;
		volatile	node_type*	_next_node;

		//transaction st
		while(1){
			_head_node = headloc;
			_tail_node = tailloc;
			
			if (_head_node == headloc){
				if(_head_node ==_tail_node){
					//false
					return NULL;
				}else{
					_next_node = headloc->next;
					if( &headloc->next != NULL ){
						if(__sync_bool_compare_and_swap(&headloc->next,_next_node,NULL)){
							if(__sync_bool_compare_and_swap(&headloc,_head_node,_next_node)){
								delete _head_node;
								__sync_sub_and_fetch( &counter, 1 );
								return _next_node->value;
							}
						}
					}
				}
			}
		
		}
		//transaction ed
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
