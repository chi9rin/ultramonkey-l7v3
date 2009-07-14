//#define _DEBUG_MODE_

#include <boost/thread/thread.hpp>
#include "l7vsd_threadtest.h"
#include "tcp_session_socket_model.h"

 
namespace l7vs{

	//! construcor
	tcp_session_socket_model::tcp_session_socket_model(virtualservice_tcp* pService,boost::asio::io_service& io,boost::asio::ip::tcp::endpoint rs_endpoint):
                tcp_session_base(pService,io,rs_endpoint),
                threadA_state(0),
                threadB_state(0){
                
	}

	//! destructor
	tcp_session_socket_model::~tcp_session_socket_model(){
	}

        void tcp_session_socket_model::Run_main(){
                Run_cl();
                pVs->release_session(this);
        };
        void tcp_session_socket_model::Run_sub(){
                Run_rs();
        };


        void tcp_session_socket_model::Run_cl(){

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_socket_model::Run_cl start\n";
#endif

                socket_data* pdata = NULL;
                socket_data* pmsg = NULL;
                boost::system::error_code ec;

                {
                        rw_scoped_lock scope_lock(threadA_state_mutex);
                        threadA_state = 1;
                }

                //RSスレッド待機
                while(true){
                        {
                                rd_scoped_lock scope_lock(threadB_state_mutex);
                                if(threadB_state == 1){
                                        break;
                                }
                        }
                        boost::this_thread::yield();
                }


                // ノンブロッキングに設定
                boost::asio::socket_base::non_blocking_io cmd(true);
                cl_socket.io_control(cmd,ec);
                if(ec){
                        //ノンブロッキング設定失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                        std::cout << "tcp_session_socket_model::Run_cl cl_socket.io_control error[";
                        std::cout << ec.message();
                        std::cout << "]\n";
#endif
                        if(pdata == NULL){
                                pdata = new socket_data();
                        }
                        pdata->data_size = 0;
                        pdata->msg_type = 0;
                        while(!cl2rs_msg.push(pdata)){}
                        pdata = NULL;
                }else{
#ifdef _DEBUG_MODE_
                        std::cout << "tcp_session_socket_model::Run_cl Loop start\n";
#endif
                        while(true){// CLスレッドメインループ
                                boost::this_thread::yield();

                                // 受信
                                if(pdata == NULL){
                                        pdata = new socket_data();
                                }
                                size_t recv_size = cl_socket.read_some(boost::asio::buffer(pdata->buff.data(),65535), ec);
                                if(!ec){
                                         if(recv_size > 0){
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_socket_model::Run_cl cl_socket.read_some receive data size[";
                                                std::cout << recv_size;
                                                std::cout << "]\n";
#endif
                                                if(!rs_socket.is_open()){
                                                        //RSが接続されてないので、接続要求を投げる
                                                        socket_data* pConData = new socket_data();
                                                        pConData->msg_type = 2;
                                                        while(!cl2rs_msg.push(pConData)){}
                                                        pConData = NULL;
                                                }
                                                pdata->data_size = recv_size;
                                                pdata->msg_type = 1;
                                                while(!cl2rs_msg.push(pdata)){}
                                                pdata = NULL;
                                        }
                                }else{
                                        if(ec != boost::asio::error::try_again){
                                                //受信失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_socket_model::Run_cl cl_socket.read_some error[";
                                                std::cout << ec.message();
                                                std::cout << "]\n";
#endif
                                                pdata->data_size = 0;
                                                pdata->msg_type = 0;
                                                while(!cl2rs_msg.push(pdata)){}
                                                pdata = NULL;
                                                break;
                                        }else{
                                                ec.clear();
                                        }
                                }


                                //　CL要求チェック
                                while(true){
                                        boost::this_thread::yield();
                                        pmsg = rs2cl_msg.pop();
                                        if(pmsg == NULL){
                                                break;
                                        }
                                        if(pmsg->msg_type == 1){
                                                // 送信
                                                size_t send_size = 0;
                                                while(send_size < pmsg->data_size){
                                                        size_t ret_size = cl_socket.write_some(boost::asio::buffer(pmsg->buff.data() + send_size,pmsg->data_size - send_size), ec);
                                                        if(!ec){
                                                        // 送信成功
                                                                send_size += ret_size;
#ifdef _DEBUG_MODE_
                                                                if(ret_size > 0){
                                                                        std::cout << "tcp_session_socket_model::Run_cl cl_socket.write_some send data size[";
                                                                        std::cout << ret_size;
                                                                        std::cout << "]\n";
                                                                }
#endif
                                                        }else{
                                                        if( ec != boost::asio::error::try_again ){
                                                                break;
                                                        }
                                                                ec.clear();
                                                        }
                                                        boost::this_thread::yield();
                                                }
                                                delete pmsg;
                                                pmsg = NULL;
                                                if(ec){
                                                        // 送信失敗 
                                                        break;
                                                }
                                        }else{
                                                // 終了要求
                                                break;
                                        }
                                }
                                if(ec){
                                //送信失敗 -> 終了のお知らせ
                                        if(pdata == NULL){
                                                pdata = new socket_data();
                                        }
                                        pdata->data_size = 0;
                                        pdata->msg_type = 0;
                                        while(!cl2rs_msg.push(pdata)){}
                                        pdata = NULL;
                                        break;
                                }
                                if(pmsg != NULL){
                                        //　終了要求
                                        delete pmsg;
                                        pmsg = NULL;
                                        break;
                                }
                        }
#ifdef _DEBUG_MODE_
                        std::cout << "tcp_session_socket_model::Run_cl Loop end\n";
#endif
                }

                // クローズ
                cl_socket.close(ec);

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_socket_model::Run_cl cl_socket.close\n";
#endif

                // 受信バッファのお掃除
                if(pdata != NULL){
                        delete pdata;
                        pdata = NULL;
                }

                //RSスレッド待機
               while(true){ 
                        {
                                rd_scoped_lock scope_lock(threadB_state_mutex);
                                if(threadB_state == 0){
                                        break;
                                }
                        }
                        boost::this_thread::yield();
                }

               //キューのお掃除
               while(true){
                        pmsg = cl2rs_msg.pop();
                        if(pmsg == NULL){
                                break;
                        }
                        delete pmsg;
                        pmsg = NULL;
               }
               while(true){
                        pmsg = rs2cl_msg.pop();
                        if(pmsg == NULL){
                                break;
                        }
                        delete pmsg;
                        pmsg = NULL;
               }

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_socket_model::Run_cl end\n";
#endif
        }


        void tcp_session_socket_model::Run_rs(){
#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_socket_model::Run_rs start\n";
#endif

                socket_data* pdata = NULL;
                socket_data* pmsg = NULL;
                boost::system::error_code ec;

                {
                        rw_scoped_lock scope_lock(threadB_state_mutex);
                        threadB_state = 1;
                }
                //CLスレッド待機
                while(true){ 
                        {
                                rd_scoped_lock scope_lock(threadA_state_mutex);
                                if(threadA_state == 1){
                                        break;
                                }
                        }
                        boost::this_thread::yield();
                }
#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_socket_model::Run_rs Loop start\n";
#endif
                while(true){// RSスレッドメインループ
                        boost::this_thread::yield();

                        // RS 受信処理
                        if(rs_socket.is_open()){
                                if(pdata == NULL){
                                        pdata = new socket_data();
                                }
                                boost::this_thread::yield();
                                // 受信
                                size_t recv_size = rs_socket.read_some(boost::asio::buffer(pdata->buff.data(),65535), ec);
                                if(!ec){
                                        if(recv_size > 0){
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_socket_model::Run_rs rs_socket.read_some receive data size[";
                                                std::cout << recv_size;
                                                std::cout << "]\n";
#endif
                                                pdata->data_size = recv_size;
                                                pdata->msg_type = 1;
                                                while(!rs2cl_msg.push(pdata)){}
                                                pdata = NULL;
                                        }
                                }else{
                                        if(ec != boost::asio::error::try_again){
                                                //受信失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_socket_model::Run_rs rs_socket.read_some error[";
                                                std::cout << ec.message();
                                                std::cout << "]\n";
#endif
                                                pdata->data_size = 0;
                                                pdata->msg_type = 0;
                                                while(!rs2cl_msg.push(pdata)){}
                                                pdata = NULL;
                                                break;
                                        }else{
                                                ec.clear();
                                        }
                                }
                        }

                        //　RS要求チェック
                        while(true){
                                boost::this_thread::yield();
                                pmsg = cl2rs_msg.pop();
                                if(pmsg == NULL){
                                        break;
                                }
                                if(pmsg->msg_type == 1){// 送信要求
                                        size_t send_size = 0;
                                        while(send_size < pmsg->data_size){
                                                size_t ret_size = rs_socket.write_some(boost::asio::buffer(pmsg->buff.data() + send_size,pmsg->data_size - send_size), ec);
                                                if(!ec){
                                                // 送信成功
                                                        send_size += ret_size;
#ifdef _DEBUG_MODE_
                                                        if(ret_size > 0){
                                                                std::cout << "tcp_session_socket_model::Run_rs rs_socket.write_some send data size[";
                                                                std::cout << ret_size;
                                                                std::cout << "]\n";
                                                        }
#endif
                                                }else{
                                                        if( ec != boost::asio::error::try_again ){
                                                                //送信失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                                                                std::cout << "tcp_session_socket_model::Run_rs rs_socket.write_some error[";
                                                                std::cout << ec.message();
                                                                std::cout << "]\n";
#endif
                                                                if(pdata == NULL){
                                                                        pdata = new socket_data();
                                                                }
                                                                pdata->data_size = 0;
                                                                pdata->msg_type = 0;
                                                                while(!rs2cl_msg.push(pdata)){}
                                                                pdata = NULL;
                                                                break;
                                                        }
                                                        ec.clear();
                                                }
                                                boost::this_thread::yield();
                                        }
                                        // 送信要求処理完了
                                        delete pmsg;
                                        pmsg = NULL;
                                        break;
                                }
                                if(pmsg->msg_type == 2){// 接続要求
                                        // 接続
                                        rs_socket.connect(realserver_endpoint,ec);
                                        if(ec){
                                                //接続失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_socket_model::Run_rs rs_socket.connect error[";
                                                std::cout << ec.message();
                                                std::cout << "]\n";
#endif
                                                if(pdata == NULL){
                                                        pdata = new socket_data();
                                                }
                                                pdata->data_size = 0;
                                                pdata->msg_type = 0;
                                                while(!rs2cl_msg.push(pdata)){}
                                                pdata = NULL;
                                        }else{
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_socket_model::Run_rs rs_socket.connect OK\n";
#endif
                                        }
                                        // ノンブロッキングに設定
                                        boost::asio::socket_base::non_blocking_io cmd(true);
                                        rs_socket.io_control(cmd,ec);
                                        if(ec){
                                                //ノンブロッキング設定失敗 -> 終了のお知らせ
#ifdef _DEBUG_MODE_
                                                std::cout << "tcp_session_socket_model::Run_rs rs_socket.io_control error[";
                                                std::cout << ec.message();
                                                std::cout << "]\n";
#endif
                                                if(pdata == NULL){
                                                        pdata = new socket_data();
                                                }
                                                pdata->data_size = 0;
                                                pdata->msg_type = 0;
                                                while(!rs2cl_msg.push(pdata)){}
                                                pdata = NULL;
                                        }
                                        // 接続要求処理完了
                                        delete pmsg;
                                        pmsg = NULL;
                                        break;
                                }
                                // 終了要求
                                break;
                        }
                        if(pmsg != NULL){
                               //メッセージが残っているのは終了要求なので終了する
                                delete pmsg;
                                pmsg = NULL;
                                break;
                        }
                        if(ec){
                                // socket エラーが発生しているので終了
                                break;
                        }
                }

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_socket_model::Run_rs Loop end\n";
#endif

                // クローズ
                rs_socket.close(ec);

#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_socket_model::Run_rs rs_socket.close\n";
#endif

                // 受信バッファのお掃除
                if(pdata != NULL){
                        delete pdata;
                        pdata = NULL;
                }

                {
                        rw_scoped_lock scope_lock(threadB_state_mutex);
                        threadB_state = 0;
                }
#ifdef _DEBUG_MODE_
                std::cout << "tcp_session_socket_model::Run_rs end\n";
#endif
        }
}


