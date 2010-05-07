
#include "interproccom.h"
#include "subagent.h"


#define SCOPED_LOCK(m)                                                  \
  pthread_mutex_lock(&m); /* エラー処理は略 */                          \
  void u_(pthread_mutex_t** m_) { pthread_mutex_unlock(*m_); }          \
  __attribute__((__cleanup__(u_))) pthread_mutex_t* scoped_lock_ = &m


/*!
 *
 */
l7ag_interproccommunicate::l7ag_interproccommunicate()
{
        status    = STOP;
        stop_flag = false;

        pthread_mutex_init(&mutex_sbuf, NULL);
        pthread_mutex_init(&mutex_rbuf, NULL);
}


/*!
 *
 */
l7ag_interproccommunicate::l7ag_interproccommunicate(l7ag_subagent *p_subagent)
{
        status    = STOP;
        stop_flag = false;

        pthread_mutex_init(&mutex_sbuf, NULL);
        pthread_mutex_init(&mutex_rbuf, NULL);

        set_parent(p_subagent);
}


/*!
 *
 */
l7ag_interproccommunicate::~l7ag_interproccommunicate()
{


}


/*!
 *
 */
void
l7ag_interproccommunicate::start_thread()
{


}


/*!
 *
 */
void
l7ag_interproccommunicate::send_mibdatacollect(std::string oid)
{
        //make request message

}


/*!
 *
 */
void
l7ag_interproccommunicate::set_parent(l7ag_subagent *p_subagent)
{
        parent = p_subagent;
}


/*!
 *
 */
void *
l7ag_interproccommunicate::do_communicate(void *args)
{
        l7ag_interproccommunicate    *proccom = (l7ag_interproccommunicate *)args;
        //initialize

        while (1) {
                //recv
                //send
                //stop
                if (proccom->getStopFlag()) {
                        //
                }
        }
        //finalize
}


/*!
 *
 */
void
l7ag_interproccommunicate::setRequestMessage()
{
}

