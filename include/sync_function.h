#ifndef	SYNC_FUNCTION_H
#define	SYNC_FUNCTION_H

#include <stdint.h>

struct	dcas_t{
	intptr_t	val;
	intptr_t	ver;
};



extern "C" bool	__sync_bool_double_compare_and_swap(
		volatile dcas_t*	addr,		// ターゲットのdcas_tが存在するアドレス
		intptr_t		new_val1,	// 入れる新しい値1
		intptr_t		new_val2,	// 入れる新しい値2
		intptr_t		old_val1,	// 比較する値
		intptr_t		old_val2 ){	// 比較する値

	char	result;
#ifndef	__X86_64__
	__asm__ __volatile__( "lock; cmpxchg16b %0; setz %1"
#else
	__asm__ __volatile__( "lock; cmpxchg8b %0; setz %1"
#endif
				: "=m"(*addr), "=q"(result)
				: "m"(*addr), "d"(old_val1), "a"(old_val2), "c"(old_val1), "b"(old_val2)
				: "memory"
	);
}

#endif	//SYNC_FUNCTION
