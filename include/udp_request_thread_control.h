#ifndef UDP_REQUEST_THREAD_CONTROL_H
#define UDP_REQUEST_THREAD_CONTROL_H

#include <boost/shared_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "udp_request.h"

namespace l7vs{

	class udp_request_thread_control : private boost::noncopyable{
		protected:
			typedef boost::shared_ptr< boost::thread > thread_ptr;
			typedef boost::shared_ptr< l7vs::udp_request > request_ptr;

			enum UDP_REQUEST_THREAD_STATE_TAG{
				WAIT = 0,
				RUNNING,
				EXIT
			};

			thread_ptr request_thread;
			UDP_REQUEST_THREAD_STATE_TAG request_thread_state;
			boost::condition thread_condition;
			boost::mutex condition_mutex;
			request_ptr my_request;

		public:
			udp_request_thread_control(request_ptr request);
			~udp_request_thread_control(void);
			void start(void);
			void stop(void);
			void join(void);
			boost::thread::id get_thread_id();
			boost::shared_ptr< l7vs::udp_request > get_request();
			
		protected:
			void run(void);

	};// class udp_request_thread_control
}// namespace l7vs

#endif//UDP_REQUEST_THREAD_CONTROL_H
