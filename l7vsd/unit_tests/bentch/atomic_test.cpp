#include <pthread.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <vector>
#include "../../include/atomic.h"
#include "rdtsc64.h"


typedef    l7vs::atomic< unsigned long long >    atomic_ulong_t;
atomic_ulong_t    ulong_value;

pthread_mutex_t     mutex;
unsigned long long lock_ulong_value;

void*    thread_func_atomic( void* param ){
    unsigned long long starttime, endtime;
    RDTSC64( starttime );
    for( int i = 0 ; i < 10000; ++i ){
        ulong_value -= 1000;
        ulong_value -= 1000;
    }
    RDTSC64( endtime );
    std::cout << endtime - starttime << std::endl;
    return 0;
}

void*    thread_func_mutex( void* param ){
    unsigned long long starttime, endtime;
    RDTSC64( starttime );
    for( int i = 0 ; i < 10000; ++i ){
        pthread_mutex_lock( &mutex );
        lock_ulong_value += 1000;
        lock_ulong_value -= 1000;
        pthread_mutex_unlock( &mutex );
    }
    RDTSC64( endtime );
    std::cout << endtime - starttime << std::endl;
    return 0;
}

int main( int argc, char* argv[] ){
    ulong_value = 0;
    lock_ulong_value = 0;
    pthread_mutex_init( &mutex, NULL );
    if( argc < 2 ){
        std::cout << "usage : a.out [thread_num] " << std::endl;
        return 0;
    }

    int count = boost::lexical_cast<int>( argv[1] );
    std::vector<pthread_t>    thread_vec;
    pthread_t    thd;

    //atomic version
    std::cout << "atomic func time start" << std::endl;
    for( int i = 0; i < count; ++i ){
        pthread_create( &thd, NULL, thread_func_atomic, NULL );
        thread_vec.push_back( thd );
    }
    for( std::vector<pthread_t>::iterator itr = thread_vec.begin();
         itr != thread_vec.end();
         ++itr ){
        pthread_join( *itr, NULL );
    }
    thread_vec.clear();

    //mutex version
    std::cout << "mutex func time start" << std::endl;
    for( int i = 0; i < count; ++i ){
        pthread_create( &thd, NULL, thread_func_mutex, NULL );
        thread_vec.push_back( thd );
    }
    for( std::vector<pthread_t>::iterator itr = thread_vec.begin();
         itr != thread_vec.end();
         ++itr ){
        pthread_join( *itr, NULL );
    }

    pthread_mutex_destroy( &mutex );

    return 0;
}
