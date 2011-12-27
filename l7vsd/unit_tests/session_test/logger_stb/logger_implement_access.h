#ifndef LOGGER_IMPL_ACCESS_H
#define LOGGER_IMPL_ACCESS_H

//#include <sstream>
//#include <map>
//#include <log4cxx/logger.h>
//#include <log4cxx/level.h>
//#include <log4cxx/net/syslogappender.h>
//#include <log4cxx/fileappender.h>
//#include <log4cxx/rollingfileappender.h>
//#include <log4cxx/patternlayout.h>
#include <boost/format.hpp>
//#include <boost/tr1/unordered_map.hpp>
//#include <boost/foreach.hpp>
//#include "appender_property.h"

#define LOGGER_ACCESS_PROCESS_ID "ACCESS"


#define LOGGER_NULL "/dev/null"

#define LOGGER_LEVEL_NUM (6)

#define LOG_CAT_L7VSD_ACCESS_LOGGER "l7vsd_access_logger"

//namespace log4cxx
//{
//  typedef helpers::ObjectPtrT<RollingFileAppender> RollingFileAppenderPtr;
//}

namespace l7vs
{

class logger_implement_access
{
public:

//    typedef std::map< std::string , std::string > accesslog_rotate_map_type;

        logger_implement_access(const std::string &aclogFilename);
        virtual ~logger_implement_access() {};

        //! initialze function
//  virtual bool init(appender_property& access_log_default_property,const std::map<std::string,std::string>& rotate);

        /*!
         * output info log.
         *
         * @param   category that logging matter occured
         * @param   log message id
         * @param   log message
         * @param   current file
         * @param   current line
         * @retrun  void
         */
        virtual inline void putLog(
                const std::string &vsinfo,
                const std::string &cl_con_org,
                const std::string &rs_con_org,
                const std::string &rs_con_dest,
                const std::string &msg) {
                putLog_vsinfo = vsinfo;
                putLog_cl_con_org = cl_con_org;
                putLog_rs_con_org = rs_con_org;
                putLog_rs_con_dest = rs_con_dest;
                putLog_msg = msg;
        };

        std::string putLog_vsinfo;
        std::string putLog_cl_con_org;
        std::string putLog_rs_con_org;
        std::string putLog_rs_con_dest;
        std::string putLog_msg;

//    void  addRef();

//    void  releaseRef();

//    bool  operator<=(const int access_num );

//    std::string getAcLogFileName(){ return( this->acLogFileName ); }

//    bool checkRotateParameterComp(accesslog_rotate_map_type &rotatedata);

//    bool is_rotate_default_flag();

protected:


//    int access_cnt;

//    std::string acLogFileName;

        //! initialized flag
//  bool initialized;

        //! hostname
//    appender_property access_log_property;

//    accesslog_rotate_map_type aclog_args;

//    bool rotate_default_flag;

//    bool setAcLoggerConf(appender_property& access_log_default_property,const accesslog_rotate_map_type& rotate);

};

};

#endif //   LOGGER_IMPL_ACCESS_H
