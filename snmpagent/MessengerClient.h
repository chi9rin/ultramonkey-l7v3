#ifndef MESSENGERCLIENT_H
#define MESSENGERCLIENT_H

#include <pthread.h>
#include "lockedqueue.h"
#include "QueueContainer.h"

enum IPC_STATUS_TAG {
    UNKNOWN = 0,
    DISCONNECT,
    CONNECT
};

typedef lockedqueue< QueueContainer > MessengerQueue;

class   MessengerClient{

protected:
    const char* host;
    const char* port;
    bool    endflg;
    int     socketfd;
    pthread_t   threadid;
    IPC_STATUS_TAG  ipcstat;
    MessengerQueue* messengerQueue;

    MessengerClient( MessengerClient& ){}
    MessengerClient& operator=( MessengerClient& ){ return *this; }
    static void*    run( void* );

public:
    MessengerClient( MessengerQueue* inqueue )
      : endflg(false),
        socketfd(-1),
        threadid(0),
        ipcstat(UNKNOWN),
        messengerQueue(inqueue) {}
    ~MessengerClient(){};

    void    setEndpoint( const char*, const char* );
    bool    connect();
    void    close();
    int     send( size_t , const void* );
    void    start();
    void    stop();
};

#endif  //MESSENGERCLIENT_H
