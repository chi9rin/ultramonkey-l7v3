#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <boost/thread.hpp>

class thread_pool {
public:
	template <typename F>
	void schedule(F threadfunc) {
		tg_->create_thread(threadfunc);
	}

	void wait() {
		tg_->join_all();
	}

	thread_pool() :
		tg_(new boost::thread_group) {}


protected:
	boost::shared_ptr<boost::thread_group> tg_;
};

#endif	//__THREAD_POOL_H__
