#ifndef    STUB_H
#define    STUB_H

#include "virtualservice.h"

#define    SESSION_POOL_NUM_PARAM    (512)

class    debugg_flug_struct{
protected:
    bool    session_pool_param_flg;
    bool    session_loop_mode;
    bool    pmcontrol_error_flag;
    bool    smcontrol_error_flag;
    bool    pm_function_called;
    l7vs::LOG_LEVEL_TAG    loglevel;

    int        pm_rep_count;
    int        sm_rep_count;

    char*    rep_area;

    debugg_flug_struct() : session_pool_param_flg( false ),
                                pm_rep_count( 0 ),
                                sm_rep_count( 0 ) {
        loglevel = l7vs::LOG_LV_ERROR;
        session_loop_mode = true;
    }
    debugg_flug_struct( const debugg_flug_struct & in ){ rep_area = NULL; }
    debugg_flug_struct& operator=( const debugg_flug_struct & in ){ return *this; }
public:
    static debugg_flug_struct&    getInstance(){
        static    debugg_flug_struct    instance;
        return    instance;
    }
    bool&    session_loop_flag(){
        return session_loop_mode;
    }
    l7vs::LOG_LEVEL_TAG&    stub_loglevel(){
        return loglevel;
    }
    bool&    param_exist_flag(){
        return session_pool_param_flg;
    }
    bool&    pmcontrol_err_flag(){
        return pmcontrol_error_flag;
    }
    bool&    smcontrol_err_flag(){
        return smcontrol_error_flag;
    }
    bool&    pm_function_called_flag(){
        return pm_function_called;
    }
    void    pm_rep_count_inc(){
        pm_rep_count = pm_rep_count+1;
    }
    int        get_pm_rep_count(){
        return pm_rep_count;
    }
    void    sm_rep_count_inc(){
        sm_rep_count = sm_rep_count+1;
    }
    int        get_sm_rep_count(){
        return sm_rep_count;
    }
    void    create_rep_area(){
        if( NULL == rep_area )
            rep_area = new char[(l7vs::virtualservice_base::MAX_REPLICATION_DATA_NUM)*480];
            memset( rep_area, 0, (sizeof(char)*480*l7vs::virtualservice_base::MAX_REPLICATION_DATA_NUM) );
    }
    void*    get_rep_area(){
        return reinterpret_cast<void*>( rep_area );
    }
    void    destroy_rep_area(){
        if( NULL != rep_area ){
            delete [] rep_area;
            rep_area = NULL;
        }
    }
};

#endif
