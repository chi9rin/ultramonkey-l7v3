#ifndef LF_QUEUE_H
#define LF_QUEUE_H

#define cas(a,b,c) __sync_bool_compare_and_swap(a,b,c)
#define add(a) __sync_add_and_fetch(a,1)
#define sub(a) __sync_sub_and_fetch(a,1)

template<class Tvalue>
class lf_queue
{
protected:

public:

	typedef struct node_t
	{
		Tvalue *value;
		struct node_t *next;
	} node_t;

	typedef struct mng_queue_t
	{
		int counter;
		node_t *headloc;
		node_t *tailloc;
	} mng_queue_t;

	mng_queue_t *mng_queue;

	// constractor
	lf_queue(){
		node_t *new_node;
		new_node = new node_t;
		new_node->value = new Tvalue;
		mng_queue = new mng_queue_t;

		new_node->next = NULL;
		new_node->value = NULL;
		mng_queue->headloc = new_node;
		mng_queue->tailloc = new_node;
		mng_queue->counter = 0;
	}

	volatile void push(const Tvalue& _v){
		node_t *_new_node,*_tail,*_next;
		_new_node = new node_t;
		_new_node->value = new Tvalue;
		*_new_node->value = _v;
		_new_node->next = NULL;

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

	Tvalue pop(){
		node_t *_head_node,*_tail_node,*_next_node;
		Tvalue *_tvalue;
		_tvalue = new Tvalue;

		//transaction st
		while(1){
			_head_node = mng_queue->headloc;
			_tail_node = mng_queue->tailloc;

			if (_head_node == mng_queue->headloc){
				if(_head_node ==_tail_node){
					if(_head_node->next == NULL){
						//false
						return *_tvalue;
					}
					_next_node = _head_node->next;
					cas(&mng_queue->tailloc,_tail_node,_next_node);
				}else{
					_next_node = _head_node->next;
					if(cas(&mng_queue->headloc,_head_node,_next_node)){
						*_tvalue = *_next_node->value;
						break;

					}
				}
			}

		}
		//transaction ed

		delete _head_node->value;
		delete _head_node;
		sub(&mng_queue->counter);
		return *_tvalue;
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
#endif

