#include <pthread.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <queue>
#include "lockfree_queue.h"
#include "rdtsc64.h"


l7vs::lockfree_queue< long long >    lockfree_queue_long;
std::queue<long long>            stl_queue_long;
pthread_mutex_t                mutex;


void*    thread_func_atomic( void* param ){
    unsigned long long starttime, endtime;
    RDTSC64( starttime );
    for( long long i = 0 ; i < 100; ++i ){
        lockfree_queue_long.push( &i );    
    }
    for( long long i = 0 ; i < 100; ++i ){
        long long* value = lockfree_queue_long.pop();
    }
    RDTSC64( endtime );
    std::cout << endtime - starttime << " , " << std::endl;
    return 0;
}

void*    thread_func_mutex( void* param ){
    unsigned long long starttime, endtime;
    RDTSC64( starttime );
    for( long long i = 0 ; i < 100; ++i ){
        pthread_mutex_lock( &mutex );
        stl_queue_long.push( i );
        pthread_mutex_unlock( &mutex );
    }
    for( long long i = 0 ; i < 100; ++i ){
        pthread_mutex_lock( &mutex );
        long long v = stl_queue_long.front();
        stl_queue_long.pop();
        pthread_mutex_unlock( &mutex );
    }
    RDTSC64( endtime );
    std::cout << endtime - starttime << " ," << std::endl;
    return 0;
}

int main( int argc, char* argv[] ){

    pthread_mutex_init( &mutex, NULL );
    if( argc < 2 ){
        std::cout << "usage : queue_bench [thread_num] " << std::endl;
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
