#ifndef  __UTILITY_H__
#define  __UTILITY_H__

#define likely(c)   __builtin_expect(!!(c), 1)
#define unlikely(c) __builtin_expect(!!(c), 0)

#endif  //__ULITITY_H__
