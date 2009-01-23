#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <cstring>

#include "MessengerClient.h"

#define READBUFSIZE     (512)
#define SLEEPNANOTIME   (1000000)

void*
MessengerClient::run( void* inClass ) {
    MessengerClient* msgClPtr = static_cast<MessengerClient*>( inClass );
    char buffer[READBUFSIZE];
    struct timespec req;

    while ( !msgClPtr->endflg ) {   // async read
        req.tv_sec = 0;
        req.tv_nsec = SLEEPNANOTIME;
        int ret = nanosleep( &req, NULL );
        if ( ret == -1 ) {
            break; // nanosleep failed
        }

        if (msgClPtr->connect()) {
            size_t recvsize = recv( msgClPtr->socketfd, &buffer, READBUFSIZE, 0 );
            if (recvsize == -1) {
                if (EAGAIN != errno) {
                    // TODO error
                    msgClPtr->close();
                    msgClPtr->ipcstat = DISCONNECT;
                    continue;
                }
            }
            else if (recvsize == 0) {
                continue;
            }
            else {
                char* ptr = new char[recvsize];
                ::memcpy(ptr , buffer, recvsize);
                QueueContainer container;
                container.reset(recvsize, ptr);
                msgClPtr->messengerQueue->push(container);
            }
        }
    }
    msgClPtr->threadid = 0;
    msgClPtr->endflg = false;

    return NULL;
}

void
MessengerClient::setEndpoint( const char* targethost, const char* targetport )
{
    host = targethost;
    port = targetport;
}

bool
MessengerClient::connect()
{
    struct addrinfo     hints;
    struct addrinfo*    res = NULL;

    if (!host || !port) {
        ipcstat = DISCONNECT;
        return false;
    }
    if (ipcstat == CONNECT) return true;

    ::memset( &hints, 0, sizeof(hints) );
    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;

    int err = getaddrinfo( host, port, &hints, &res );
    if ( err != 0 ) {
        // TODO insert error log
        //char  buff[255];
        //::sprintf( buff, "%s", gai_strerror(err) );
        freeaddrinfo( res );
        ipcstat = DISCONNECT;
        return false;
    }

    for ( addrinfo* ai = res; ai; ai = ai->ai_next ) {
        socketfd = socket( ai->ai_family, ai->ai_socktype, ai->ai_protocol );
        if ( socketfd < 0 ) {
            // TODO error log
            freeaddrinfo( res );
            ipcstat = DISCONNECT;
            return false;
        }
/*
        int sockflg = fcntl(socketfd, F_GETFD);
        sockflg |= O_NONBLOCK;
        int ret = fcntl(socketfd, F_SETFD, sockflg);
        if (ret < 0) {
            // TODO error log
            ::close( socketfd );
            socketfd = -1;
            continue;
        }
*/

        // TODO must check timeout
        if ( ::connect(socketfd, ai->ai_addr, ai->ai_addrlen ) < 0 ) {
            // TODO error log
            ::close( socketfd );
            socketfd = -1;
            ipcstat = DISCONNECT;
            continue;
        }
        break;
    }

    freeaddrinfo( res );

    if ( socketfd < 0 ) {
        ipcstat = DISCONNECT;
        return false;
    }

    ipcstat = CONNECT;
    return true;
}

void
MessengerClient::close() {
    if (socketfd >= 0)
        ::close( socketfd );
    socketfd = -1;
    ipcstat = DISCONNECT;
}

int
MessengerClient::send( size_t insize, const void* data ) {
    if( socketfd < 0 )
        return -1;
    return ::send( socketfd, data, insize, MSG_NOSIGNAL );
}

void
MessengerClient::start() {
    pthread_create( &threadid, NULL, run,  this ); 
}

void
MessengerClient::stop() {
    endflg = true;
}
