#ifndef	STUB_H
#define	STUB_H

#define	SESSION_POOL_NUM_PARAM	(512)

class	session_pool_debugger{
protected:
	bool session_pool_param_flg;
	bool pmcontrol_error_flag;
	bool smcontrol_error_flag;
	bool pm_function_called;

	session_pool_debugger() : session_pool_param_flg( false ) {}
	session_pool_debugger( const session_pool_debugger & in ){}
	session_pool_debugger& operator=( const session_pool_debugger & in ){ return *this; }
public:
	static session_pool_debugger&	getInstance(){
		static	session_pool_debugger	instance;
		return	instance;
	}
	bool&	param_exist_flag(){
		return session_pool_param_flg;
	}
	bool&	pmcontrol_err_flag(){
		return pmcontrol_error_flag;
	}
	bool&	smcontrol_err_flag(){
		return smcontrol_error_flag;
	}
	bool&	pm_function_called_flag(){
		return pm_function_called;
	}
};

#endif
