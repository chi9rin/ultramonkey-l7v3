#ifndef	MODULE_BASE_H
#define	MODULE_BASE_H


class module_base
{
public:
	virtual	bool	is_tcp() = 0;
	virtual	bool	is_udp() = 0;
}

#endif //MODULE_BASE_H
