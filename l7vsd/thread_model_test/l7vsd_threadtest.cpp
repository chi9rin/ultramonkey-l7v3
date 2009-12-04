#include <sys/mman.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <boost/shared_ptr.hpp>
#include <sched.h>
#include <sys/socket.h>

#include "l7vsd_threadtest.h"


int main( int argc, char* argv[] ){
    int ret = 0;
    int flg = 1;

    if( argc < 2 ){
    } else {
        flg = boost::lexical_cast<int>( argv[1] );
    }

    // signal block
//    sigset_t    newmask;
//    sigset_t    oldmask;
//    sigfillset( &newmask );
//    ret = pthread_sigmask( SIG_BLOCK, &newmask, &oldmask );
std::cout << "vsd start" <<std::endl;
    l7vs::l7vsd vsd;
    ret =  vsd.run( flg );


    // restore sigmask
//    pthread_sigmask( SIG_SETMASK, &oldmask, NULL );
    return ret;
}
