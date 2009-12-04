#ifndef PROTOCOL_MODULE_CONTROL_TEST
#define PROTOCOL_MODULE_CONTROL_TEST

#include <boost/thread/condition.hpp>
#include "protocol_module_control.h"

#define    PM1    "protocol_module_test1"
#define    PM2    "protocol_module_test2"

class    l7vs::protocol_module_control;

class    protocol_module_control_testclass : public l7vs::protocol_module_control{
public:
    std::string&            get_modulefile_path(){ return modulefile_path; }
    name_module_info_map&    get_loadmodule_map(){ return loadmodule_map; }

    static protocol_module_control_testclass& getInstance(){
        static protocol_module_control_testclass instance_test;
        return instance_test;
    }

    boost::mutex            load_mutex;
    boost::condition        load_condition;

    l7vs::protocol_module_base* load_module2( int id, const std::string& modulename ){
        std::stringstream msg1, msg2;
        msg1 << "load_module start thread:" << id;
        msg2 << "load_module end thread:" << id;

        boost::mutex::scoped_lock lock( load_mutex );
        BOOST_MESSAGE(msg1.str());
        load_condition.wait( lock );

        l7vs::protocol_module_base* return_value = NULL;

        return_value = load_module(modulename);
        BOOST_MESSAGE(msg2.str());
        return return_value;
    }
    void unload_module2( int id, l7vs::protocol_module_base* module_ptr ){
        std::stringstream msg1, msg2;
        msg1 << "unload_module start thread:" << id;
        msg2 << "unload_module end thread:" << id;

        boost::mutex::scoped_lock lock( load_mutex );
        BOOST_MESSAGE(msg1.str());
        load_condition.wait( lock );

        unload_module(module_ptr);
        BOOST_MESSAGE(msg2.str());
    }
};

l7vs::Logger::Logger() :
    scopedLogCategory(LOG_CAT_L7VSD_LOGGER),
    scopedLogId(0),
    scopedLogMessage("Logger Constructor"),
    scopedLogFile(__FILE__),
    scopedLogLine(__LINE__) {}


l7vs::Logger::Logger( LOG_CATEGORY_TAG cat, const unsigned int id, const std::string& msg, const char* file, int line) :
    scopedLogCategory(cat),
    scopedLogId(id),
    scopedLogMessage(msg),
    scopedLogFile(file),
    scopedLogLine(line) {}

l7vs::Logger::~Logger(){}

void    l7vs::Logger::loadConf(){}


l7vs::LOG_LEVEL_TAG l7vs::Logger::getLogLevel( l7vs::LOG_CATEGORY_TAG ){
    l7vs::LOG_LEVEL_TAG        ret_tag = l7vs::LOG_LV_DEBUG;
    return ret_tag;
}
bool    l7vs::Logger::setLogLevel( l7vs::LOG_CATEGORY_TAG cat, l7vs::LOG_LEVEL_TAG level ){
    return true;
}


void    l7vs::Logger::putLogFatal( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "FATAL : " << msg << std::endl;
}

void    l7vs::Logger::putLogError( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "ERROR : " << msg << std::endl;
}

void    l7vs::Logger::putLogWarn( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "WARN  : " << msg << std::endl;
}

void    l7vs::Logger::putLogInfo( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "INFO  : " << msg << std::endl;
}

void    l7vs::Logger::putLogDebug( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "DEBUG : " << msg << std::endl;
}

#endif // PROTOCOL_MODULE_CONTROL_TEST
