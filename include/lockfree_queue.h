#ifndef LOCKFREE_QUEUE_H
#define LOCKFREE_QUEUE_H

#include <boost/noncopyable.hpp>

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
	size_t					element_num;
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

	bool push(const Tvalue* value){
		size_t tail,nexttail;
		//transaction st
		while(true){
			tail = tailloc;
			nexttail = get_num_next(tail);
			if ( node[tail].value) {
				return false;
			}
			if ( __sync_bool_compare_and_swap(&tailloc,tail,nexttail) ) break;
		}
		//transaction ed
		node[tail].value = const_cast<Tvalue*>( value );
		__sync_add_and_fetch( &counter, 1 );
		return true;
	}

	Tvalue* pop() {
		size_t head,nexthead;
		Tvalue* rtnvalue;
		//transaction st
		while(true){
			head = headloc;
			nexthead = get_num_next(head);
			if(!(node[head].value)){
				if(headloc==tailloc){
					//false
					return NULL;
				}
			}else{
				if( __sync_bool_compare_and_swap(&headloc,head,nexthead) ) {
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

	bool empty() const{
		if( counter ) return false;
		return true;
	}
	size_t size() const{
		return counter;
	}

private:
	size_t get_num_next( const size_t num ){
		if( num >= element_num ){
			return 0;
		}else{
			return num + 1;
		}
	}
};

} // namespace l7vs

#endif	// LOCKFREE_QUEUE_H
