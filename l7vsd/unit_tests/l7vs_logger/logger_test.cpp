/*

        Logger class Unit Test Program.


*/

#include <boost/test/unit_test.hpp>

using namespace boost::unit_test_framework;
//using namespace l7vs;

//enum LOG_ROTATION_TAG{ LOG_ROT_SIZE, LOG_ROT_DATE, LOG_ROT_DATE_SIZE };
//enum LOG_ROTATION_TIMING_TAG{ LOG_TIM_YEAR, LOG_TIM_MONTH, LOG_TIM_WEEK, LG_TIM_DATE, LO_TIM_HOUR };

#include "logger_enum.h"
#include "logger.cpp"

class   Logger_Test : public l7vs::Logger
{
public:
        Logger_Test() {}
        static void     constractor_test();
        static void     destractor_test();
        static void     reloadConf_test();
        static void     getinstance_test();
        static void     getLogLevel_test();
        static void     setLogLevel_test();
        static void     putLogFatal_test();
        static void     putLogError_test();
        static void     putLogWarn_test();
        static void     putLogInfo_test();
        static void     putLogDebug_test();
        static void     getCategoryRangeStart_test();
        static void     getCategoryRangeEnd_test();
};

template <typename T> T &operator++(T &value)
{
        value = static_cast<T>(value + 1);
        return value;
}

void Logger_Test::constractor_test()
{
        LoggerImpl      impl = LoggerImpl::getInstance();
        bool            flag = true;
        impl.init_flag = flag;
        Logger_Test     logger;
        BOOST_CHECK_EQUAL(flag , LoggerImpl::getInstance().init_flag);
}
void Logger_Test::destractor_test()
{
        //nothing
}
void Logger_Test::getinstance_test()
{
        //複数回のgetInstanceを呼んで同じものがくるか
        l7vs::Logger logger = l7vs:: Logger::getInstance();
        void *first = &logger;
        logger = Logger::getInstance();
        void *second = &logger;
        BOOST_CHECK_EQUAL(first, second);
        //失敗は存在しないのでなし
}
void Logger_Test::reloadConf_test()
{
        //rimplのreadが呼ばれて素の結果が返却されているか
        LoggerImpl impl = LoggerImpl::getInstance();
        l7vs::Logger    logger = l7vs::Logger::getInstance();
        //true
        bool test_flag = true;
        impl.reloadConf_flag = true;
        BOOST_CHECK_EQUAL(logger.reloadConf(), test_flag);
        //false
        test_flag = false;
        impl.reloadConf_flag = test_flag;
        BOOST_CHECK_EQUAL(logger.reloadConf(), test_flag);
}

void Logger_Test::getLogLevel_test()
{
        LoggerImpl      impl = LoggerImpl::getInstance();
        l7vs::Logger    logger = l7vs::Logger::getInstance();
        for (LOG_LEVEL_TAG level = LOG_LV_NONE; level <= LOG_LV_FATAL; ++level) {
                for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= LOG_CAT_SNMPAGENT_L7VSD_SEND; ++cat) {
                        impl.test_log_level = level;
                        impl.test_log_cat   = cat;
                        BOOST_CHECK_EQUAL(static_cast<int>(logger.getLogLevel(cat)) , static_cast<int>(level));
                }
        }
}
void Logger_Test::setLogLevel_test()
{
        LoggerImpl impl = LoggerImpl::getInstance();
        l7vs::Logger logger = l7vs::Logger::getInstance();
        for (LOG_LEVEL_TAG level = LOG_LV_NONE; level <= LOG_LV_FATAL; ++level) {
                for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= LOG_CAT_SNMPAGENT_L7VSD_SEND; ++cat) {
                        bool flag = true;
                        impl.setLogLevel_flag = flag;
                        BOOST_CHECK_EQUAL(static_cast<int>(logger.setLogLevel(cat, level)), static_cast<int>(flag));
                        BOOST_CHECK_EQUAL(static_cast<int>(impl.test_log_level) , static_cast<int>(level));
                        BOOST_CHECK_EQUAL(static_cast<int>(impl.test_log_cat), static_cast<int>(cat));
                        flag = false;
                        impl.setLogLevel_flag = flag;
                        BOOST_CHECK_EQUAL(static_cast<int>(logger.setLogLevel(cat, level)) , static_cast<int>(flag));
                        BOOST_CHECK_EQUAL(static_cast<int>(impl.test_log_level), static_cast<int>(level));
                        BOOST_CHECK_EQUAL(static_cast<int>(impl.test_log_cat), static_cast<int>(cat));
                }
        }
}
void Logger_Test::putLogFatal_test()
{
        LoggerImpl impl = LoggerImpl::getInstance();
        l7vs::Logger logger = l7vs::Logger::getInstance();
        unsigned int mesid = 0;
        std::string  mes  = "mes";
        std::string  file = "file";
        int          line = 0;
        for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= LOG_CAT_SNMPAGENT_L7VSD_SEND; ++cat) {
                logger.putLogFatal(cat, mesid, mes, file.c_str(), line);
                BOOST_CHECK_EQUAL(static_cast<int>(impl.test_log_cat), static_cast<int>(cat));
                BOOST_CHECK_EQUAL(impl.test_mesid, mesid);
                BOOST_CHECK_EQUAL(impl.test_file, file);
                BOOST_CHECK_EQUAL(impl.test_line, line);
        }
}
void Logger_Test::putLogError_test()
{
        LoggerImpl impl = LoggerImpl::getInstance();
        l7vs::Logger logger = l7vs::Logger::getInstance();
        unsigned int mesid = 0;
        std::string  mes  = "mes";
        std::string  file = "file";
        int          line = 0;
        for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= LOG_CAT_SNMPAGENT_L7VSD_SEND; ++cat) {
                logger.putLogError(cat, mesid, mes, file.c_str(), line);
                BOOST_CHECK_EQUAL(static_cast<int>(impl.test_log_cat), static_cast<int>(cat));
                BOOST_CHECK_EQUAL(impl.test_mesid, mesid);
                BOOST_CHECK_EQUAL(impl.test_file, file);
                BOOST_CHECK_EQUAL(impl.test_line, line);
        }
}
void Logger_Test::putLogWarn_test()
{
        LoggerImpl impl = LoggerImpl::getInstance();
        l7vs::Logger logger = l7vs::Logger::getInstance();
        unsigned int mesid = 0;
        std::string  mes  = "mes";
        std::string  file = "file";
        int          line = 0;
        for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= LOG_CAT_SNMPAGENT_L7VSD_SEND; ++cat) {
                logger.putLogWarn(cat, mesid, mes, file.c_str(), line);
                BOOST_CHECK_EQUAL(static_cast<int>(impl.test_log_cat), static_cast<int>(cat));
                BOOST_CHECK_EQUAL(impl.test_mesid, mesid);
                BOOST_CHECK_EQUAL(impl.test_file, file);
                BOOST_CHECK_EQUAL(impl.test_line, line);
        }
}
void Logger_Test::putLogInfo_test()
{
        LoggerImpl impl = LoggerImpl::getInstance();
        l7vs::Logger logger = l7vs::Logger::getInstance();
        unsigned int mesid = 0;
        std::string  mes  = "mes";
        std::string  file = "file";
        int          line = 0;
        for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= LOG_CAT_SNMPAGENT_L7VSD_SEND; ++cat) {
                logger.putLogInfo(cat, mesid, mes, file.c_str(), line);
                BOOST_CHECK_EQUAL(static_cast<int>(impl.test_log_cat), static_cast<int>(cat));
                BOOST_CHECK_EQUAL(impl.test_mesid, mesid);
                BOOST_CHECK_EQUAL(impl.test_file, file);
                BOOST_CHECK_EQUAL(impl.test_line, line);
        }
}
void Logger_Test::putLogDebug_test()
{
        LoggerImpl impl = LoggerImpl::getInstance();
        l7vs::Logger logger = l7vs::Logger::getInstance();
        unsigned int mesid = 0;
        std::string  mes  = "mes";
        std::string  file = "file";
        int          line = 0;
        for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= LOG_CAT_SNMPAGENT_L7VSD_SEND; ++cat) {
                logger.putLogDebug(cat, mesid, mes, file.c_str(), line);
                BOOST_CHECK_EQUAL(static_cast<int>(impl.test_log_cat), static_cast<int>(cat));
                BOOST_CHECK_EQUAL(impl.test_mesid, mesid);
                BOOST_CHECK_EQUAL(impl.test_file, file);
                BOOST_CHECK_EQUAL(impl.test_line, line);
        }
}
void Logger_Test::getCategoryRangeStart_test()
{
        LoggerImpl impl = LoggerImpl::getInstance();
        l7vs::Logger    logger = l7vs::Logger::getInstance();
        for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= LOG_CAT_SNMPAGENT_L7VSD_SEND; ++cat) {
                for (LOG_MODULE_TAG mod = LOG_MOD_L7VSD; mod <= LOG_MOD_SNMPAGENT; ++mod) {
                        impl.test_log_cat = cat;
                        BOOST_CHECK_EQUAL(static_cast<int>(cat), static_cast<int>(logger.getCategoryRangeStart(mod)));
                        BOOST_CHECK_EQUAL(mod, impl.test_module);
                }
        }
}
void Logger_Test::getCategoryRangeEnd_test()
{
        LoggerImpl impl = LoggerImpl::getInstance();
        l7vs::Logger    logger = l7vs::Logger::getInstance();
        for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= LOG_CAT_SNMPAGENT_L7VSD_SEND; ++cat) {
                for (LOG_MODULE_TAG mod = LOG_MOD_L7VSD; mod <= LOG_MOD_SNMPAGENT; ++mod) {
                        impl.test_log_cat = cat;
                        BOOST_CHECK_EQUAL(static_cast<int>(cat), static_cast<int>(logger.getCategoryRangeEnd(mod)));
                        BOOST_CHECK_EQUAL(mod, impl.test_module);
                }
        }
}

test_suite*
init_unit_test_suite(int argc, char *argv[])
{
        test_suite *test = BOOST_TEST_SUITE("logger class test");
        test->add(BOOST_TEST_CASE(Logger_Test::constractor_test));
        test->add(BOOST_TEST_CASE(Logger_Test::getinstance_test));
        test->add(BOOST_TEST_CASE(Logger_Test::getLogLevel_test));
        test->add(BOOST_TEST_CASE(Logger_Test::setLogLevel_test));
        test->add(BOOST_TEST_CASE(Logger_Test::putLogFatal_test));
        test->add(BOOST_TEST_CASE(Logger_Test::putLogError_test));
        test->add(BOOST_TEST_CASE(Logger_Test::putLogWarn_test));
        test->add(BOOST_TEST_CASE(Logger_Test::putLogDebug_test));
        test->add(BOOST_TEST_CASE(Logger_Test::getCategoryRangeStart_test));
        test->add(BOOST_TEST_CASE(Logger_Test::getCategoryRangeEnd_test));

        return test;
}

