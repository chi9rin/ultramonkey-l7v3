#ifndef	SCHEDULE_MODULE_CONTROL
#define	SCHEDULE_MODULE_CONTROL
namespace	l7vs{
class	schedule_module_control{
protected:
	schedule_module_control(){}

public:
// variables
	static	bool	initialize_called;
	static	bool	finalize_called;

// functions
	// instance getter function.
	static schedule_module_control&	getInstance()
	{
		static	schedule_module_control	instance;
		return	instance;
	}

	//! initialize
	void	initialize( const std::string& infile_path )
	{ initialize_called = true; }

	//! finalize
	void	finalize()
	{ finalize_called = true; }

};	// class

}	//namespace	l7vs
#endif	//SCHEDULE_MODULE_CONTROL
