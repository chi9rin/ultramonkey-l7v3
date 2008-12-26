#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <boost/thread.hpp>

class thread_pool {
protected:
	typedef std::map<boost::thread::id, tcp_session*> session_map_type;

public:
	template <typename F>
	void schedule(tcp_session& sess) {
		sess.initialize();

		tg_->create_thread(sess.down_thread_run());
		tg_->create_thread(sess.up_thread_run());

		session_map.add(sess.get_down_thread_id(), &sess);
		
	}

	//virtual_serviceが終了するときにsessionをすべて終了させる関数
	void stop_threads() {
		for (session_map::iterator itr = session_map.begin(); itr != session_map.end(); ++itr) {
			itr->second->finalize();
		}
		tg_->join_all();
	}

	//sessionが終了するときに呼ぶ関数
	void end_session(boost::thread::id id) {
		vs.end_session(session_map[id]->second);
	}

	thread_pool(virtual_service& vs) :
		vs_(vs),
		tg_(new boost::thread_group) {}


protected:
	virtual_service& vs_;
	boost::shared_ptr<boost::thread_group> tg_;
	session_map_type session_map;
};

#endif	//__THREAD_POOL_H__
