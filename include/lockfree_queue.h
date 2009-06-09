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
		struct node_type*  volatile	next;
		node_type() : value( NULL ) , next( NULL ){}
	};
	
	unsigned int volatile	counter;
	node_type* volatile 	headloc;
	node_type* volatile		tailloc;
	node_type*				new_node;
public:
	// constractor
	lockfree_queue(){
		new_node = new node_type();
		headloc = new_node;
		tailloc = new_node;
		counter = 0;
	}

	// destractor
	~lockfree_queue(){
		Tvalue popval;
		const unsigned int delcnt = counter;
		for(int i=1;i<=delcnt;i++){
			pop(popval);
		}
		delete new_node;
	}
	
	void push(const Tvalue& value){
		node_type *_new_node,*_tail,*_next;
		_new_node = new node_type();
		_new_node->value = const_cast<Tvalue*>(&value);

		// transaction st
		while(true){
			_tail = tailloc;
			_next = _tail->next;
			if (_tail == tailloc){
				if(!_next){
					if(cas(&_tail->next,_next,_new_node)) break;
				}
			}else{
				cas(&tailloc,_tail,_next);
			}
		}
		// transaction ed

		cas(&tailloc,_tail,_new_node);
		add(&counter);
		return;
	}

	void pop(Tvalue& _value){
		node_type *_head_node,*_tail_node,*_next_node;

		//transaction st
		while(1){
			_head_node = headloc;
			_tail_node = tailloc;

			if (_head_node == headloc){
				if(_head_node ==_tail_node){
					if(_head_node->next == NULL){
						//false
						return;
					}
					_next_node = _head_node->next;
					cas(&tailloc,_tail_node,_next_node);
				}else{
					_next_node = _head_node->next;
					if(cas(&headloc,_head_node,_next_node)){
						_value = *_next_node->value;
						break;

					}
				}
			}

		}
		//transaction ed

		delete _head_node;
		sub(&counter);
		return;
	}

	bool empty(){
		if (tailloc == headloc){
			return true;
		}else{
			return false;
		}
	}
	unsigned int size(){
		return counter;
	}
};

} // namespace l7vs

#endif	// LOCKFREE_QUEUE_H
