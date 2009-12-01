//
//!	@file	dispaatcher.cpp
//!	@brief	snmpagent - l7vsd message dispatcher
//
//	copyright(c) sdy corporation.2008
//	mail: h.okada at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <sys/time.h>
#include "dispatcher.h"
#include "store_mibdata.h"

#define SLEEPNANOTIME       (1000000)
#define DISPATCH_LOOP_COUNT (10)

/*!
 *
 */
l7ag_dispatcher::l7ag_dispatcher(){
    stop_flag   = false;
    msg_size    = 0;
    msg         = NULL;
    message_que = NULL;
    trap_que    = NULL;
}

/*!
 *
 */
l7ag_dispatcher::l7ag_dispatcher( MessageQueue* msgque, TrapQueue* trapque ){
    stop_flag   = false;
    msg_size    = 0;
    msg         = NULL;
    message_que = msgque;
    trap_que    = trapque;
}

/*!
 *
 */
l7ag_dispatcher::~l7ag_dispatcher(){
    if( NULL != msg )free( msg );
}

/*!
 *
 */
void*
l7ag_dispatcher::run( void* args ){
    l7ag_dispatcher* dispatcher = static_cast<l7ag_dispatcher*>( args );

    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = SLEEPNANOTIME;

    while (!dispatcher->stop_flag) {
        int ret = nanosleep( &req, NULL );
        if ( ret == -1 ) {
            break; // nanosleep failed
        }
        if ( !dispatcher->message_que->empty() ) {
            //まず、取得するための領域確保
            if (NULL == dispatcher->msg) {
                dispatcher->msg_size = dispatcher->message_que->front().size();
                dispatcher->msg      = static_cast<char*>(malloc( ( sizeof( char ) * dispatcher->msg_size) ) );
                memcpy( dispatcher->msg, dispatcher->message_que->front().data().get(), dispatcher->msg_size );
            }
            else {
                dispatcher->msg_pos  = dispatcher->msg_size;
                dispatcher->msg_size = dispatcher->msg_size + dispatcher->message_que->front().size();
                dispatcher->msg      = static_cast<char*>(realloc( dispatcher->msg, ( sizeof( char ) * dispatcher->msg_size) ) );
                //キューから取得(前に取得しているデータに追加取得)
                memcpy( &dispatcher->msg[dispatcher->msg_pos], dispatcher->message_que->front().data().get(), dispatcher->message_que->front().size() );
            }
            //popする
            dispatcher->message_que->pop();
            //ヘッダのサイズ参照
            l7ag_message_header* header;
            header = (l7ag_message_header*)dispatcher->msg;
            if (header->size <= dispatcher->msg_size) {
                //データサイズが十分ならディスパッチ
                dispatcher->dispatch_message();
                //ディスパッチしたら、あまりデータを前に詰める
                //いったんバックアップ
                size_t  next_size = dispatcher->msg_size - header->size;
                char*   msg_bak   = static_cast<char*>( malloc( sizeof(char)*next_size ) );
                memcpy( msg_bak, &dispatcher->msg[header->size], next_size );
                //領域のサイズ変更
                dispatcher->msg = static_cast<char*>( realloc( dispatcher->msg, sizeof(char)*next_size ) );
                //リストアする
                memcpy( dispatcher->msg, msg_bak, next_size );
                dispatcher->msg_size = next_size;
                free(msg_bak);
            }
            //データサイズが足りなければ次回持ち越し
        }
        else {
            // TODO 複数のキューでたまる速度が遅かった場合消える？
            if(NULL != dispatcher->msg){
                free( dispatcher->msg );
                dispatcher->msg = NULL;
            }
        }
    }

    dispatcher->stop_flag = false;

    return NULL;
}

/*!
 *
 */
bool
l7ag_dispatcher::start(){
    bool    retbool = true;
    if ( 0 != pthread_create( &dispatch_thread, NULL, run, this ) ) {
        retbool = false;
    }
    return  retbool;
}

/*!
 *
 */
bool
l7ag_dispatcher::stop(){
    bool    retbool       = true;
//    void*   thread_return = NULL;

    stop_flag = true;
    //join
//    if ( 0 != pthread_join( dispatch_thread, &thread_return ) ) {
    if ( 0 != pthread_join( dispatch_thread, NULL ) ) {
        //join error
        retbool = false;
    }
    return  retbool;
}

/*!
 * ディスパッチ入口
 */
void
l7ag_dispatcher::dispatch_message(){
    if ( dispatch_functions.empty() ) {
        init_dispatcher_function_map();
    }

    char*   p_message = msg;
    l7ag_message_header*    msg_header = reinterpret_cast<l7ag_message_header*>( msg );
    if (msg_header->magic[0] != 0x4d || msg_header->magic[1] != 0x47) return;
    
    //まず最初のpayload_headerにポインタをずらず
    p_message = p_message + sizeof( l7ag_message_header );
    
    for (unsigned long long i = 0; i < msg_header->payload_count; ++i) {
        //payload_headerとpayloadは1:1
        //payload_headerのmessage_idでディスパッチ先を決める
        l7ag_payload_header* payload_header = reinterpret_cast<l7ag_payload_header*>(p_message);
        if (payload_header->magic[0] != 0x50 || payload_header->magic[1] != 0x59) return;
        //payloadにポインタをずらす
        char* payload = p_message + sizeof( l7ag_payload_header );
        
        std::map< int, boost::function<void(void*)> >::iterator it = dispatch_functions.find( payload_header->message_id );
        if( dispatch_functions.end() != it ){
            it->second( payload );
        }
        //次のデータにポインタをずらす
        p_message = p_message + payload_header->payload_datasize;
    }
}

/*!
 * 設定コマンド
 */
void
l7ag_dispatcher::process_setting_command( void* p_data ){
    l7ag_settingcommand_message* command = (l7ag_settingcommand_message*)p_data;
    //コマンド種別をみる
    if (COMMAND_LOGLEVEL_CHANGE == command->command_id) {
    //ログレベル変更
        l7ag_changeloglevel_parameter*  param = reinterpret_cast<l7ag_changeloglevel_parameter*>(&command->data[0]);
        change_loglevel( param->log_category, param->log_level );
    }
    else if (COMMAND_SETTINGFILE_RELOAD ==command->command_id) {
    //設定ファイル再読み込み
        reload_configure();
    }
}

/*!
 * Trap送信リクエスト
 */
void
l7ag_dispatcher::process_trap_request( void* p_data ){
    l7ag_traprequest_message* command = (l7ag_traprequest_message*)p_data;
    trapdata    trap_req;
    //OIDを取得
    char* data_oid = static_cast<char*>( calloc( (OIDDATASIZE+1), sizeof(char) ) );
    memcpy( data_oid, command->oid, OIDDATASIZE );
    trap_req.oid   = data_oid;
    //messageを取得
    char* data_msg = static_cast<char*>( calloc( (TRAPREQUESTMESSAGESIZE+1), sizeof(char) ) );
    memcpy( data_msg, command->message, TRAPREQUESTMESSAGESIZE );
    trap_req.message = data_msg;
    trap_que->push( trap_req );
}

/*!
 * MIBデータ収集結果(VirtualService)
 */
void
l7ag_dispatcher::process_mib_collect_response_vs( void* p_data ){
    bool    writeflag = false;
    l7ag_mibdata_payload_vs* command = (l7ag_mibdata_payload_vs*)p_data;
    if (command->magic[0] != 0x56 || command->magic[1] != 0x53) return;
    if (command->vs_count > 0) {
        int vscount = l7ag_store_mibdata::getInstance().getVSdatacount();
        for (int i = 0; i < vscount; ++i) {
            if ( l7ag_store_mibdata::getInstance().getVSmibdata( i )->index == command->index ) {
                l7ag_store_mibdata::getInstance().updateVSmibdata( i, command );
                writeflag = true;
            }
        }
        if (!writeflag) {
            l7ag_store_mibdata::getInstance().addVSmibdata( command );
        }
        if (command->index == command->vs_count) {
            // delete other index
            l7ag_store_mibdata::getInstance().updateVSmibdata(command->vs_count, NULL);
        }
    }
    else {
        l7ag_store_mibdata::getInstance().clearVSmibdata();
    }
}

/*!
 * MIBデータ収集結果(RealServer)
 */
void
l7ag_dispatcher::process_mib_collect_response_rs( void* p_data ){
    bool    writeflag = false;
    l7ag_mibdata_payload_rs* command = (l7ag_mibdata_payload_rs*)p_data;
    if (command->magic[0] != 0x52 || command->magic[1] != 0x53) return;
    if (command->rs_count > 0) {
        int rscount = l7ag_store_mibdata::getInstance().getRSdatacount();
        for (int i = 0; i < rscount; ++i) {
            if ( l7ag_store_mibdata::getInstance().getRSmibdata( i )->index == command->index ) {
                l7ag_store_mibdata::getInstance().updateRSmibdata( i, command );
                writeflag = true;
            }
        }
        if (!writeflag) {
            l7ag_store_mibdata::getInstance().addRSmibdata( command );
        }
        if (command->index == command->rs_count) {
            // delete other index
            l7ag_store_mibdata::getInstance().updateRSmibdata(command->rs_count, NULL);
        }
    }
    else {
        l7ag_store_mibdata::getInstance().clearRSmibdata();
    }
}

/*!
 *
 */
void
l7ag_dispatcher::change_loglevel( unsigned long long category, unsigned long long loglevel ){
	l7vs::Logger	logger;
	logger.setLogLevel( static_cast<l7vs::LOG_CATEGORY_TAG>(category), static_cast<l7vs::LOG_LEVEL_TAG>(loglevel) );
}

/*!
 *
 */
void
l7ag_dispatcher::reload_configure(){
	l7vs::Parameter		parameter;
	parameter.read_file( l7vs::PARAM_COMP_SNMPAGENT );
}

void
l7ag_dispatcher::init_dispatcher_function_map(){
    dispatch_functions.clear();
    
    dispatch_functions[MESSAGE_ID_COMMANDREQUEST]        = boost::bind( &l7ag_dispatcher::process_setting_command, this, _1 );
    dispatch_functions[MESSAGE_ID_TRAPREQUEST]           = boost::bind( &l7ag_dispatcher::process_trap_request, this, _1 );
    dispatch_functions[MESSAGE_ID_MIBCOLLECTRESPONSE_VS] = boost::bind( &l7ag_dispatcher::process_mib_collect_response_vs, this, _1 );
    dispatch_functions[MESSAGE_ID_MIBCOLLECTRESPONSE_RS] = boost::bind( &l7ag_dispatcher::process_mib_collect_response_rs, this, _1 );
}
