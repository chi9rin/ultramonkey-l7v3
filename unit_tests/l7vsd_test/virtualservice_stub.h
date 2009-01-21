#ifndef	VIRTUALSERVICE_H
#define	VIRTUALSERVICE_H

#include "virtualservice_element.h"

namespace	l7vs{
class	virtualservice{
public:
	virtualservice_element		element;
	virtualservice_element&		get_element(){ return element; }

};
}	//namespace	l7vs
#endif	//VIRTUALSERVICE_H
