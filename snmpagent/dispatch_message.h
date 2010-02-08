#ifndef __L7AG_DISPATCH_MESSAGE_H__
#define __L7AG_DISPATCH_MESSAGE_H__

#include <string>

#include "proc_mibcollectmessage.h"
#include "proc_trapsendrequest.h"
#include "proc_settingrequest.h"


class    l7ag_dispatch_message
{
    protected:

    private:
        std::string    message;
    public:
        l7ag_dispatch_message();
        ~l7ag_dispatch_message();

        void    add_message( std::string in_msg );
        void    do_dispatch();
};

#endif //__L7AG_DISPATCH_MESSAGE_H__
