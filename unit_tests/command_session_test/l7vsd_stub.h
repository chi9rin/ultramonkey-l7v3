#ifndef	L7VSD_H
#define	L7VSD_H
namespace	l7vs{
class	l7vsd{
public:
	struct l7vsd_operation_result{
		bool	flag;
		std::string	message;
		l7vsd_operation_result() : flag(true), message(""){}
		bool	operator==( const l7vsd_operation_result& in )
				{ return ( ( flag == in.flag ) && ( message == in.message ) ); }
		bool	operator!=( const l7vsd_operation_result& in )
				{ return ( ( flag != in.flag ) || ( message != in.message ) ); }
	};
};
}	//namespace	l7vs
#endif	//L7VSD_H
