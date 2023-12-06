#ifndef __CLOCKS_PER_US_H__
#define __CLOCKS_PER_US_H__

#define CLOCKS_PER_US ((unsigned long long) 2394.412271)
#endif

#define CLOCK_READ() ({ \
                        unsigned int lo; \
                        unsigned int hi; \
                        __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi)); \
                        (unsigned long long)(((unsigned long long)hi) << 32 | lo); \
                        })
