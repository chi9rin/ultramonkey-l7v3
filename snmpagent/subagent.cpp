#include "subagent.h"

/*!
 * l7ag_subagent class constructor
 */
l7ag_subagent::l7ag_subagent()
{
	stop_flag	= false;
	stat		= STOP;
}

/*!
 * main loop
 */
void
l7ag_subagent::do_subagent()
{
	//initialize

	//loop
	while(1){
		if( stop_flag ){
			break;
		}
	}

	//finalize
}

/*!
 * stop main loop
 */
void
l7ag_subagent::stop_subagent()
{
	stop_flag = true;
}

