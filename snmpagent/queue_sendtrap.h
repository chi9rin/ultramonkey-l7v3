#ifndef __QUEUE_SENDTRAP_H__
#define __QUEUE_SENDTRAP_H__

#include <string>

class    l7ag_queuesendtrap
{
private:
    void    set_queuedata();

public:
    l7ag_queuesendtrap();
    ~l7ag_queuesendtrap();

    void    get_queuedata();
    void    make_queuedata( std::string oid, std::string trapmessage );
};

#endif //__QUEUE_SENDTRAP_H__
