#ifndef	LOCKFREE_HASHMAP_H
#define	LOCKFREE_HASHMAP_H

#include <boost/functional/hash.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace l7vs{

//Argument Tkey, its size must be 64bit or less.
template < class Tkey, class Tvalue, class Hash = boost::hash< Tkey > >
class lockfree_hashmap : boost::noncopyable{
protected:

	volatile mutable size_t	element_num;
	struct container{
		Tkey*		key;
		Tvalue*		value;
		bool		rehash;
		explicit container() : key ( NULL ), value( NULL ) , rehash( false ) {}
	};

	volatile mutable size_t	all_ctr;
	container*	hashmap;

public:
	typedef	Hash	hasher_type;
	hasher_type		hasher;

	// constractor
	lockfree_hashmap( size_t num = 65535, Hash inhasher = boost::hash< Tkey >() ) : 
		element_num( num ) {
		hashmap = new container[num];
		hasher = inhasher;
		__sync_lock_test_and_set( &all_ctr, 0 );
	}

	// destractor
	virtual	~lockfree_hashmap(){
		delete [] hashmap;
	}

	//inserter
	void	insert( const Tkey* key, const Tvalue* value ){
		size_t	hashvalue = get_hashvalue( *key );
		Tkey*	pre_key = NULL;
		do{
			if( !find_bucket( hashvalue, key, pre_key, true ) ) return;
		}
		while( !__sync_bool_compare_and_swap( &hashmap[hashvalue].key, pre_key, key ) );
		hashmap[hashvalue].value	= const_cast< Tvalue* >( value );
		__sync_add_and_fetch( &all_ctr, 1 );
	}

	//finder
	Tvalue*	find( const Tkey* key ){
		size_t	hashvalue = get_hashvalue( *key );
		Tkey*	pre_key = NULL;
		if( find_bucket( hashvalue, key, pre_key, false ) )
			return hashmap[hashvalue].value;
		else
			return NULL;
	}

	//eracer
	void	erase( const Tkey*	key ){
		size_t	hashvalue = get_hashvalue( *key );
		Tkey*	pre_key = NULL;
		do{
			if( !find_bucket( hashvalue, key, pre_key, false ) ) return;
		}
		while( !__sync_bool_compare_and_swap( &hashmap[hashvalue].key, pre_key, NULL ) );
		hashmap[hashvalue].value	= NULL;
		__sync_sub_and_fetch( &all_ctr, 1 );
	}

	//poper
	void	pop( Tkey*& key, Tvalue*& value ){
		for( size_t i = 0 ; i < element_num; ++i ){
			if( hashmap[i].key ){
				key	= hashmap[i].key;
				value	= hashmap[i].value;
				hashmap[i].key		= NULL;
				hashmap[i].value	= NULL;
				__sync_sub_and_fetch( &all_ctr, 1 );
				return;
			}
		}
	}

	//clear
	void	clear(){
		delete [] hashmap;
		hashmap = new container[element_num];
		__sync_lock_test_and_set( &all_ctr, 0 );
	}

	// size
	size_t	size(){ return all_ctr; }

	// empty
	bool	empty(){ return !all_ctr; }

	// functor
	void	do_all( boost::function< void( Tvalue* ) >	func ){
		for( size_t	i = 0; i < element_num; ++i ){
			if( hashmap[i].key != NULL ){
				func( hashmap[i].value );
			}
		}
	}

private:
	//bucket finder
	bool	find_bucket( size_t& hash_value, const Tkey*& key, Tkey*& pre_key, bool insert ){
		for(;;){
			if( hashmap[hash_value].key == NULL ){
				if( insert ) return true;
				if( hashmap[hash_value].rehash )
					hash_value = re_hashvalue( hash_value );
				else
					return( false );
			}
			else if( hashmap[hash_value].key == key ) {
				pre_key = hashmap[hash_value].key;
				return true;
			}
			else{
				if( insert ){
					hashmap[hash_value].rehash = true;
					hash_value = re_hashvalue( hash_value );
				}
				else{
					if( !hashmap[hash_value].rehash ) return false;
					hash_value = re_hashvalue( hash_value );
				}
			}
		}
	}
	size_t	get_hashvalue(const Tkey& key ){
		return hasher( key ) % element_num;
	}
	size_t	re_hashvalue( size_t& key ){
		return ((key+1) % element_num);
	}
};

}	//namespace l7vs
#endif	//LOCKFREE_MAP_H
