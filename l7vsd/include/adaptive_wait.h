/*!
 * @file  adaptive_wait.h
 * @brief spin and nanosleep wait class
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *      
 **********************************************************************/

#ifndef	ADAPTIVE_WAIT_H
#define	ADAPTIVE_WAIT_H
#include <time.h>
#include <stdint.h>
#include <boost/noncopyable.hpp>
#include <boost/detail/yield_k.hpp>

/*



usage:
  adaptive_wait	wait( 1000, 10 );
  
  for(;;){
      if( recv( hogehoge ) ){
	  // executing data process
	  ...
	  wait.reset(); //clear that default wait mode and parameter.
      }
      else{
	//boost::this_thread::yield();
	wait.wait();	// calcurate automaticary wait mode.
      }
  }

*/

namespace l7vs{

  
/*!
  @class	adaptive_wait
  Spin wait to nansleep wait, because that IOWAIT have sometime any waiting time.
*/
class	adaptive_wait : public boost::noncopyable {
protected:
    /*! @enum	WAIT MODE
	@brief	wait mode selection
    */
    enum WAIT_MODE{
      SPIN = 0,	//! spin mode( default )
      SLEEP	//! sleep mode( after spin mode )
    };
    //static const uint64_t
	//		MAX_SPIN_DIFF = 100000;	//! 100000clock max spin time
    static const unsigned int MAX_SPIN = 32;

    static const long	MAX_SLEEP = 1024;	//! 1024nsec max sleep time
    
    unsigned int spin_counter;
    //uint64_t		spin_diff;		//! spin difference value
    uint64_t		spin_increase;		//! spin increase value
    timespec		nanosleep_counter;	//! nanosleep timespec structure
    long		nanosleep_increase;	//! tvsec.nsec increase value
    WAIT_MODE		mode;			//! executable mode
    

    /*!
	rdtsc function.
	CPU clocking power on time that is load registar.
	@return uint64t cpu clock time.
    */
    uint64_t rdtsc(){
      uint64_t ret;
      #if defined(ARCH_X86)	//x86 mode
	  __asm__ __volatile__ (	"rdtsc" : "=a" (ret));
      #else			//x86_64 mode
	  __asm__ __volatile__ (	"rdtsc;" \
					"shlq $32, %%rdx;" \
					"orq %%rdx, %%rax;" \
					"movq %%rax, %0" : "=g"(ret)::"rax","rdx" );
     #endif
      return ret;
    }
    adaptive_wait(){}	//!don't open default constractor.

public:
    /*!
      constractor
	@param [in] uint64_t	spin_increase	spin wait clock increase.
	@param [in] long	nanosleep_increase nanosleep timespec.tv_nsec increase
    */
    adaptive_wait( uint64_t spin_increase_,
            long nanosleep_increase_ )
            : spin_counter( 0 ),
            spin_increase( spin_increase_ ),
            nanosleep_increase( nanosleep_increase_ ),
            mode( SPIN ) {
        nanosleep_counter.tv_sec = nanosleep_counter.tv_nsec = 0;
    }
    //! wait	spin wait or nanosleep wait function
    void		wait(){
	    if( mode == SPIN ){	//mode SPIN
            if( spin_counter < 4 )
            {
            }
            else if( spin_counter < 16 )
            {
                __asm__ __volatile__( "rep; nop" : : : "memory" );
            }
            else if( spin_counter < 32 || spin_counter & 1 )
            {
                sched_yield();
            }
            else
            {
                struct timespec rqtp = { 0, 0 };
                rqtp.tv_sec = 0;
                rqtp.tv_nsec = 1000;
                nanosleep( &rqtp, 0 );
            }
    
            if( MAX_SPIN > spin_counter ) spin_counter += spin_increase;
            else mode = SLEEP;
        }
        else {	//mode SLEEP
        nanosleep( &nanosleep_counter, NULL );
        if( MAX_SLEEP > nanosleep_counter.tv_sec )
            nanosleep_counter.tv_nsec += nanosleep_increase;
        }
    }
    
    //! reset	nanosleep mode -> spin mode change and spin time and nanoslep time tuning.
    void		reset(){
        mode = SPIN;
        //nanosleep_counter.tv_nsec -= nanosleep_increase;
        //if( nanosleep_counter.tv_nsec < nanosleep_increase )
        //    nanosleep_counter.tv_nsec = 0;
        //else
        //    nanosleep_counter.tv_nsec -= nanosleep_increase;
        nanosleep_counter.tv_nsec = 0;
        //spin_diff -= spin_diff;
        //if( spin_counter < spin_increase )
        //    spin_counter = 0;
        //else
        //    spin_counter -= spin_increase;
        spin_counter = 0;
    }
};

}	//namespace l7vs
#endif	// ADAPTIVE_WAIT_H
