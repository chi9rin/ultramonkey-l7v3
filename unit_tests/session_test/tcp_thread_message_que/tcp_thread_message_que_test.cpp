#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

#define TCP_THREAD_MESSAGE_H
namespace l7vs{

	class tcp_thread_message : private boost::noncopyable{
		public:
			//! construcor 
			tcp_thread_message() : msg_id(0){};
			//! destructor
			~tcp_thread_message(){};
			
			int msg_id;
			
	};// class tcp_thread_message
}// namespace l7vs


#include "../../../src/tcp_thread_message_que.cpp"

using namespace boost::unit_test_framework;

class common_test : public l7vs::tcp_thread_message_que{
	public:
		common_test(){};
		~common_test(){};
		std::queue< l7vs::tcp_thread_message_que::tcp_thread_message_ptr >& get_que(){
			return  message_que;
		}
};


//--test case--
// construcor test
void construcor_test(){

	
	BOOST_MESSAGE( "----- construcor test start -----" );
	
	common_test test_obj;

	BOOST_MESSAGE( "----- construcor test end -----" );
}

// push test
void push_test() {
	BOOST_MESSAGE( "----- push test start -----" );

	common_test test_obj;
	std::queue< l7vs::tcp_thread_message_que::tcp_thread_message_ptr >& test_que = test_obj.get_que();

	boost::shared_ptr<l7vs::tcp_thread_message> chk_msg;

	// ## push test [1] 
	std::cout << "push test [1] " << std::endl;
	BOOST_CHECK(test_que.empty());
	for(int i = 0; i < (int)USHRT_MAX;i++){	
		boost::shared_ptr<l7vs::tcp_thread_message> test1_msg(new l7vs::tcp_thread_message);
		test1_msg->msg_id = i;
		test_obj.push(test1_msg);
	}

	BOOST_CHECK(!test_que.empty());
	BOOST_CHECK_EQUAL(test_que.size() , (unsigned long)USHRT_MAX);

	for(int i = 0; i < (int)USHRT_MAX;i++){
		chk_msg = test_que.front();
		BOOST_CHECK_EQUAL(i , chk_msg->msg_id);
		test_que.pop();
	}

	BOOST_MESSAGE( "----- push test end -----" );
	
}
// front test
void front_test(){
	BOOST_MESSAGE( "----- front test start -----" );

	common_test test_obj;
	std::queue< l7vs::tcp_thread_message_que::tcp_thread_message_ptr >& test_que = test_obj.get_que();

	boost::shared_ptr<l7vs::tcp_thread_message> chk_msg;
	
	// ## front test [1] empty queue
	std::cout << "front test [1] empty queue" << std::endl;
	BOOST_CHECK(test_obj.front() == chk_msg);

	// ## front test [2] push queue
	std::cout << "front test [2] push queue" << std::endl;
	BOOST_CHECK(test_que.empty());
	for(int i = 0; i < (int)USHRT_MAX;i++){	
		boost::shared_ptr<l7vs::tcp_thread_message> test1_msg(new l7vs::tcp_thread_message);
		test1_msg->msg_id = i;
		test_que.push(test1_msg);
	}

	BOOST_CHECK(!test_que.empty());
	BOOST_CHECK_EQUAL(test_que.size() , (unsigned long)USHRT_MAX);

	for(int i = 0; i < (int)USHRT_MAX;i++){
		chk_msg = test_obj.front();
		BOOST_CHECK_EQUAL(i , chk_msg->msg_id);
	}
	BOOST_CHECK(test_que.empty());
	
	BOOST_MESSAGE( "----- front test end -----" );
}
// empty test
void empty_test(){
	BOOST_MESSAGE( "----- empty test start -----" );

	common_test test_obj;
	std::queue< l7vs::tcp_thread_message_que::tcp_thread_message_ptr >& test_que = test_obj.get_que();

	boost::shared_ptr<l7vs::tcp_thread_message> chk_msg;

	// ## empty test [1] que qmpty
	std::cout << "empty test [1] que qmpty" << std::endl;
	BOOST_CHECK(test_obj.empty());
	
	// ## empty test [2] push que
	std::cout << "empty test [2] push que" << std::endl;
	for(int i = 0; i < (int)USHRT_MAX;i++){	
		boost::shared_ptr<l7vs::tcp_thread_message> test1_msg(new l7vs::tcp_thread_message);
		test1_msg->msg_id = i;
		test_que.push(test1_msg);
		BOOST_CHECK(!test_obj.empty());
	}

	// ## empty test [3] pop que
	std::cout << "empty test [3] pop que" << std::endl;
	for(int i = 0; i < (int)USHRT_MAX;i++){
		BOOST_CHECK(!test_obj.empty());
		test_que.pop();
	}
	
	// ## empty test [4] all pop que
	std::cout << "empty test [4] all pop que" << std::endl;	
	BOOST_CHECK(test_obj.empty());

	BOOST_MESSAGE( "----- empty test end -----" );	
}
// clear test
void clear_test(){
	BOOST_MESSAGE( "----- clear test start -----" );

	common_test test_obj;
	std::queue< l7vs::tcp_thread_message_que::tcp_thread_message_ptr >& test_que = test_obj.get_que();

	boost::shared_ptr<l7vs::tcp_thread_message> chk_msg;

	// ## clear test [1] queue empty
	std::cout << "clear test [1] queue empty" << std::endl;
	BOOST_CHECK(test_que.empty());
	test_obj.clear();	
	BOOST_CHECK(test_que.empty());
	
	
	// ## clear test [2] push queue 
	std::cout << "clear test [2] push empty" << std::endl;	
	for(int i = 0; i < (int)USHRT_MAX;i++){	
		boost::shared_ptr<l7vs::tcp_thread_message> test1_msg(new l7vs::tcp_thread_message);
		test1_msg->msg_id = i;
		test_que.push(test1_msg);
	}

	BOOST_CHECK(!test_que.empty());
	BOOST_CHECK_EQUAL(test_que.size() , (unsigned long)USHRT_MAX);	
	test_obj.clear();
	BOOST_CHECK(test_que.empty());
	
	BOOST_MESSAGE( "----- clear test end -----" );
	
}
// push_lock_test
class que_push_test : public l7vs::tcp_thread_message_que{
	public:
		boost::thread::id befor_thread_id;
		boost::thread::id after_thread_id;
		que_push_test(){
		};
		~que_push_test(){
		};
		void push(l7vs::tcp_thread_message_que::tcp_thread_message_ptr message){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_thread_message_que::push(message);
			after_thread_id = boost::this_thread::get_id();
			
		};
		void mutex_lock(){
			que_mutex.wrlock();
		};
		void mutex_unlock(){
			que_mutex.unlock();
		};
		
		boost::mutex test_thread_wait;
		
		void test_run(){
			boost::mutex::scoped_lock scope_lock(test_thread_wait);
			boost::shared_ptr<l7vs::tcp_thread_message> msg( new l7vs::tcp_thread_message);
			push(msg);
			message_que.pop();
		};
	
};
void push_lock_test(){
	BOOST_MESSAGE( "----- push lock test start -----" );
		
	que_push_test test_obj;
	
	test_obj.test_thread_wait.lock();
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.befor_thread_id = proc_id;
	test_obj.after_thread_id = proc_id;
	test_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&que_push_test::test_run,&test_obj));
	
	BOOST_CHECK(test_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_obj.test_thread_wait.unlock();
	sleep(1);
	
	// ## push lock test [1] thread block test (mutex lock)
	std::cout << "push lock test [1] thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == proc_id);
	
	test_obj.mutex_unlock();
	sleep(1);
	
	// ## push lock test [2] thread run test (mutex unlock)
	std::cout << "push lock test [2] thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == test_id);
	
	
	BOOST_MESSAGE( "----- push lock test end -----" );
}

// front_lock_test
class que_front_test : public l7vs::tcp_thread_message_que{
	public:
		boost::thread::id befor_thread_id;
		boost::thread::id after_thread_id;
		que_front_test(){
		};
		~que_front_test(){
		};
		l7vs::tcp_thread_message_que::tcp_thread_message_ptr front(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_thread_message_que::tcp_thread_message_ptr res;
			res = l7vs::tcp_thread_message_que::front();
			after_thread_id = boost::this_thread::get_id();
			return res;
		};
		void mutex_lock(){
			que_mutex.wrlock();
		};
		void mutex_unlock(){
			que_mutex.unlock();
		};
		
		boost::mutex test_thread_wait;
		
		void test_run(){
			boost::mutex::scoped_lock scope_lock(test_thread_wait);
			front();
		};
	
};
void front_lock_test(){
	BOOST_MESSAGE( "----- front lock test start -----" );
			
	que_front_test test_obj;
			
	test_obj.test_thread_wait.lock();
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.befor_thread_id = proc_id;
	test_obj.after_thread_id = proc_id;
	test_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&que_front_test::test_run,&test_obj));
	
	BOOST_CHECK(test_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_obj.test_thread_wait.unlock();
	sleep(1);
	
	// ## front lock test [1] thread block test (mutex lock)
	std::cout << "front lock test [1] thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == proc_id);
	
	test_obj.mutex_unlock();
	sleep(1);
	
	// ## front lock test [2] thread run test (mutex unlock)
	std::cout << "front lock test [2] thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == test_id);
	
	BOOST_MESSAGE( "----- front lock test end -----" );
}

// clear_lock_test
class que_clear_test : public l7vs::tcp_thread_message_que{
	public:
		boost::thread::id befor_thread_id;
		boost::thread::id after_thread_id;
		que_clear_test(){
		};
		~que_clear_test(){
		};
		void clear(){
			befor_thread_id = boost::this_thread::get_id();
			l7vs::tcp_thread_message_que::clear();
			after_thread_id = boost::this_thread::get_id();
		};
		void mutex_lock(){
			que_mutex.wrlock();
		};
		void mutex_unlock(){
			que_mutex.unlock();
		};
		
		boost::mutex test_thread_wait;
		
		void test_run(){
			boost::mutex::scoped_lock scope_lock(test_thread_wait);
			clear();
		};
	
};
void clear_lock_test(){
	BOOST_MESSAGE( "----- clear lock test start -----" );
	
	que_clear_test test_obj;
	
	test_obj.test_thread_wait.lock();
	boost::thread::id proc_id = boost::this_thread::get_id();
	test_obj.befor_thread_id = proc_id;
	test_obj.after_thread_id = proc_id;
	test_obj.mutex_lock();
	
	boost::thread test_thread(boost::bind(&que_clear_test::test_run,&test_obj));
	
	BOOST_CHECK(test_obj.befor_thread_id == proc_id);
	BOOST_CHECK(test_obj.after_thread_id == proc_id);
	
	boost::thread::id test_id = test_thread.get_id();
	
	BOOST_CHECK(test_id != proc_id);
	
	// test start
	test_obj.test_thread_wait.unlock();
	sleep(1);
	
	// ## clear lock test [1] thread block test (mutex lock)
	std::cout << "clear lock test [1] thread block test (mutex lock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == proc_id);
	
	test_obj.mutex_unlock();
	sleep(1);
	
	// ## clear lock test [2] thread run test (mutex unlock)
	std::cout << "clear lock test [2] thread run test (mutex unlock)" << std::endl;
	BOOST_CHECK(test_obj.befor_thread_id == test_id);
	BOOST_CHECK(test_obj.after_thread_id == test_id);
	
	
	BOOST_MESSAGE( "----- clear lock test end -----" );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "l7vs::tcp_thread_message_que class test" );

	ts->add( BOOST_TEST_CASE( &construcor_test ) );
	
	ts->add( BOOST_TEST_CASE( &push_test ) );
	ts->add( BOOST_TEST_CASE( &front_test ) );
	ts->add( BOOST_TEST_CASE( &empty_test ) );
	ts->add( BOOST_TEST_CASE( &clear_test ) );
	
	ts->add( BOOST_TEST_CASE( &push_lock_test ) );
	ts->add( BOOST_TEST_CASE( &front_lock_test ) );
	ts->add( BOOST_TEST_CASE( &clear_lock_test ) );

	framework::master_test_suite().add( ts );

	return NULL;
}
