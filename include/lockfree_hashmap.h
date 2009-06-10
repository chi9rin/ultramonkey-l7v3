#ifndef	LOCKFREE_HASHMAP_H
#define	LOCKFREE_HASHMAP_H

#include <boost/functional/hash.hpp>
#include <boost/noncopyable.hpp>
#include <boost/foreach.hpp>

namespace l7vs{

template < class Tkey, class Tvalue, class Hash >
class lockfree_hashmap : boost::noncopyable{
protected:

	volatile mutable size_t	element_num;
	struct container{
		boost::shared_ptr< typename Tkey >		key;
		boost::shared_ptr< typename Tvalue >	value;
		bool	rehash;
		explicit contenor() : rehash( false ) {}
	};

	volatile mutable size_t	all_ctr;
	container*	hashmap;

public:
	typedef	Hash	hasher;

	// constractor
	lockfree_hashmap( size_t	num = 65535, inhasher = boost::hash< typename Tkey >() ) : 
		element_num( num ) {
		hashmap = new container[num];
		hasher = inhasher;
		__sync_lock_test_and_fetch( &all_ctr, 0 );
	}

	// destractor
	virtual	~lockfree_hashmap(){
		delete [] hashmap;
	}


	//inserter
	void	insert( const Tkey& key, const Tvalue& value ){
		size_t	hashvalue = get_hashvalue( key );
		Tkey*	pre_key = NULL;

		do{
			if( !find_baucket( hashvalue, key, pre_key, true ) ) return;
		}
		while( !__sync_bool_compare_and_swap( hashmap[hashvalue].key.get(), &pre_key, value ) );

		hashmap[hashvalue].key.reset( *const_cast< Tkey >( key ) );
		hashmap[hashvalue].value.reset( *const_cast< Tvalue >( value ) );
		__sync_add_and_fetch( &all_ctr, 1 );
	}

	//finder
	boost::shared_ptr< typename Tvalue >	find( const Tkey& key ){
		size_t	hashvalue = get_hashvalue( key );
		Tkey*	pre_key = NULL;
		do{
			if( find_bucket( hashvalue, key, pre_key, false ) ) return hashmap[hashvalue].value;
		}
		while( !__sync_bool_compare_and_swap( hashmap[hashvalue].key.get(), &pre_key, &key ) );
	}

	//eracer
	void	erase( const Tkey&	key ){
		size_t	hashvalue = get_hashvalue( key );
		Tkey*	pre_key = NULL;

		do{
			if( !find_bucket( hashvalue, key, pre_key, NULL ) ) return;
		}
		while( !__sync_boool_compare_and_swap( hashmap[hashvalue].key.get(), &pre_key, NULL ) );

		hashmap[hashvalue].key.reset( NULL );
		hashmap[hashvalue].value.reset( NULL );
		__sync_sub_and_fetch( &all_ctr, 1 );
	}

	//bucket finder
	bool	find_bucket( size_t& hash_value, const Tkey& key, Tkey*& pre_key, bool insert ){
		for(;;){
			if( hashmap[hashvalue].key.get() == NULL ){
				if( insert ) return true;
				( hashmap[hashvalue].rehash ) ? hashvalue = rehash_value( hashvalue ) : return false;
			}
			else if( *(hashmap[hashvalue].key.get()) == key ) {
				pre_key = hashmap[hashvalue].key.get();
				return true;
			}
			else{
				if( insert ){
					hashmap[hashvalue].rehash = true;
					hashvalue = re_hashvalue( hashvalue );
				}
				else{
					if( !hashmap[hashvalue].rehash ) return false;
					hashvalue = re_hashvalue( hashvalue );
				}
			}
		}
	}

	//poper
	void	pop( boost::shared_ptr< typename Tkey >& key, boost::shared_ptr< typename Tvalue >& value ){
		for( size_t i = 0 ; i < element_num; ++i ){
			if( hashmap[i].key.get() )
				key.reset( hashmap[i].key.get() );
				value.reset( hashmap[i].value.get() );
				hashmap[i].key.reset( NULL );
				hashmap[i].value.reset( NULL );
				__sync_sub_and_fetch( &all_ctr, 1 );
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
	void	do_all( boost::function< void, typename Tvalue* >	func ){
		for( size_t	i = 0; i < element_num; ++i ){
			if( hashmap[i].key.get() != NULL ){
				func( hashmap[i].value.get() );
			}
		}
	}
protected:
	size_t	get_hashvalue( const Tkey& key ){
		return hasher()( key ) % element_num;
	}

	size_t	re_hashvalue( size_t& key ){
		boost::hash< size_t >	boosthash;
		return boosthash( key+1 ) % element_num;
	}
};



}	//namespace l7vs
#endif	//LOCKFREE_MAP_H