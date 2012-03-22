/*!
 * @file  protocol_module_control.cpp
 * @brief control load/unload shared object protocol module.
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
#include    <dlfcn.h>
#include    "protocol_module_control.h"
#include    "logger.h"
#include    "parameter.h"

#define L7VS_MODULE_INITFN "create_module"
#define L7VS_MODULE_FINIFN "destroy_module"

namespace l7vs
{

#if defined(LOGGER_PROCESS_VSD)
const LOG_CATEGORY_TAG loggerCategory = LOG_CAT_L7VSD_MODULE;
const LOG_CATEGORY_TAG loggerCategorySysMem = LOG_CAT_L7VSD_SYSTEM_MEMORY;
#else
const LOG_CATEGORY_TAG loggerCategory = LOG_CAT_L7VSADM_MODULE;
const LOG_CATEGORY_TAG loggerCategorySysMem = LOG_CAT_L7VSADM_COMMON;
#endif

/*!
 * returns singleton instance.
 *
 * @param   void
 * @return  instance
 */
protocol_module_control&
protocol_module_control::getInstance()
{
        Logger logger(loggerCategory, 99999, "protocol_module_control::getInstance", __FILE__, __LINE__);
        static    protocol_module_control    instance;
        return    instance;
}

/*!
 * initialize class.
 *
 * @param   file path
 * @return  void
 */
void
protocol_module_control::initialize(const std::string &infile_path)
{
        Logger logger(loggerCategory, 99999, "protocol_module_control::initialize", __FILE__, __LINE__);
        if (&infile_path != NULL) {
                module_control_base::modulefile_path    = infile_path;
        }
}

/*!
 * finalize class.
 *
 * @param   void
 * @return  void
 */
void
protocol_module_control::finalize()
{
        Logger logger(loggerCategory, 99999, "protocol_module_control::finalize", __FILE__, __LINE__);
}

/*!
 * load shared object.
 *
 * @param   module name
 * @param   logging function
 * @return  pointer for protocol_module_base class instance
 */
protocol_module_base*
protocol_module_control::load_module(const    std::string &modulename)
{
        Logger logger(loggerCategory, 99999, "protocol_module_control::load_module", __FILE__, __LINE__);
        protocol_module_base *return_value = NULL;
        boost::mutex::scoped_lock lock(loadmodule_map_mutex);
        name_module_info_map::iterator it = loadmodule_map.find(modulename);
        if (it == loadmodule_map.end()) {
                std::string load_module_name = modulefile_path + "/protomod_" + modulename + ".so";
                void *h = dlopen(load_module_name.c_str(), RTLD_LAZY);
                if (h == NULL) {
                        std::string msg = "Could not open " + load_module_name + " module: " + dlerror();
                        Logger::putLogError(loggerCategory, 1, msg, __FILE__, __LINE__);
                        return NULL;
                }

                protocol_module_base*(*create_func)(void);
                create_func = (protocol_module_base * ( *)(void))dlsym(h, L7VS_MODULE_INITFN);
                if (create_func == NULL) {
                        std::stringstream buf;
                        buf << "Could not find symbol " << L7VS_MODULE_INITFN << ": " << dlerror();
                        Logger::putLogError(loggerCategory, 2, buf.str(), __FILE__, __LINE__);
                        dlclose(h);
                        return NULL;
                }
                void (*destroy_func)(protocol_module_base *);
                destroy_func = (void ( *)(protocol_module_base *))dlsym(h, L7VS_MODULE_FINIFN);
                if (destroy_func == NULL) {
                        std::stringstream buf;
                        buf << "Could not find symbol " << L7VS_MODULE_FINIFN << ": " << dlerror();
                        Logger::putLogError(loggerCategory, 3, buf.str(), __FILE__, __LINE__);
                        dlclose(h);
                        return NULL;
                }
                protocol_module_control::protocol_module_info module_info;
                module_info.handle = h;
                module_info.create_func = create_func;
                module_info.destroy_func = destroy_func;
                loadmodule_map.insert(std::pair< std::string, protocol_module_control::protocol_module_info >(modulename, module_info));
                it = loadmodule_map.find(modulename);
        }
        if (it != loadmodule_map.end()) {
                return_value = it->second.create_func();
                if (return_value != NULL) {
                        it->second.ref_count++;
                } else {
                        std::string msg = "Module initialization failed.";
                        Logger::putLogError(loggerCategory, 4, msg, __FILE__, __LINE__);
                }
        }
        return return_value;
}

/*!
 * unload shared object.
 *
 * @param   module name
 * @param   pointer for protocol_module_base class instance
 * @return  void
 */
void
protocol_module_control::unload_module(protocol_module_base *module_ptr)
{
        Logger logger(loggerCategory, 99999, "protocol_module_control::unload_module", __FILE__, __LINE__);
        if (module_ptr == NULL) {
                std::string msg = "Arg(module_ptr) is NULL pointer.";
                Logger::putLogError(loggerCategory, 5, msg, __FILE__, __LINE__);
                return;
        }

        boost::mutex::scoped_lock lock(loadmodule_map_mutex);
        name_module_info_map::iterator it = loadmodule_map.find(module_ptr->get_name());
        if (it == loadmodule_map.end()) {
                std::string msg = "module name is not found.";
                Logger::putLogError(loggerCategory, 6, msg, __FILE__, __LINE__);
                return;
        }
        it->second.destroy_func(module_ptr);
        it->second.ref_count--;
        if (it->second.ref_count == 0) {
                dlclose(it->second.handle);
                loadmodule_map.erase(it);
        }
}

}
