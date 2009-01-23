#ifndef __STORE_MIBDATA_H__
#define __STORE_MIBDATA_H__

#include <string>

class	l7ag_store_mibdata
{
	protected:

	private:

	public:
		l7ag_store_mibdata();
		~l7ag_store_mibdata();

		std::string	get_mibdata( std::string oid );

		void		set_mibdata( std::string oid, std::string value );
};

#endif //__STORE_MIBDATA_H__
