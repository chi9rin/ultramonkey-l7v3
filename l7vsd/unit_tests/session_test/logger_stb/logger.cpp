#include "logger.h"
 
namespace l7vs{

    LOG_LEVEL_TAG Logger::test_loglevel = LOG_LV_NONE;
        
    LOG_CATEGORY_TAG Logger::Logger_category = LOG_CAT_NONE;
    int Logger::Logger_id         = 0;
    std::string Logger::Logger_message     = "";
    char* Logger::Logger_filename = NULL;
    int Logger::Logger_lineno     = 0;
        
    LOG_CATEGORY_TAG Logger::getLogLevel_category = LOG_CAT_NONE;
        
    LOG_CATEGORY_TAG Logger::setLogLevel_category    = LOG_CAT_NONE;
    LOG_LEVEL_TAG Logger::setLogLevel_level         = LOG_LV_NONE;

    LOG_CATEGORY_TAG Logger::putLogFatal_category     = LOG_CAT_NONE;
    int Logger::putLogFatal_id             = 0;
    std::string Logger::putLogFatal_message     = "";
    char* Logger::putLogFatal_filename     = NULL;
    int Logger::putLogFatal_lineno         = 0;

    LOG_CATEGORY_TAG Logger::putLogError_category     = LOG_CAT_NONE;
    int Logger::putLogError_id             = 0;
    std::string Logger::putLogError_message     = "";
    char* Logger::putLogError_filename     = NULL;
    int Logger::putLogError_lineno         = 0;

    LOG_CATEGORY_TAG Logger::putLogWarn_category     = LOG_CAT_NONE;
    int Logger::putLogWarn_id             = 0;
    std::string Logger::putLogWarn_message         = "";
    char* Logger::putLogWarn_filename     = NULL;
    int Logger::putLogWarn_lineno         = 0;

    LOG_CATEGORY_TAG Logger::putLogInfo_category     = LOG_CAT_NONE;
    int Logger::putLogInfo_id             = 0;
    std::string Logger::putLogInfo_message         = "";
    char* Logger::putLogInfo_filename     = NULL;
    int Logger::putLogInfo_lineno         = 0;

    LOG_CATEGORY_TAG Logger::putLogDebug_category     = LOG_CAT_NONE;
    int Logger::putLogDebug_id             = 0;
    std::string Logger::putLogDebug_message     = "";
    char* Logger::putLogDebug_filename     = NULL;
    int Logger::putLogDebug_lineno         = 0;

    

    Logger::Logger(){
    }
    
    Logger::Logger( LOG_CATEGORY_TAG cat, const unsigned int id, const std::string& msg, const char* pfile_name, int line_no){
        
        Logger_category = cat;
        Logger_id         = id;
        Logger_message  = msg;
        Logger_filename = (char*)pfile_name;
        Logger_lineno     = line_no;
        
    }
    
    Logger::~Logger(){}
    
    void Logger::loadConf(){};    
    
    LOG_LEVEL_TAG Logger::getLogLevel( LOG_CATEGORY_TAG cat){
        Logger::getLogLevel_category = cat;
        return test_loglevel;
    };
    
    bool Logger::setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level){
        Logger::setLogLevel_category    = cat;
        Logger::setLogLevel_level         = level;
        return true;
    };

    void Logger::putLogFatal( LOG_CATEGORY_TAG cat, const unsigned int id, const std::string& msg, const char* pfile_name, int line_no ){
        Logger::putLogFatal_category     = cat;
        Logger::putLogFatal_id             = id;
        Logger::putLogFatal_message     = msg;
        Logger::putLogFatal_filename     = (char*)pfile_name;
        Logger::putLogFatal_lineno         = line_no;
    };
    
    void Logger::putLogError(LOG_CATEGORY_TAG cat, const unsigned int id, const std::string& msg, const char* pfile_name, int line_no){
        Logger::putLogError_category     = cat;
        Logger::putLogError_id             = id;
        Logger::putLogError_message     = msg;
        Logger::putLogError_filename     = (char*)pfile_name;
        Logger::putLogError_lineno         = line_no;
    };
    
    void Logger::putLogWarn(LOG_CATEGORY_TAG cat, const unsigned int id, const std::string& msg, const char* pfile_name, int line_no){
        Logger::putLogWarn_category     = cat;
        Logger::putLogWarn_id             = id;
        Logger::putLogWarn_message         = msg;
        Logger::putLogWarn_filename     = (char*)pfile_name;
        Logger::putLogWarn_lineno         = line_no;
    };

    void Logger::putLogInfo(LOG_CATEGORY_TAG cat, const unsigned int id, const std::string& msg, const char* pfile_name, int line_no){
        Logger::putLogInfo_category     = cat;
        Logger::putLogInfo_id             = id;
        Logger::putLogInfo_message         = msg;
        Logger::putLogInfo_filename     = (char*)pfile_name;
        Logger::putLogInfo_lineno         = line_no;
    };
    
    void Logger::putLogDebug(LOG_CATEGORY_TAG cat, const unsigned int id, const std::string& msg, const char* pfile_name, int line_no){
        Logger::putLogDebug_category     = cat;
        Logger::putLogDebug_id             = id;
        Logger::putLogDebug_message     = msg;
        Logger::putLogDebug_filename     = (char*)pfile_name;
        Logger::putLogDebug_lineno         = line_no;
    }
    
    void Logger::initilize(LOG_LEVEL_TAG test_level){
            
        Logger::test_loglevel = test_level;
        
        Logger::Logger_category = LOG_CAT_NONE;
        Logger::Logger_id         = 0;
        Logger::Logger_message     = "";
        Logger::Logger_filename = NULL;
        Logger::Logger_lineno     = 0;
        
        Logger::getLogLevel_category = LOG_CAT_NONE;
        
        Logger::setLogLevel_category    = LOG_CAT_NONE;
        Logger::setLogLevel_level         = LOG_LV_NONE;

        Logger::putLogFatal_category     = LOG_CAT_NONE;
        Logger::putLogFatal_id             = 0;
        Logger::putLogFatal_message     = "";
        Logger::putLogFatal_filename     = NULL;
        Logger::putLogFatal_lineno         = 0;

        Logger::putLogError_category     = LOG_CAT_NONE;
        Logger::putLogError_id             = 0;
        Logger::putLogError_message     = "";
        Logger::putLogError_filename     = NULL;
        Logger::putLogError_lineno         = 0;

        Logger::putLogWarn_category     = LOG_CAT_NONE;
        Logger::putLogWarn_id             = 0;
        Logger::putLogWarn_message         = "";
        Logger::putLogWarn_filename     = NULL;
        Logger::putLogWarn_lineno         = 0;

        Logger::putLogInfo_category     = LOG_CAT_NONE;
        Logger::putLogInfo_id             = 0;
        Logger::putLogInfo_message         = "";
        Logger::putLogInfo_filename     = NULL;
        Logger::putLogInfo_lineno         = 0;

        Logger::putLogDebug_category     = LOG_CAT_NONE;
        Logger::putLogDebug_id             = 0;
        Logger::putLogDebug_message     = "";
        Logger::putLogDebug_filename     = NULL;
        Logger::putLogDebug_lineno         = 0;
    
    }
    
}// l7vs
