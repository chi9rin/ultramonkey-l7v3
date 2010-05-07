#ifndef __PROC_SETTINGREQUEST_H__
#define __PROC_SETTINGREQUEST_H__

#include "proc_message.h"

class    proc_settingrequest : public    proc_message
{
private:

public:
        proc_settingrequest();
        ~proc_settingrequest();

        void    do_process();
};

#endif //__PROC_SETTINGREQUEST_H__

