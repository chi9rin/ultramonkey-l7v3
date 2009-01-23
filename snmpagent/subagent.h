#ifndef __SUBAGENT_H__
#define __SUBAGENT_H__

#include "l7ag_parameter.h"
#include "lockedqueue.h"
#include "QueueContainer.h"
#include "tag_threadstat.h"         // TODO no need? set only STOP. no read.
#include "store_mibdata.h"
#include "MessengerClient.h"
#include "trapdata.h"
#include "dispatcher.h"

class l7ag_subagent
{
private:
    l7ag_parameter snmpparam;
    std::map<std::string,l7vs::LOG_LEVEL_TAG> levelstring_map;

    bool            stop_flag;
    THREAD_STAT_TAG stat;           // TODO no need? set only STOP. no read.

    MessageQueue    msg_queue;
    TrapQueue       trap_queue;

    MessengerClient*    messenger;  // IPC class
    l7ag_dispatcher*    dispatcher; // Dispatch class

    void    reload_config();
    void    load_loglevel();
    void    loglevelmap_init();

public:
    l7ag_subagent();
    ~l7ag_subagent();

    bool    start();
    void    stop();

    l7ag_parameter* get_parameter(){ return &snmpparam; }
};

#endif //__SUBAGENT_H__
