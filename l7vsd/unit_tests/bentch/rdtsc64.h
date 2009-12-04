#ifndef    RDTSC64_H
#define    RDTSC64_H
//
//    this code is AMD64 only. don't use other platform.
//    n.nakai@sdy.co.jp
//
//

#define    RDTSC64(X) __asm__ __volatile__ (    \
        "rdtsc;"         \
        "movq %%rdx,%0;"     \
        "salq $32,%0;"         \
        "orq %%rax, %0"     \
        : "=r"(X)         \
        :             \
        : "%rax", "%rdx" );

#endif    //RDTSC64_H
