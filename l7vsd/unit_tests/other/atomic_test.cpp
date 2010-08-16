#include <pthread.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
//#include <boost/test/unit_test.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>

#include <vector>
#include "../../include/atomic.h"
#include "rdtsc64.h"

//  このテストコードでは、Atomicとwrlock.の確認を同時に実施します。

#define CHECK(a,b,c,d)	do { \
				if (a != b) { \
					std::cout << "error " << c << ": "; \
					std::cout << a << " != " << b; \
					std::cout << ", line:" << d <<std::endl; \
				} \
			} while (0)

typedef    l7vs::atomic< int >    atomic_int_t;
atomic_int_t    int_value;

typedef    l7vs::atomic< long long >    atomic_long_t;
atomic_long_t    long_value;

typedef    l7vs::atomic< unsigned long long >    atomic_ulong_t;
atomic_ulong_t    ulong_value;

unsigned long long lock_ulong_value;

void*    thread_func_atomic_i( void* param ){
    int temp;
   
//    ++int_value;
//    --int_value;
    int_value = 0;
    temp = int_value.get();
//std::cout << "int_value=(:0):" << temp << std::endl;
    CHECK(0, temp, "get", __LINE__);
//    BOOST_CHECK( 0 == temp );
    int_value++;
    temp = int_value.get();
//std::cout << "int_value++(:1):" << temp << std::endl;
//    BOOST_CHECK( 1 == temp );
    CHECK(1, temp, "++", __LINE__);
    int_value--;
    temp = int_value.get();
//std::cout << "int_value--(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
    CHECK(0, temp, "--", __LINE__);
    int_value += 1000;
    temp = int_value.get();
//std::cout << "int_value+=(:999):" << temp << std::endl;
//    BOOST_CHECK( 1000 == temp );
    CHECK(1000, temp, "+=", __LINE__);
    int_value -= 1000;
    temp = int_value.get();
//std::cout << "int_value-=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
    CHECK(0, temp, "-=", __LINE__);
    int_value      += 1000;
//std::cout << "int_value< (:0)"  << (int_value< 1000) << std::endl;
//    BOOST_CHECK( 0 == (int_value< 1000) );
    CHECK(0, (int_value< 1000), "<", __LINE__);
//std::cout << "int_value<=(:1)" << (int_value<=1000) << std::endl;
//    BOOST_CHECK( 1 == (int_value<=1000) );
    CHECK(1, (int_value<=1000), "<=", __LINE__);
//std::cout << "int_value> (:0)"  << (int_value> 1000) << std::endl;
//    BOOST_CHECK( 0 == (int_value> 1000) );
    CHECK(0, (int_value> 1000), ">", __LINE__);
//std::cout << "int_value>=(:1)" << (int_value>=1000) << std::endl;
//    BOOST_CHECK( 1 == (int_value>=1000) );
    CHECK(1, (int_value>=1000), ">=", __LINE__);
//std::cout << "int_value==(:1)" << (int_value==1000) << std::endl;
//    BOOST_CHECK( 1 == (int_value==1000) );
    CHECK(1, (int_value==1000), "==", __LINE__);
//std::cout << "int_value!==(:0)" << (int_value!=1000) << std::endl;
//    BOOST_CHECK( 0 == (int_value!=1000) );
    CHECK(0, (int_value!=1000), "!=", __LINE__);
//std::cout << "int_value< (:0)"  << (int_value< 999) << std::endl;
//    BOOST_CHECK( 0 == (int_value< 999) );
    CHECK(0, (int_value< 999), "<", __LINE__);
//std::cout << "int_value<=(:0)" << (int_value<=999) << std::endl;
//    BOOST_CHECK( 0 == (int_value<=999) );
    CHECK(0, (int_value<=999), "<=", __LINE__);
//std::cout << "int_value> (:1)"  << (int_value> 999) << std::endl;
//    BOOST_CHECK( 1 == (int_value> 999) );
    CHECK(1, (int_value> 999), ">", __LINE__);
//std::cout << "int_value>=(:1)" << (int_value>=999) << std::endl;
//    BOOST_CHECK( 1 == (int_value>=999) );
    CHECK(1, (int_value>=999), ">=", __LINE__);
//std::cout << "int_value==(:0)" << (int_value==999) << std::endl;
//    BOOST_CHECK( 0 == (int_value==999) );
    CHECK(0, (int_value==999), "==", __LINE__);
//std::cout << "int_value!==(:1)" << (int_value!=999) << std::endl;
//    BOOST_CHECK( 1 == (int_value!=999) );
    CHECK(1, (int_value!=999), "!=", __LINE__);
//std::cout << "int_value< (:1)"  << (int_value <1001) << std::endl;
//    BOOST_CHECK( 1 == (int_value <1001) );
    CHECK(1, (int_value <1001), "<", __LINE__);
//std::cout << "int_value<=(:1)" << (int_value<=1001) << std::endl;
//    BOOST_CHECK( 0 == (int_value<=1001) );
    CHECK(1, (int_value<=1001), "<=", __LINE__);
//std::cout << "int_value> (:0)"  << (int_value >1001) << std::endl;
//    BOOST_CHECK( 0 == (int_value >1001) );
    CHECK(0, (int_value >1001), ">", __LINE__);
//std::cout << "int_value>=(:0)" << (int_value>=1001) << std::endl;
//    BOOST_CHECK( 0 == (int_value>=1001) );
    CHECK(0, (int_value>=1001), ">=", __LINE__);
//std::cout << "int_value==(:0)" << (int_value==1001) << std::endl;
//    BOOST_CHECK( 0 == (int_value==1001) );
    CHECK(0, (int_value==1001), "==", __LINE__);
//std::cout << "int_value!==(:1)" << (int_value!=1001) << std::endl;
//    BOOST_CHECK( 1 == (int_value!=1001) );
    CHECK(1, (int_value!=1001), "!=", __LINE__);
    return 0;
}

void*    thread_func_atomic_ll( void* param ){
    long long temp;
        
//    ++long_value;
//    --long_value;
    long_value = 0;
    temp = long_value.get();
//std::cout << "long_value=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
    CHECK(0, temp, "get", __LINE__);
    long_value++;
    temp = long_value.get();
//std::cout << "long_value++(:1):" << temp << std::endl;
//    BOOST_CHECK( 1 == temp );
    CHECK(1, temp, "++", __LINE__);
    long_value--;
    temp = long_value.get();
//std::cout << "long_value--(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
    CHECK(0, temp, "--", __LINE__);
    long_value += 1000;
    temp = long_value.get();
//std::cout << "long_value+=(:1000):" << temp << std::endl;
//    BOOST_CHECK( 1000 == temp );
    CHECK(1000, temp, "+=", __LINE__);
    long_value -= 1000;
    temp = long_value.get();
//std::cout << "long_value-=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
    CHECK(0, temp, "-=", __LINE__);
    long_value      += 1000;
//std::cout << "long_value< (:0)"  << (long_value< 1000) << std::endl;
//    BOOST_CHECK( 0 == (long_value< 1000) );
    CHECK(0, (long_value< 1000), "<", __LINE__);
//std::cout << "long_value<=(:1)" << (long_value<=1000) << std::endl;
//    BOOST_CHECK( 1 == (long_value<=1000) );
    CHECK(1, (long_value<= 1000), "<=", __LINE__);
//std::cout << "long_value> (:0)"  << (long_value> 1000) << std::endl;
//    BOOST_CHECK( 0 == (long_value> 1000) );
    CHECK(0, (long_value> 1000), ">", __LINE__);
//std::cout << "long_value>=(:1)" << (long_value>=1000) << std::endl;
//    BOOST_CHECK( 1 == (long_value>=1000) );
    CHECK(1, (long_value>= 1000), ">=", __LINE__);
//std::cout << "long_value==(:1)" << (long_value==1000) << std::endl;
//    BOOST_CHECK( 1 == (long_value==1000) );
    CHECK(1, (long_value== 1000), "==", __LINE__);
//std::cout << "long_value!==(:0)" << (long_value!=1000) << std::endl;
//    BOOST_CHECK( 0 == (long_value!=1000) );
    CHECK(0, (long_value!= 1000), "!=", __LINE__);
//std::cout << "long_value< (:0)"  << (long_value< 999) << std::endl;
//    BOOST_CHECK( 0 == (long_value< 999) );
    CHECK(0, (long_value< 999), "<", __LINE__);
//std::cout << "long_value<=(:0)" << (long_value<=999) << std::endl;
//    BOOST_CHECK( 0 == (long_value<=999) );
    CHECK(0, (long_value<= 999), "<=", __LINE__);
//std::cout << "long_value> (:1)"  << (long_value> 999) << std::endl;
//    BOOST_CHECK( 1 == (long_value> 999) );
    CHECK(1, (long_value> 999), ">", __LINE__);
//std::cout << "long_value>=(:1)" << (long_value>=999) << std::endl;
//    BOOST_CHECK( 1 == (long_value>=999) );
    CHECK(1, (long_value>= 999), ">=", __LINE__);
//std::cout << "long_value==(:0)" << (long_value==999) << std::endl;
//    BOOST_CHECK( 0 == (long_value==999) );
    CHECK(0, (long_value== 999), "==", __LINE__);
//std::cout << "long_value!==(:1)" << (long_value!=999) << std::endl;
//    BOOST_CHECK( 1 == (long_value!=999) );
    CHECK(1, (long_value!= 999), "!=", __LINE__);
//std::cout << "long_value< (:1)"  << (long_value <1001) << std::endl;
//    BOOST_CHECK( 1 == (long_value <1001) );
    CHECK(1, (long_value< 1001), "<", __LINE__);
//std::cout << "long_value<=(:1)" << (long_value<=1001) << std::endl;
//    BOOST_CHECK( 1 == (long_value<=1001) );
    CHECK(1, (long_value<= 1001), "<=", __LINE__);
//std::cout << "long_value> (:0)"  << (long_value >1001) << std::endl;
//    BOOST_CHECK( 0 == (long_value >1001) );
    CHECK(0, (long_value> 1001), ">", __LINE__);
//std::cout << "long_value>=(:0)" << (long_value>=1001) << std::endl;
//    BOOST_CHECK( 0 == (long_value>=1001) );
    CHECK(0, (long_value>= 1001), ">=", __LINE__);
//std::cout << "long_value==(:0)" << (long_value==1001) << std::endl;
//    BOOST_CHECK( 0 == (long_value==1001) );
    CHECK(0, (long_value== 1001), "==", __LINE__);
//std::cout << "long_value!==(:1)" << (long_value!=1001) << std::endl;
//    BOOST_CHECK( 1 == (long_value!=1001) );
    CHECK(1, (long_value!= 1001), "!=", __LINE__);
    return 0;
}

void*    thread_func_atomic_ull( void* param ){
    unsigned long long temp;
        
//    ++ulong_value;
//    --ulong_value;
    ulong_value = 0;
    temp = ulong_value.get();
//std::cout << "ulong_value=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
    CHECK(0, temp, "get", __LINE__);
    ulong_value++;
    temp = ulong_value.get();
//std::cout << "ulong_value++(:1):" << temp << std::endl;
//    BOOST_CHECK( 1 == temp );
    CHECK(1, temp, "++", __LINE__);
    ulong_value--;
    temp = ulong_value.get();
//std::cout << "ulong_value--(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
    CHECK(0, temp, "--", __LINE__);
    ulong_value += 1000;
    temp = ulong_value.get();
//std::cout << "ulong_value+=(:1000):" << temp << std::endl;
//    BOOST_CHECK( 1000 == temp );
    CHECK(1000, temp, "+=", __LINE__);
    ulong_value -= 1000;
    temp = ulong_value.get();
//std::cout << "ulong_value-=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
    CHECK(0, temp, "-=", __LINE__);
    ulong_value      += 1000;
//std::cout << "ulong_value< (:0)"  << (ulong_value< 1000) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value< 1000) );
    CHECK(0, (ulong_value< 1000), "<", __LINE__);
//std::cout << "ulong_value<=(:1)" << (ulong_value<=1000) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value<=1000) );
    CHECK(1, (ulong_value<= 1000), "<=", __LINE__);
//std::cout << "ulong_value> (:0)"  << (ulong_value> 1000) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value> 1000) );
    CHECK(0, (ulong_value> 1000), ">", __LINE__);
//std::cout << "ulong_value>=(:1)" << (ulong_value>=1000) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value>=1000) );
    CHECK(1, (ulong_value>= 1000), ">=", __LINE__);
//std::cout << "ulong_value==(:1)" << (ulong_value==1000) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value==1000) );
    CHECK(1, (ulong_value== 1000), "==", __LINE__);
//std::cout << "ulong_valu!==(:0)" << (ulong_value!=1000) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value!=1000) );
    CHECK(0, (ulong_value!= 1000), "!=", __LINE__);
//std::cout << "ulong_value< (:0)"  << (ulong_value< 999) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value< 999) );
    CHECK(0, (ulong_value< 999), "<", __LINE__);
//std::cout << "ulong_value<=(:0)" << (ulong_value<=999) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value<=999) );
    CHECK(0, (ulong_value<= 999), "<=", __LINE__);
//std::cout << "ulong_value> (:1)"  << (ulong_value> 999) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value> 999) );
    CHECK(1, (ulong_value> 999), ">", __LINE__);
//std::cout << "ulong_value>=(:1)" << (ulong_value>=999) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value>=999) );
    CHECK(1, (ulong_value>= 999), ">=", __LINE__);
//std::cout << "ulong_value==(:0)" << (ulong_value==999) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value==999) );
    CHECK(0, (ulong_value== 999), "==", __LINE__);
//std::cout << "ulong_valu!==(:1)" << (ulong_value!=999) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value!=999) );
    CHECK(1, (ulong_value!= 999), "!=", __LINE__);
//std::cout << "ulong_value< (:1)"  << (ulong_value <1001) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value <1001) );
    CHECK(1, (ulong_value< 1001), "<", __LINE__);
//std::cout << "ulong_value<=(:1)" << (ulong_value<=1001) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value<=1001) );
    CHECK(1, (ulong_value<= 1001), "<=", __LINE__);
//std::cout << "ulong_value> (:0)"  << (ulong_value >1001) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value >1001) );
    CHECK(0, (ulong_value> 1001), ">", __LINE__);
//std::cout << "ulong_value>=(:0)" << (ulong_value>=1001) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value>=1001) );
    CHECK(0, (ulong_value>= 1001), ">=", __LINE__);
//std::cout << "ulong_value==(:0)" << (ulong_value==1001) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value==1001) );
    CHECK(0, (ulong_value== 1001), "==", __LINE__);
//std::cout << "ulong_valu!==(:1)" << (ulong_value!=1001) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value!=1001) );
    CHECK(1, (ulong_value!= 1001), "!=", __LINE__);
    return 0;
}

int main( int argc, char* argv[] ){
    ulong_value = 0;
    lock_ulong_value = 0;
    if( 0 ){
        std::cout << "usage : a.out [thread_num] " << std::endl;
        return 0;
    }

    //int count = boost::lexical_cast<int>( argv[1] );
    int count = boost::lexical_cast<int>( "3" );
    std::vector<pthread_t>    thread_vec;
    pthread_t    thd[3];

    //atomic version
    std::cout << "atomic func time start" << std::endl;
    for( int i = 0; i < count; ++i ){
        pthread_create( &thd[0], NULL, thread_func_atomic_i, NULL );
        pthread_create( &thd[1], NULL, thread_func_atomic_ll, NULL );
        pthread_create( &thd[2], NULL, thread_func_atomic_ull, NULL );
    }
    thread_vec.push_back( thd[0] );
    thread_vec.push_back( thd[1] );
    thread_vec.push_back( thd[2] );

    int retval = 0;
    for( std::vector<pthread_t>::iterator itr = thread_vec.begin();
         itr != thread_vec.end();
         ++itr ){
        int ret;
        pthread_join( *itr, (void**)&ret);
        retval |= ret;
    }
    thread_vec.clear();

    return retval;
}
