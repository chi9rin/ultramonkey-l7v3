/*!
 * @file  logger_access_manager.cpp
 * @brief logger module implementation class For access log instance manager.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#include "logger_access_manager.h"
#include "logger_logrotate_utility.h"
#include "parameter.h"

/*!
 * initialize function.
 *
 * @retrun  current instance.
 */
l7vs::logger_access_manager &l7vs::logger_access_manager::getInstance()
{
        static logger_access_manager log_manager;
        return(log_manager);
}

/*!
 * constructor initialize member variables.
 *
 * @param   access log file name
 */
l7vs::logger_access_manager::logger_access_manager()
        : rotate_default_load_flag(false) ,
          rotate_default_verbose_displayed_contents("")
{

        logimp_access_map.clear();
        access_log_default_data.clear();

}

/*!
 * destructor.
 */
l7vs::logger_access_manager::~logger_access_manager()
{

        logimp_access_map.clear();
        access_log_default_data.clear();

}

/*!
 * find or create logger_implement_access class function.
 *
 * @param   access log file name
 * @param   logrotation designation contents
 * @param   error_code
 * @retrun  void.
 */
l7vs::logger_implement_access *l7vs::logger_access_manager::find_logger_implement_access(
        const std::string &access_log_filename,
        std::map< std::string , std::string > rotatedata,
        l7vs::error_code &err)
{
        err.setter(false, "");
        bool init_flag;

        rd_scoped_lock scope_lock(log_ac_flag_mutex);

        logger_implement_access *logger_access_instance = NULL;

        logimp_access_map_type::iterator itr = logimp_access_map.find(access_log_filename);
        if (itr == logimp_access_map.end()) {
                logger_access_instance = new logger_implement_access(access_log_filename);

                if (rotatedata.size() > 0) {
                        init_flag = logger_access_instance->init(true , access_log_default_property , rotatedata);
                } else {
                        init_flag = logger_access_instance->init(false , access_log_default_property , access_log_default_data);
                }

                if (init_flag == false) {
                        err.setter(true, "logger_implement_access initialize err.");
                        delete logger_access_instance;
                        return(NULL);
                }

                logimp_access_map.insert(make_pair(access_log_filename , logger_access_instance));
        } else {
                logger_access_instance = itr->second;

                bool rotate_comp_result = logger_access_instance->checkRotateParameterComp(rotatedata);

                if (rotate_comp_result == true) {
                        logger_access_instance->addRef();
                } else {
                        err.setter(true, "logger_implement_access rotation info not equal err.");
                        logger_access_instance = NULL;
                }

        }

        return(logger_access_instance);

}

/*!
 * erase logger_implement_access class function.
 *
 * @param   access log file name
 * @param   error_code
 * @retrun  void.
 */
void l7vs::logger_access_manager::erase_logger_implement_access(
        const std::string &access_log_filename,
        l7vs::error_code &err)
{
        err.setter(false, "");

        logger_implement_access *logger_access_instance = NULL;

        rw_scoped_lock scope_lock(log_ac_flag_mutex);

        logimp_access_map_type::iterator itr = logimp_access_map.find(access_log_filename);
        if (itr == logimp_access_map.end()) {
                err.setter(true, "access log erase instance find err.");
                return;
        } else {
                logger_access_instance = itr->second;
                logger_access_instance->releaseRef();
        }

        if ((*logger_access_instance) <= 0) {
                logimp_access_map.erase(access_log_filename);
                delete logger_access_instance;
        }

}

/*!
 * log rotate parameter check function.
 *
 * @param   logrotation designation contents
 * @retrun  false failed.
 */
bool l7vs::logger_access_manager::access_log_logrotate_parameter_check(
        const std::map<std::string, std::string>& rotatedata)
{
        appender_property access_log_property;
        std::map<std::string, std::string> rotatedata_cpy = rotatedata;

        return(logger_logrotate_utility::access_log_LogrotateParamCheck(rotatedata_cpy , access_log_property));
}

/*!
 * load logrotation info from l7vs.cf.
 *
 * @retrun  void.
 */
void l7vs::logger_access_manager::access_log_rotate_loadConf()
{
        using namespace log4cxx;
        using namespace l7vs;

        Parameter param;

        std::string rotation_type;
        std::string max_backup_index;
        std::string max_file_size;
        std::string rotation_timing;
        std::string rotation_timing_value_key;

        LOG_ROTATION_TAG rotation_type_data = LOG_ROT_SIZE;
        unsigned int max_backup_index_data = 0;
        unsigned long long max_file_size_data = 0;
        LOG_ROTATION_TIMING_TAG rotation_timing_data = LOG_TIM_YEAR;
        std::string rotation_timing_key_data = "";

        l7vs::error_code ec;

        rotate_default_verbose_displayed_contents = "";

        rotation_type = param.get_string(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_KEY, ec);
        if (ec) {
                logger_logrotate_utility::logrotation_utility_logic_error(110, "Not Exist Log Rotation Setting.", __FILE__, __LINE__);
        }

        rotate_default_verbose_displayed_contents = rotate_default_verbose_displayed_contents + ACCESS_LOG_ROTATE_TYPE + " " + rotation_type;

        access_log_default_data[ ACCESS_LOG_ROTATE_TYPE ] = rotation_type;

        rotation_type_data = logger_logrotate_utility::check_rotate_type(rotation_type);

        max_backup_index = param.get_string(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_BACKUP_INDEX_KEY, ec);
        if (ec) {
                logger_logrotate_utility::logrotation_utility_logic_error(111, "Not Exist Log MaxBackupIndex Setting.", __FILE__, __LINE__);
        }

        rotate_default_verbose_displayed_contents = rotate_default_verbose_displayed_contents + " " + ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX + " " + max_backup_index;

        access_log_default_data[ ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX ] = max_backup_index;

        max_backup_index_data = logger_logrotate_utility::check_max_backup_index(max_backup_index);

        if (rotation_type_data == LOG_ROT_SIZE) {

                max_file_size = param.get_string(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY, ec);
                if (ec) {
                        logger_logrotate_utility::logrotation_utility_logic_error(112, "Not Exist Log MaxFileSize Setting.", __FILE__, __LINE__);
                }

                rotate_default_verbose_displayed_contents = rotate_default_verbose_displayed_contents + " " + ACCESS_LOG_ROTATE_MAX_FILESIZE + " " + max_file_size;

                access_log_default_data[ ACCESS_LOG_ROTATE_MAX_FILESIZE ] = max_file_size;

                max_file_size_data = logger_logrotate_utility::check_max_file_size(max_file_size);

        } else if (rotation_type_data == LOG_ROT_DATE) {

                rotation_timing = param.get_string(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY, ec);
                if (ec) {
                        logger_logrotate_utility::logrotation_utility_logic_error(113, "Not Exist Log RotationTiming Setting.", __FILE__, __LINE__);
                }

                rotate_default_verbose_displayed_contents = rotate_default_verbose_displayed_contents + " " + ACCESS_LOG_ROTATE_ROTATION_TIMING + " " + rotation_timing;

                access_log_default_data[ ACCESS_LOG_ROTATE_ROTATION_TIMING ] = rotation_timing;

                rotation_timing_data = logger_logrotate_utility::check_rotate_timing(rotation_timing);

                rotation_timing_value_key = param.get_string(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY, ec);
                if (ec) {
                        std::stringstream   ss;
                        ss << "Not Exist Log RotationTiming " << rotation_timing << " Setting.";
                        logger_logrotate_utility::logrotation_utility_logic_error(114, ss.str(), __FILE__, __LINE__);
                }

                rotate_default_verbose_displayed_contents = rotate_default_verbose_displayed_contents + " " + ACCESS_LOG_ROTATION_TIMING_VALUE + " " + rotation_timing_value_key;

                access_log_default_data[ ACCESS_LOG_ROTATION_TIMING_VALUE ] = rotation_timing_value_key;

                rotation_timing_key_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing_data);

        } else {

                max_file_size = param.get_string(PARAM_COMP_LOGGER, ACCESS_LOG_MAX_FILE_SIZE_KEY, ec);
                if (ec) {
                        logger_logrotate_utility::logrotation_utility_logic_error(115, "Not Exist Log MaxFileSize Setting.", __FILE__, __LINE__);
                }

                rotate_default_verbose_displayed_contents = rotate_default_verbose_displayed_contents + " " + ACCESS_LOG_ROTATE_MAX_FILESIZE + " " + max_file_size;

                access_log_default_data[ ACCESS_LOG_ROTATE_MAX_FILESIZE ] = max_file_size;

                max_file_size_data = logger_logrotate_utility::check_max_file_size(max_file_size);

                rotation_timing = param.get_string(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_KEY, ec);
                if (ec) {
                        logger_logrotate_utility::logrotation_utility_logic_error(116, "Not Exist Log RotationTiming Setting.", __FILE__, __LINE__);
                }

                rotate_default_verbose_displayed_contents = rotate_default_verbose_displayed_contents + " " + ACCESS_LOG_ROTATE_ROTATION_TIMING + " " + rotation_timing;

                access_log_default_data[ ACCESS_LOG_ROTATE_ROTATION_TIMING ] = rotation_timing;

                rotation_timing_data = logger_logrotate_utility::check_rotate_timing(rotation_timing);

                rotation_timing_value_key = param.get_string(PARAM_COMP_LOGGER, ACCESS_LOG_ROTATION_TIMING_VALUE_KEY, ec);
                if (ec) {
                        std::stringstream   ss;
                        ss << "Not Exist Log RotationTiming " << rotation_timing << " Setting.";
                        logger_logrotate_utility::logrotation_utility_logic_error(117, ss.str(), __FILE__, __LINE__);
                }

                rotate_default_verbose_displayed_contents = rotate_default_verbose_displayed_contents + " " + ACCESS_LOG_ROTATION_TIMING_VALUE + " " + rotation_timing_value_key;

                access_log_default_data[ ACCESS_LOG_ROTATION_TIMING_VALUE ] = rotation_timing_value_key;

                rotation_timing_key_data = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key, rotation_timing_data);

        }

        access_log_default_property.rotation_value = rotation_type_data;
        access_log_default_property.max_backup_index_value = max_backup_index_data;
        access_log_default_property.max_file_size_value = max_file_size_data;
        access_log_default_property.rotation_timing_value = rotation_timing_data;
        access_log_default_property.rotation_timing_value_value = rotation_timing_key_data;


}

/*!
 * member variable data getter.
 *
 * @retrun  default rotate info str
 */
std::string l7vs::logger_access_manager::get_rotate_default_verbose_displayed_contents()
{
        return(rotate_default_verbose_displayed_contents);
}

