#ifndef    REPLICATION_H
#define    REPLICATION_H

#include <boost/thread.hpp>
#include <boost/asio.hpp>

namespace    l7vs{
class    replication{
public:
    enum REPLICATION_MODE_TAG{
        REPLICATION_OUT = 0,
        REPLICATION_SINGLE,
        REPLICATION_MASTER,
        REPLICATION_SLAVE,
        REPLICATION_MASTER_STOP,
        REPLICATION_SLAVE_STOP
    };

//variable
    boost::asio::io_service&                receive_io;

//function
    replication(    boost::asio::io_service& inreceive_io )
                :    receive_io( inreceive_io )
    {};

    int                            initialize();

    void                        finalize();

    void                        switch_to_master();

    void                        switch_to_slave();

    void*                        pay_memory( std::string& inid, unsigned int& outsize );
    void                        dump_memory();
    void                        start();
    void                        stop();
    void                        force_replicate();
    void                        reset();
    enum REPLICATION_MODE_TAG    get_status();
    int                            check_interval();
    int                            handle_send();
    int                            handle_receive();
    int                            lock( std::string& inid );
    int                            unlock( std::string& inid );
    int                            refer_lock_mutex( std::string& inid, boost::mutex& outmutex );
};    //class

}    //namespace    l7vs
#endif    //REPLICATION_H
