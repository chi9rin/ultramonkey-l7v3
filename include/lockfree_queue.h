#ifndef LOCKFREE_QUEUE_H
#define LOCFFREE_QUEUE_H

#include <boost/noncopyable.hpp>

#define cas(a,b,c) __sync_bool_compare_and_swap(a,b,c)
#define add(a) __sync_add_and_fetch(a,1)
#define sub(a) __sync_sub_and_fetch(a,1)

namespace l7vs{

template<class Tvalue>
class lockfree_queue : protected boost::noncopyable {
protected:
	struct node_type {
		Tvalue*				value;
		struct node_type*	next;
		node_type() : value( NULL ) , next( NULL ){}
	};
	struct mng_queue_type {
		int 			counter;
		node_type*		headloc;
		node_type*		tailloc;
		mng_queue_type() : counter(0), headloc( NULL ), tailloc( NULL ) {}
	};
public:
	mng_queue_type*		mng_queue;

	// constractor
	lockfree_queue(){
		node_type*	new_node = new node_type();
		new_node->value = new Tvalue();
		mng_queue = new mng_queue_type();
		mng_queue->headloc = new_node;
		mng_queue->tailloc = new_node;
	}

	volatile void push(const Tvalue& _v){
		node_type *_new_node,*_tail,*_next;
		_new_node = new node_type();
		_new_node->value = new Tvalue;
		*_new_node->value = _v;

		// tracsaction st
		while(1){
			_tail = mng_queue->tailloc;
			_next = _tail->next;

			if(_tail == mng_queue->tailloc){
				if(_next == NULL){
					if(cas(&_tail->next,_next,_new_node)){
						break;
					}
				}
			}else{
				cas(&mng_queue->tailloc,_tail,_next);
			}

		}
		// tracsaction ed

		cas(&mng_queue->tailloc,_tail,_new_node);
		add(&mng_queue->counter);
		return;
	}

	void pop(Tvalue& _value){
		node_type *_head_node,*_tail_node,*_next_node;

		//transaction st
		while(1){
			_head_node = mng_queue->headloc;
			_tail_node = mng_queue->tailloc;

			if (_head_node == mng_queue->headloc){
				if(_head_node ==_tail_node){
					if(_head_node->next == NULL){
						//false
						return;
					}
					_next_node = _head_node->next;
					cas(&mng_queue->tailloc,_tail_node,_next_node);
				}else{
					_next_node = _head_node->next;
					if(cas(&mng_queue->headloc,_head_node,_next_node)){
						_value = *_next_node->value;
						break;

					}
				}
			}

		}
		//transaction ed

		delete _head_node->value;
		delete _head_node;
		sub(&mng_queue->counter);
		return;
	}

	bool empty(){
		if (mng_queue->tailloc == mng_queue->headloc){
			return true;
		}else{
			return false;
		}
	}
	int size(){
		return mng_queue->counter;
	}
};

} // namespace l7vs

#endif	// LOCKFREE_QUEUE_H

