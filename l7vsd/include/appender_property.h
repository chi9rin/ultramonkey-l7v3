#ifndef _APPENDER_PROPERTY
#define _APPENDER_PROPERTY

#include <string>
#include "logger_rotation_enum.h"


namespace l7vs
{

struct appender_property {
        std::string     log_filename_key;           //!< parameter logfile key
        std::string     log_filename_value;         //!< parameter logfile value
        std::string     rotation_key;               //!< parameter rotation kind key
        LOG_ROTATION_TAG
        rotation_value;             //!< parameter rotation kind value
        std::string     max_backup_index_key;       //!< parameter backup maximum number key
        unsigned int    max_backup_index_value;     //!< parameter backup maximum number value
        std::string     max_file_size_key;          //!< parameter maximum backup size key
        unsigned long long
        max_file_size_value;    //!< parameter maximum backup size value
        std::string     rotation_timing_key;        //!< parameter rotation timing key
        LOG_ROTATION_TIMING_TAG
        rotation_timing_value;      //!< parameter rotation timing value

        std::string     rotation_timing_value_key;  //!< parameter rotation timing value key
        std::string     rotation_timing_value_value;//!< parameter rotation timing value value
        appender_property() :                       //!< constructor
                rotation_value(LOG_ROT_SIZE),
                max_backup_index_value(0),
                max_file_size_value(0),
                rotation_timing_value(LOG_TIM_YEAR)
        {}
};

};

#endif // _APPENDER_PROPERTY

