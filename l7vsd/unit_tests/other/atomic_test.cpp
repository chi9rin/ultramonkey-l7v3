#include <pthread.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>

#include <vector>
#include "../../include/atomic.h"
#include "rdtsc64.h"

//  このテストコードでは、Atomicとwrlock.の確認を同時に実施します。

#define THREAD_COUNT 10
#define CHECK(a,b,c,d)  do { \
                                if (a != b) { \
                                        std::cout << "error " << c << ": "; \
                                        std::cout << a << " != " << b; \
                                        std::cout << ", line:" << d <<std::endl; \
                                        ret = 1; \
                                } \
                        } while (0)

using namespace boost::unit_test_framework;

typedef    l7vs::atomic< int >    atomic_int_t;
atomic_int_t    int_value;

typedef    l7vs::atomic< long long >    atomic_long_t;
atomic_long_t    long_value;

typedef    l7vs::atomic< unsigned long long >    atomic_ulong_t;
atomic_ulong_t    ulong_value;

unsigned long long lock_ulong_value;

static int ret = 0;

void    *thread_func_atomic_i(void *param)
{
        int temp;

//    ++int_value;
//    --int_value;
        int_value = 0;
        temp = int_value.get();
//std::cout << "int_value=(:0):" << temp << std::endl;
        CHECK(0, temp, "get", __LINE__);
//    BOOST_CHECK( 0 == temp );
        int_value++;
        temp = int_value.get();
//std::cout << "int_value++(:1):" << temp << std::endl;
//    BOOST_CHECK( 1 == temp );
        CHECK(1, temp, "++", __LINE__);
        int_value--;
        temp = int_value.get();
//std::cout << "int_value--(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
        CHECK(0, temp, "--", __LINE__);
        int_value += 1000;
        temp = int_value.get();
//std::cout << "int_value+=(:999):" << temp << std::endl;
//    BOOST_CHECK( 1000 == temp );
        CHECK(1000, temp, "+=", __LINE__);
        int_value -= 1000;
        temp = int_value.get();
//std::cout << "int_value-=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
        CHECK(0, temp, "-=", __LINE__);
        int_value      += 1000;
//std::cout << "int_value< (:0)"  << (int_value< 1000) << std::endl;
//    BOOST_CHECK( 0 == (int_value< 1000) );
        CHECK(0, (int_value < 1000), "<", __LINE__);
//std::cout << "int_value<=(:1)" << (int_value<=1000) << std::endl;
//    BOOST_CHECK( 1 == (int_value<=1000) );
        CHECK(1, (int_value <= 1000), "<=", __LINE__);
//std::cout << "int_value> (:0)"  << (int_value> 1000) << std::endl;
//    BOOST_CHECK( 0 == (int_value> 1000) );
        CHECK(0, (int_value > 1000), ">", __LINE__);
//std::cout << "int_value>=(:1)" << (int_value>=1000) << std::endl;
//    BOOST_CHECK( 1 == (int_value>=1000) );
        CHECK(1, (int_value >= 1000), ">=", __LINE__);
//std::cout << "int_value==(:1)" << (int_value==1000) << std::endl;
//    BOOST_CHECK( 1 == (int_value==1000) );
        CHECK(1, (int_value == 1000), "==", __LINE__);
//std::cout << "int_value!==(:0)" << (int_value!=1000) << std::endl;
//    BOOST_CHECK( 0 == (int_value!=1000) );
        CHECK(0, (int_value != 1000), "!=", __LINE__);
//std::cout << "int_value< (:0)"  << (int_value< 999) << std::endl;
//    BOOST_CHECK( 0 == (int_value< 999) );
        CHECK(0, (int_value < 999), "<", __LINE__);
//std::cout << "int_value<=(:0)" << (int_value<=999) << std::endl;
//    BOOST_CHECK( 0 == (int_value<=999) );
        CHECK(0, (int_value <= 999), "<=", __LINE__);
//std::cout << "int_value> (:1)"  << (int_value> 999) << std::endl;
//    BOOST_CHECK( 1 == (int_value> 999) );
        CHECK(1, (int_value > 999), ">", __LINE__);
//std::cout << "int_value>=(:1)" << (int_value>=999) << std::endl;
//    BOOST_CHECK( 1 == (int_value>=999) );
        CHECK(1, (int_value >= 999), ">=", __LINE__);
//std::cout << "int_value==(:0)" << (int_value==999) << std::endl;
//    BOOST_CHECK( 0 == (int_value==999) );
        CHECK(0, (int_value == 999), "==", __LINE__);
//std::cout << "int_value!==(:1)" << (int_value!=999) << std::endl;
//    BOOST_CHECK( 1 == (int_value!=999) );
        CHECK(1, (int_value != 999), "!=", __LINE__);
//std::cout << "int_value< (:1)"  << (int_value <1001) << std::endl;
//    BOOST_CHECK( 1 == (int_value <1001) );
        CHECK(1, (int_value < 1001), "<", __LINE__);
//std::cout << "int_value<=(:1)" << (int_value<=1001) << std::endl;
//    BOOST_CHECK( 0 == (int_value<=1001) );
        CHECK(1, (int_value <= 1001), "<=", __LINE__);
//std::cout << "int_value> (:0)"  << (int_value >1001) << std::endl;
//    BOOST_CHECK( 0 == (int_value >1001) );
        CHECK(0, (int_value > 1001), ">", __LINE__);
//std::cout << "int_value>=(:0)" << (int_value>=1001) << std::endl;
//    BOOST_CHECK( 0 == (int_value>=1001) );
        CHECK(0, (int_value >= 1001), ">=", __LINE__);
//std::cout << "int_value==(:0)" << (int_value==1001) << std::endl;
//    BOOST_CHECK( 0 == (int_value==1001) );
        CHECK(0, (int_value == 1001), "==", __LINE__);
//std::cout << "int_value!==(:1)" << (int_value!=1001) << std::endl;
//    BOOST_CHECK( 1 == (int_value!=1001) );
        CHECK(1, (int_value != 1001), "!=", __LINE__);
}

void    *thread_func_atomic_ll(void *param)
{
        long long temp;

//    ++long_value;
//    --long_value;
        long_value = 0;
        temp = long_value.get();
//std::cout << "long_value=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
        CHECK(0, temp, "get", __LINE__);
        long_value++;
        temp = long_value.get();
//std::cout << "long_value++(:1):" << temp << std::endl;
//    BOOST_CHECK( 1 == temp );
        CHECK(1, temp, "++", __LINE__);
        long_value--;
        temp = long_value.get();
//std::cout << "long_value--(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
        CHECK(0, temp, "--", __LINE__);
        long_value += 1000;
        temp = long_value.get();
//std::cout << "long_value+=(:1000):" << temp << std::endl;
//    BOOST_CHECK( 1000 == temp );
        CHECK(1000, temp, "+=", __LINE__);
        long_value -= 1000;
        temp = long_value.get();
//std::cout << "long_value-=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
        CHECK(0, temp, "-=", __LINE__);
        long_value      += 1000;
//std::cout << "long_value< (:0)"  << (long_value< 1000) << std::endl;
//    BOOST_CHECK( 0 == (long_value< 1000) );
        CHECK(0, (long_value < 1000), "<", __LINE__);
//std::cout << "long_value<=(:1)" << (long_value<=1000) << std::endl;
//    BOOST_CHECK( 1 == (long_value<=1000) );
        CHECK(1, (long_value <= 1000), "<=", __LINE__);
//std::cout << "long_value> (:0)"  << (long_value> 1000) << std::endl;
//    BOOST_CHECK( 0 == (long_value> 1000) );
        CHECK(0, (long_value > 1000), ">", __LINE__);
//std::cout << "long_value>=(:1)" << (long_value>=1000) << std::endl;
//    BOOST_CHECK( 1 == (long_value>=1000) );
        CHECK(1, (long_value >= 1000), ">=", __LINE__);
//std::cout << "long_value==(:1)" << (long_value==1000) << std::endl;
//    BOOST_CHECK( 1 == (long_value==1000) );
        CHECK(1, (long_value == 1000), "==", __LINE__);
//std::cout << "long_value!==(:0)" << (long_value!=1000) << std::endl;
//    BOOST_CHECK( 0 == (long_value!=1000) );
        CHECK(0, (long_value != 1000), "!=", __LINE__);
//std::cout << "long_value< (:0)"  << (long_value< 999) << std::endl;
//    BOOST_CHECK( 0 == (long_value< 999) );
        CHECK(0, (long_value < 999), "<", __LINE__);
//std::cout << "long_value<=(:0)" << (long_value<=999) << std::endl;
//    BOOST_CHECK( 0 == (long_value<=999) );
        CHECK(0, (long_value <= 999), "<=", __LINE__);
//std::cout << "long_value> (:1)"  << (long_value> 999) << std::endl;
//    BOOST_CHECK( 1 == (long_value> 999) );
        CHECK(1, (long_value > 999), ">", __LINE__);
//std::cout << "long_value>=(:1)" << (long_value>=999) << std::endl;
//    BOOST_CHECK( 1 == (long_value>=999) );
        CHECK(1, (long_value >= 999), ">=", __LINE__);
//std::cout << "long_value==(:0)" << (long_value==999) << std::endl;
//    BOOST_CHECK( 0 == (long_value==999) );
        CHECK(0, (long_value == 999), "==", __LINE__);
//std::cout << "long_value!==(:1)" << (long_value!=999) << std::endl;
//    BOOST_CHECK( 1 == (long_value!=999) );
        CHECK(1, (long_value != 999), "!=", __LINE__);
//std::cout << "long_value< (:1)"  << (long_value <1001) << std::endl;
//    BOOST_CHECK( 1 == (long_value <1001) );
        CHECK(1, (long_value < 1001), "<", __LINE__);
//std::cout << "long_value<=(:1)" << (long_value<=1001) << std::endl;
//    BOOST_CHECK( 1 == (long_value<=1001) );
        CHECK(1, (long_value <= 1001), "<=", __LINE__);
//std::cout << "long_value> (:0)"  << (long_value >1001) << std::endl;
//    BOOST_CHECK( 0 == (long_value >1001) );
        CHECK(0, (long_value > 1001), ">", __LINE__);
//std::cout << "long_value>=(:0)" << (long_value>=1001) << std::endl;
//    BOOST_CHECK( 0 == (long_value>=1001) );
        CHECK(0, (long_value >= 1001), ">=", __LINE__);
//std::cout << "long_value==(:0)" << (long_value==1001) << std::endl;
//    BOOST_CHECK( 0 == (long_value==1001) );
        CHECK(0, (long_value == 1001), "==", __LINE__);
//std::cout << "long_value!==(:1)" << (long_value!=1001) << std::endl;
//    BOOST_CHECK( 1 == (long_value!=1001) );
        CHECK(1, (long_value != 1001), "!=", __LINE__);
}

void    *thread_func_atomic_ull(void *param)
{
        unsigned long long temp;

//    ++ulong_value;
//    --ulong_value;
        ulong_value = 0;
        temp = ulong_value.get();
//std::cout << "ulong_value=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
        CHECK(0, temp, "get", __LINE__);
        ulong_value++;
        temp = ulong_value.get();
//std::cout << "ulong_value++(:1):" << temp << std::endl;
//    BOOST_CHECK( 1 == temp );
        CHECK(1, temp, "++", __LINE__);
        ulong_value--;
        temp = ulong_value.get();
//std::cout << "ulong_value--(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
        CHECK(0, temp, "--", __LINE__);
        ulong_value += 1000;
        temp = ulong_value.get();
//std::cout << "ulong_value+=(:1000):" << temp << std::endl;
//    BOOST_CHECK( 1000 == temp );
        CHECK(1000, temp, "+=", __LINE__);
        ulong_value -= 1000;
        temp = ulong_value.get();
//std::cout << "ulong_value-=(:0):" << temp << std::endl;
//    BOOST_CHECK( 0 == temp );
        CHECK(0, temp, "-=", __LINE__);
        ulong_value      += 1000;
//std::cout << "ulong_value< (:0)"  << (ulong_value< 1000) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value< 1000) );
        CHECK(0, (ulong_value < 1000), "<", __LINE__);
//std::cout << "ulong_value<=(:1)" << (ulong_value<=1000) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value<=1000) );
        CHECK(1, (ulong_value <= 1000), "<=", __LINE__);
//std::cout << "ulong_value> (:0)"  << (ulong_value> 1000) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value> 1000) );
        CHECK(0, (ulong_value > 1000), ">", __LINE__);
//std::cout << "ulong_value>=(:1)" << (ulong_value>=1000) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value>=1000) );
        CHECK(1, (ulong_value >= 1000), ">=", __LINE__);
//std::cout << "ulong_value==(:1)" << (ulong_value==1000) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value==1000) );
        CHECK(1, (ulong_value == 1000), "==", __LINE__);
//std::cout << "ulong_valu!==(:0)" << (ulong_value!=1000) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value!=1000) );
        CHECK(0, (ulong_value != 1000), "!=", __LINE__);
//std::cout << "ulong_value< (:0)"  << (ulong_value< 999) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value< 999) );
        CHECK(0, (ulong_value < 999), "<", __LINE__);
//std::cout << "ulong_value<=(:0)" << (ulong_value<=999) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value<=999) );
        CHECK(0, (ulong_value <= 999), "<=", __LINE__);
//std::cout << "ulong_value> (:1)"  << (ulong_value> 999) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value> 999) );
        CHECK(1, (ulong_value > 999), ">", __LINE__);
//std::cout << "ulong_value>=(:1)" << (ulong_value>=999) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value>=999) );
        CHECK(1, (ulong_value >= 999), ">=", __LINE__);
//std::cout << "ulong_value==(:0)" << (ulong_value==999) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value==999) );
        CHECK(0, (ulong_value == 999), "==", __LINE__);
//std::cout << "ulong_valu!==(:1)" << (ulong_value!=999) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value!=999) );
        CHECK(1, (ulong_value != 999), "!=", __LINE__);
//std::cout << "ulong_value< (:1)"  << (ulong_value <1001) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value <1001) );
        CHECK(1, (ulong_value < 1001), "<", __LINE__);
//std::cout << "ulong_value<=(:1)" << (ulong_value<=1001) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value<=1001) );
        CHECK(1, (ulong_value <= 1001), "<=", __LINE__);
//std::cout << "ulong_value> (:0)"  << (ulong_value >1001) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value >1001) );
        CHECK(0, (ulong_value > 1001), ">", __LINE__);
//std::cout << "ulong_value>=(:0)" << (ulong_value>=1001) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value>=1001) );
        CHECK(0, (ulong_value >= 1001), ">=", __LINE__);
//std::cout << "ulong_value==(:0)" << (ulong_value==1001) << std::endl;
//    BOOST_CHECK( 0 == (ulong_value==1001) );
        CHECK(0, (ulong_value == 1001), "==", __LINE__);
//std::cout << "ulong_valu!==(:1)" << (ulong_value!=1001) << std::endl;
//    BOOST_CHECK( 1 == (ulong_value!=1001) );
        CHECK(1, (ulong_value != 1001), "!=", __LINE__);
}

void run_test(void*(*func)(void *))
{
        pthread_t thd[THREAD_COUNT];

        for (int i = 0; i < THREAD_COUNT; ++i) {
                pthread_create(&thd[i], NULL, func, NULL);
        }
        for (int i = 0; i < THREAD_COUNT; ++i) {
                pthread_join(thd[i], NULL);
        }
}

void atomic_test()
{
        //atomic version
        std::cout << "atomic func time start" << std::endl;
        run_test(thread_func_atomic_i);
        run_test(thread_func_atomic_ll);
        run_test(thread_func_atomic_ull);
        BOOST_CHECK(0 == ret);
}

test_suite *init_unit_test_suite(int argc, char *argv[])
{
        ulong_value = 0;
        lock_ulong_value = 0;

        test_suite *ts = BOOST_TEST_SUITE("atomic class test");
        ts->add(BOOST_TEST_CASE(&atomic_test));
        framework::master_test_suite().add(ts);
}
