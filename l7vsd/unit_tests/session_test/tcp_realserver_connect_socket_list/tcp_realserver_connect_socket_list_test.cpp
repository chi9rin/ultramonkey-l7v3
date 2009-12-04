
#define    TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread/thread.hpp>

#include "logger.h"

#define TCP_SOCKET_H

// tcp_socket test class 
namespace l7vs{
    class tcp_socket : private boost::noncopyable{
        public:
            tcp_socket(){};
            ~tcp_socket(){};
            int socket_id; 
    };
}

#include "../../../src/tcp_realserver_connect_socket_list.cpp"

using namespace boost::unit_test_framework;

class common_test : public l7vs::tcp_realserver_connect_socket_list{
    public:
        common_test(){};
        ~common_test(){};
        std::list< l7vs::tcp_realserver_connect_socket_list::list_element >& get_list(){
            return  connect_list;
        }
};

//--test case--
// construcor test
void construcor_test(){

    
    BOOST_MESSAGE( "----- construcor test start -----" );
    
    common_test test_obj;
    
    BOOST_MESSAGE( "----- construcor test end -----" );
}


void push_back_test(){
    BOOST_MESSAGE( "----- push back test start -----" );
    
    typedef std::list< l7vs::tcp_realserver_connect_socket_list::list_element > list_type;

    common_test test_obj;
    
    list_type& test_list = test_obj.get_list();

    l7vs::tcp_realserver_connect_socket_list::list_element chk_element; 

    // ## push back test [1] 
    std::cout << "push back test [1] " << std::endl;
    BOOST_CHECK(test_list.empty());
    for(int i = 0; i < (int)USHRT_MAX;i++){
        boost::shared_ptr<l7vs::tcp_socket> push_socket(new l7vs::tcp_socket);
        l7vs::tcp_realserver_connect_socket_list::list_element push_element;
        push_element.second = push_socket;
        push_socket->socket_id = i;
        push_element.first.port((unsigned short)i);
        test_obj.push_back(push_element);
    }

    BOOST_CHECK(!test_list.empty());
    BOOST_CHECK_EQUAL(test_list.size() , (unsigned long)USHRT_MAX);

    for(int i = 0; i < (int)USHRT_MAX;i++){
        chk_element = test_list.front();
        BOOST_CHECK_EQUAL(i , chk_element.second->socket_id);
        BOOST_CHECK_EQUAL((unsigned short)i , chk_element.first.port());
        test_list.pop_front();
    }
    
    test_list.clear();
        
    BOOST_MESSAGE( "----- push back test end -----" );    
}
void get_socket_test(){
    BOOST_MESSAGE( "----- get socket test start -----" );
    
    typedef std::list< l7vs::tcp_realserver_connect_socket_list::list_element > list_type;

    common_test test_obj;
    
    list_type& test_list = test_obj.get_list();

    l7vs::tcp_realserver_connect_socket_list::list_element chk_element; 
    
    // ## get socket test [1] empty list
    std::cout << "get socket test [1] empty list" << std::endl;
    BOOST_CHECK(test_obj.get_socket() == chk_element);

    // ## get socket test [2] push list
    std::cout << "get socket test [2] push list" << std::endl;
    BOOST_CHECK(test_list.empty());
    for(int i = 0; i < (int)USHRT_MAX;i++){
        boost::shared_ptr<l7vs::tcp_socket> push_socket(new l7vs::tcp_socket);
        l7vs::tcp_realserver_connect_socket_list::list_element push_element;
        push_element.second = push_socket;
        push_socket->socket_id = i;
        push_element.first.port((unsigned short)i);
        test_list.push_back(push_element);
    }
    
    BOOST_CHECK(!test_list.empty());
    BOOST_CHECK_EQUAL(test_list.size() , (unsigned long)USHRT_MAX);

    for(int i = 0; i < (int)USHRT_MAX;i++){
        chk_element = test_obj.get_socket();
        BOOST_CHECK_EQUAL(i , chk_element.second->socket_id);
        BOOST_CHECK_EQUAL((unsigned short)i , chk_element.first.port());
    }
    
    BOOST_CHECK(test_list.empty());
    
    BOOST_MESSAGE( "----- get socket test end -----" );    
}
void empty_test(){
    BOOST_MESSAGE( "----- empty test start -----" );
    
    typedef std::list< l7vs::tcp_realserver_connect_socket_list::list_element > list_type;

    common_test test_obj;
    
    list_type& test_list = test_obj.get_list();

    l7vs::tcp_realserver_connect_socket_list::list_element chk_element; 

    // ## empty test [1] empty list
    std::cout << "empty test [1] empty list" << std::endl;
    BOOST_CHECK(test_obj.empty());
    
    // ## empty test [2] push list
    std::cout << "empty test [2] push list" << std::endl;
    for(int i = 0; i < (int)USHRT_MAX;i++){    
        boost::shared_ptr<l7vs::tcp_socket> push_socket(new l7vs::tcp_socket);
        l7vs::tcp_realserver_connect_socket_list::list_element push_element;
        push_element.second = push_socket;
        push_socket->socket_id = i;
        push_element.first.port((unsigned short)i);
        test_list.push_back(push_element);
        BOOST_CHECK(!test_obj.empty());
    }

    // ## empty test [3] pop list
    std::cout << "empty test [3] pop list" << std::endl;
    for(int i = 0; i < (int)USHRT_MAX;i++){
        BOOST_CHECK(!test_obj.empty());
        test_list.pop_front();
    }
    
    // ## empty test [4] all pop list
    std::cout << "empty test [4] all pop list" << std::endl;    
    BOOST_CHECK(test_obj.empty());
    
    BOOST_MESSAGE( "----- empty test end -----" );    
}

// push_back_lock_test
class push_back_lock_test_class : public l7vs::tcp_realserver_connect_socket_list{
    public:
        boost::thread::id befor_thread_id;
        boost::thread::id after_thread_id;
        push_back_lock_test_class(){
        };
        ~push_back_lock_test_class(){
        };
        void push_back(l7vs::tcp_realserver_connect_socket_list::list_element realserver_socket){
            befor_thread_id = boost::this_thread::get_id();
            l7vs::tcp_realserver_connect_socket_list::push_back(realserver_socket);
            after_thread_id = boost::this_thread::get_id();
        };
        void mutex_lock(){
            list_mutex.wrlock();
        };
        void mutex_unlock(){
            list_mutex.unlock();
        };
        
        boost::mutex test_thread_wait;
        
        void test_run(){
            boost::mutex::scoped_lock scope_lock(test_thread_wait);
            l7vs::tcp_realserver_connect_socket_list::list_element push_element;
            push_back(push_element);
            connect_list.pop_front();
        };
    
};
void push_back_lock_test(){
    BOOST_MESSAGE( "----- push backe lock test start -----" );
        
    push_back_lock_test_class test_obj;
    
    test_obj.test_thread_wait.lock();
    boost::thread::id proc_id = boost::this_thread::get_id();
    test_obj.befor_thread_id = proc_id;
    test_obj.after_thread_id = proc_id;
    test_obj.mutex_lock();
    
    boost::thread test_thread(boost::bind(&push_back_lock_test_class::test_run,&test_obj));
    
    BOOST_CHECK(test_obj.befor_thread_id == proc_id);
    BOOST_CHECK(test_obj.after_thread_id == proc_id);
    
    boost::thread::id test_id = test_thread.get_id();
    
    BOOST_CHECK(test_id != proc_id);
    
    // test start
    test_obj.test_thread_wait.unlock();
    sleep(1);
    
    // ## push back lock test [1] thread block test (mutex lock)
    std::cout << "push back lock test [1] thread block test (mutex lock)" << std::endl;
    BOOST_CHECK(test_obj.befor_thread_id == test_id);
    BOOST_CHECK(test_obj.after_thread_id == proc_id);
    
    test_obj.mutex_unlock();
    sleep(1);
    
    // ## push back lock test [2] thread run test (mutex unlock)
    std::cout << "push back lock test [2] thread run test (mutex unlock)" << std::endl;
    BOOST_CHECK(test_obj.befor_thread_id == test_id);
    BOOST_CHECK(test_obj.after_thread_id == test_id);
    
    
    BOOST_MESSAGE( "----- push back lock test end -----" );
}

// get_socket_lock_test
class get_socket_lock_test_class : public l7vs::tcp_realserver_connect_socket_list{
    public:
        boost::thread::id befor_thread_id;
        boost::thread::id after_thread_id;
        get_socket_lock_test_class(){
        };
        ~get_socket_lock_test_class(){
        };
        void get_socket(){
            befor_thread_id = boost::this_thread::get_id();
            l7vs::tcp_realserver_connect_socket_list::get_socket();
            after_thread_id = boost::this_thread::get_id();
        };
        void mutex_lock(){
            list_mutex.wrlock();
        };
        void mutex_unlock(){
            list_mutex.unlock();
        };
        
        boost::mutex test_thread_wait;
        
        void test_run(){
            boost::mutex::scoped_lock scope_lock(test_thread_wait);
            get_socket();
        };
    
};
void get_socket_lock_test(){

    BOOST_MESSAGE( "----- get socket lock test start -----" );
        
    get_socket_lock_test_class test_obj;
    
    test_obj.test_thread_wait.lock();
    boost::thread::id proc_id = boost::this_thread::get_id();
    test_obj.befor_thread_id = proc_id;
    test_obj.after_thread_id = proc_id;
    test_obj.mutex_lock();
    
    boost::thread test_thread(boost::bind(&get_socket_lock_test_class::test_run,&test_obj));
    
    BOOST_CHECK(test_obj.befor_thread_id == proc_id);
    BOOST_CHECK(test_obj.after_thread_id == proc_id);
    
    boost::thread::id test_id = test_thread.get_id();
    
    BOOST_CHECK(test_id != proc_id);
    
    // test start
    test_obj.test_thread_wait.unlock();
    sleep(1);
    
    // ## get socket lock test [1] thread block test (mutex lock)
    std::cout << "get socket lock test [1] thread block test (mutex lock)" << std::endl;
    BOOST_CHECK(test_obj.befor_thread_id == test_id);
    BOOST_CHECK(test_obj.after_thread_id == proc_id);
    
    test_obj.mutex_unlock();
    sleep(1);
    
    // ## get socket lock test [2] thread run test (mutex unlock)
    std::cout << "get socket lock test [2] thread run test (mutex unlock)" << std::endl;
    BOOST_CHECK(test_obj.befor_thread_id == test_id);
    BOOST_CHECK(test_obj.after_thread_id == test_id);
    
    BOOST_MESSAGE( "----- get socket lock test end -----" );
}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    test_suite* ts = BOOST_TEST_SUITE( "l7vs::tcp_realserver_connect_socket_list class test" );

    ts->add( BOOST_TEST_CASE( &construcor_test ) );
    ts->add( BOOST_TEST_CASE( &push_back_test ) );
    ts->add( BOOST_TEST_CASE( &push_back_lock_test ) );
    ts->add( BOOST_TEST_CASE( &get_socket_test ) );
    ts->add( BOOST_TEST_CASE( &get_socket_lock_test ) );
    ts->add( BOOST_TEST_CASE( &empty_test ) );
    
    framework::master_test_suite().add( ts );

    return NULL;
}
