#ifndef	STUB_H
#define	STUB_H

#define	SESSION_POOL_NUM_PARAM	(512)

class	session_pool_debugger{
protected:
	bool flg;

	session_pool_debugger(){ flg = false; }
	session_pool_debugger( const session_pool_debugger & in ){}
	session_pool_debugger& operator=( const session_pool_debugger & in ){ return *this; }
public:
	static session_pool_debugger&	getInstance(){
		static	session_pool_debugger	instance;
		return	instance;
	}
	bool	get_exist_flag(){
		return flg;
	}
	void	set_exist_flag( bool in ){
		flg = in;
	}
};

#endif
