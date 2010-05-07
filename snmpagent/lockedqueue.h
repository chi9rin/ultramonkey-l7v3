#ifndef __LOCKEDQUEUE_H__
#define __LOCKEDQUEUE_H__

#include <queue>
#include <deque>
#include <pthread.h>

template < typename T, typename _Seq = std::deque<T> >
class lockedqueue;

template<typename T, typename _Seq>
class lockedqueue
{
        typedef typename _Seq::value_type   value_type;
        typedef typename _Seq::reference    reference;
        typedef typename _Seq::size_type    size_type;

private:
        pthread_mutex_t     mtx;
        std::queue<T, _Seq>  que;

protected:

public:
        lockedqueue() {
                pthread_mutex_init(&mtx, NULL);
        }
        ~lockedqueue() {}

        bool
        empty() const {
                return que.empty();
        }

        size_type
        size() const {
                return que.size();
        }

        reference
        front() {
                return que.front();
        }

        void
        push(const value_type &_data) {
                pthread_mutex_lock(&mtx);
                que.push(_data);
                pthread_mutex_unlock(&mtx);
        }

        void
        pop() {
                pthread_mutex_lock(&mtx);
                que.pop();
                pthread_mutex_unlock(&mtx);
        }

};

#endif//__LOCKEDQUEUE_H__

