#ifndef __PROC_MIBCOLLECTMESSAGE_H__
#define __PROC_MIBCOLLECTMESSAGE_H__

#include "proc_message.h"

class    proc_mibcollectmessage : public    proc_message
{
private:

public:
        proc_mibcollectmessage();
        ~proc_mibcollectmessage();

        void    do_process();
};

#endif //__PROC_MIBCOLLECTMESSAGE_H__
