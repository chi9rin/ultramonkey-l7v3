#ifndef    LOCKFREE_HASHMAP_H
#define    LOCKFREE_HASHMAP_H
#include <boost/functional/hash.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "utility.h"

namespace l7vs
{

//Argument Tkey, its size must be 64bit or less.
template < class Tkey, class Tvalue, class Hash = boost::hash< const Tkey * > >
class lockfree_hashmap : boost::noncopyable
{
protected:

        const                 size_t    element_num;
        volatile mutable     size_t    counter;

        struct container {
                Tkey            *key;
                Tvalue            *value;
                bool            rehash;
                container() : key(NULL), value(NULL) , rehash(false) {}
        };

        volatile container    *hashmap;

public:
        typedef    Hash    hasher_type;
        hasher_type        hasher;

        // constractor
        lockfree_hashmap(size_t num = 65535, Hash inhasher = boost::hash< const Tkey * >()) :
                element_num(num) {
                hashmap = new container[element_num];
                hasher = inhasher;
                __sync_lock_test_and_set(&counter, 0);
        }

        // destractor
        virtual    ~lockfree_hashmap() {
                delete [] hashmap;
        }

        //inserter
        void    insert(const Tkey *key, const Tvalue *value) {
                size_t    hashvalue = get_hashvalue(key);
                Tkey    *pre_key = NULL;
                //transaction st
                do {
                        for (;;) {
                                if (likely(!hashmap[hashvalue].key)) break;
                                if (unlikely(hashmap[hashvalue].key == key)) {
                                        pre_key = hashmap[hashvalue].key;
                                        break;
                                } else {
                                        hashmap[hashvalue].rehash = true;
                                        hashvalue = get_rehashvalue(hashvalue);
                                }
                        }
                } while (unlikely(!__sync_bool_compare_and_swap(&hashmap[hashvalue].key, pre_key, key)));
                //transaction ed
                if (__sync_lock_test_and_set(&hashmap[hashvalue].value, value)) {};
                if (__sync_add_and_fetch(&counter, 1)) {};
        }

        //finder
        Tvalue    *find(const Tkey *key) {
                size_t    hashvalue = get_hashvalue(key);
                for (;;) {
                        if (likely(hashmap[hashvalue].key == key)) {
                                return hashmap[hashvalue].value;
                        } else {
                                if (likely(!hashmap[hashvalue].rehash)) return NULL;
                                hashvalue = get_rehashvalue(hashvalue);
                        }
                }
        }

        //eracer
        void    erase(const Tkey    *key) {
                size_t    hashvalue = get_hashvalue(key);
                for (;;) {
                        if (likely(hashmap[hashvalue].key == key)) {
                                if (__sync_lock_test_and_set(&hashmap[hashvalue].key, NULL)) {};
                                if (__sync_lock_test_and_set(&hashmap[hashvalue].value, NULL)) {};
                                __sync_sub_and_fetch(&counter, 1);
                                return;
                        } else {
                                if (likely(!hashmap[hashvalue].rehash)) return;
                                hashvalue = get_rehashvalue(hashvalue);
                        }
                }
        }

        //clear
        void    clear() {
                delete [] hashmap;
                hashmap = new container[element_num];
                __sync_lock_test_and_set(&counter, 0);
        }

        //poper
        void    pop(Tkey*& key, Tvalue*& value) {
                for (size_t i = 0 ; likely(i < element_num) ; ++i) {
                        if (unlikely(hashmap[i].key)) {
                                key    = hashmap[i].key;
                                value    = hashmap[i].value;
                                hashmap[i].key        = NULL;
                                hashmap[i].value    = NULL;
                                __sync_sub_and_fetch(&counter, 1);
                                return;
                        }
                }
        }

        // size
        size_t    size() {
                return counter;
        }

        // empty
        bool    empty() {
                return !counter;
        }

        // functor
        void    do_all(boost::function< void(Tvalue *) >    func) {
                for (size_t    i = 0; likely(i < element_num) ; ++i) {
                        if (unlikely(hashmap[i].key)) {
                                func(hashmap[i].value);
                        }
                }
        }

private:
        //get array number by hasher
        size_t    get_hashvalue(const Tkey *key) {
                return hasher(key) % element_num;
        }
        //reget array number case of collision(method chain)
        size_t    get_rehashvalue(size_t &key) {
                return ((key + 1) % element_num);
        }
};

}    //namespace l7vs
#endif    //LOCKFREE_MAP_H
