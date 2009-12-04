#ifndef __PROC_TRAPSENDREQUEST_H__
#define __PROC_TRAPSENDREQUEST_H__

#include "proc_message.h"

class    proc_trapsendrequest : public    proc_message
{
private:
    
public:
    proc_trapsendrequest();
    ~proc_trapsendrequest();
    
    void    do_process();
};

#endif //__PROC_TRAPSENDREQUEST_H__
