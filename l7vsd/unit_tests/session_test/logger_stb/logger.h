
#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <logger_enum.h>
namespace l7vs{




//! @class Logger
//! @brief Logger creation class.
//! @brief this class create Logger Inmlement class and mediate function.
class Logger{
//    protected:
//        const LOG_CATEGORY_TAG    scopedLogCategory;
//        const unsigned int        scopedLogId;
//        const std::string        scopedLogMessage;
//        const char*                scopedLogFile;
//        const int                scopedLogLine;

    public:
    //! default constructor creates implement class.
        Logger();
        
    //! log output constractor
    //! scoped log output( Logger destractor output log)
    //! @param[in]    Log output category tag
    //! @param[in]    log id
    //! @param[in]    log message
    //!    @param[in]    filename    (=__FILE__)
    //! @param[in]    lineno        (=__LINE__)
        Logger( LOG_CATEGORY_TAG, const unsigned int, const std::string&, const char*, int);
        static LOG_CATEGORY_TAG Logger_category;
        static int Logger_id;
        static std::string Logger_message;
        static char* Logger_filename;
        static int Logger_lineno;
        
    //! destructor.(output log then use log output constractor)
        ~Logger();
        
        static void initilize(LOG_LEVEL_TAG test_level);
        
        static LOG_LEVEL_TAG test_loglevel;
        
    //! load Configuration.
        void loadConf();

    //! retrieve category's log level.
    //! @param[in]    logcategory
    //! @return        loglevel
        static LOG_LEVEL_TAG getLogLevel( LOG_CATEGORY_TAG );
        static LOG_CATEGORY_TAG     getLogLevel_category;

    //! set category's log level.
    //!    @param[in]    logcategory
    //!    @param[in]    loglevel
    //!    @return true    success loglevel change
    //!    @return    false    failer loglevel change
        static bool setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level);
        static LOG_CATEGORY_TAG     setLogLevel_category;
        static LOG_LEVEL_TAG         setLogLevel_level;

    //! output fatal log.
    //! @param[in]    Log output category tag
    //! @param[in]    log id
    //! @param[in]    log message
    //!    @param[in]    filename    (=__FILE__)
    //! @param[in]    lineno        (=__LINE__)
        static void putLogFatal( LOG_CATEGORY_TAG, const unsigned int, const std::string&, const char*, int );
        static LOG_CATEGORY_TAG     putLogFatal_category;
        static int                     putLogFatal_id;
        static std::string             putLogFatal_message;
        static char*                 putLogFatal_filename;
        static int                     putLogFatal_lineno;

    //! output error log.
    //! @param[in]    Log output category tag
    //! @param[in]    log id
    //! @param[in]    log message
    //!    @param[in]    filename    (=__FILE__)
    //! @param[in]    lineno        (=__LINE__)
        static void putLogError(LOG_CATEGORY_TAG, const unsigned int, const std::string&, const char*, int );
        static LOG_CATEGORY_TAG     putLogError_category;
        static int                     putLogError_id;
        static std::string             putLogError_message;
        static char*                 putLogError_filename;
        static int                     putLogError_lineno;

    //! output warn log.
    //! @param[in]    Log output category tag
    //! @param[in]    log id
    //! @param[in]    log message
    //!    @param[in]    filename    (=__FILE__)
    //! @param[in]    lineno        (=__LINE__)
        static void putLogWarn(LOG_CATEGORY_TAG, const unsigned int, const std::string&, const char*, int );
        static LOG_CATEGORY_TAG     putLogWarn_category;
        static int                     putLogWarn_id;
        static std::string             putLogWarn_message;
        static char*                 putLogWarn_filename;
        static int                     putLogWarn_lineno;

    //! output info log.
    //! @param[in]    Log output category tag
    //! @param[in]    log id
    //! @param[in]    log message
    //!    @param[in]    filename    (=__FILE__)
    //! @param[in]    lineno        (=__LINE__)
        static void putLogInfo(LOG_CATEGORY_TAG, const unsigned int, const std::string&, const char*, int );
        static LOG_CATEGORY_TAG     putLogInfo_category;
        static int                     putLogInfo_id;
        static std::string             putLogInfo_message;
        static char*                 putLogInfo_filename;
        static int                     putLogInfo_lineno;

    //! output debug log.
    //! @param[in]    Log output category tag
    //! @param[in]    log id
    //! @param[in]    log message
    //!    @param[in]    filename    (=__FILE__)
    //! @param[in]    lineno        (=__LINE__)
        static void putLogDebug(LOG_CATEGORY_TAG, const unsigned int, const std::string&, const char*, int );
        static LOG_CATEGORY_TAG     putLogDebug_category;
        static int                     putLogDebug_id;
        static std::string             putLogDebug_message;
        static char*                 putLogDebug_filename;
        static int                     putLogDebug_lineno;
        
};

}    //namespace l7vs

#endif    //LOGGER_H
