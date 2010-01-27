#ifndef    SYNC_FUNCTION_H
#define    SYNC_FUNCTION_H

#include <stdint.h>


// structure of packing two max value of dcas
// when x86, intptr_t size is 32bit=4byte, so structure size is 8byte.
// when x86_64, intptr_t size is 64bit=8byte, so structure size is 16byte.

struct    dcas_t{
    intptr_t    val;
    intptr_t    ver;
};

extern "C" bool    __sync_bool_double_compare_and_swap(
        volatile dcas_t*    addr,    // target dcas_t address
        intptr_t        new_val1,    // new value 1 for input
        intptr_t        new_val2,    // new value 2 for input
        intptr_t        old_val1,    // old value 1 for compare
        intptr_t        old_val2 ){  // old value 2 for compare

    char    result;
#ifndef    __X86_64__
    __asm__ __volatile__( "lock; cmpxchg16b %0; setz %1"
#else
    __asm__ __volatile__( "lock; cmpxchg8b %0; setz %1"
#endif
                : "=m"(*addr), "=q"(result)
                : "m"(*addr), "d"(old_val1), "a"(old_val2), "c"(new_val1), "b"(new_val2)
                : "memory"
    );
    return static_cast<bool><( result );
}

#endif    //SYNC_FUNCTION
