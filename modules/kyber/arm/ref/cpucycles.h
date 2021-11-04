#ifndef CPUCYCLES_H
#define CPUCYCLES_H

#include <stdint.h>

#define ARMV8_PMEVTYPER_EVTCOUNT_MASK 0x3ff

#ifdef VERSION32
/*
  // enable user-mode access to the performance counter
  asm ("MCR p15, 0, %0, C9, C14, 0\n\t" :: "r"(1));

  // disable counter overflow interrupts (just in case)
  asm ("MCR p15, 0, %0, C9, C14, 2\n\t" :: "r"(0x8000000f));
*/
static inline uint64_t cpucycles(void)
{
    unsigned cc;
    static int init = 0;

    if (!init)
    {
        __asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 2" :: "r"(1<<31)); /* stop the cc */
        __asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 0" :: "r"(5));     /* initialize */
        __asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 1" :: "r"(1<<31)); /* start the cc */
        init = 1;
    }
    __asm__ __volatile__ ("mrc p15, 0, %0, c9, c13, 0" : "=r"(cc));

    return cc;
}

#else

static inline uint64_t cpucycles(void)
{
    static int init = 0;
    uint64_t r = 0;
    uint32_t events = 0;

    if (!init)
    {
        events &= ARMV8_PMEVTYPER_EVTCOUNT_MASK;
        __asm__ __volatile__("isb");
        /* Just use counter 0 */
        __asm__ __volatile__("msr pmevtyper0_el0, %0" : : "r"(events));
        /*   Performance Monitors Count Enable Set register bit 30:1 disable, 31,1
         * enable */

        __asm__ __volatile__("mrs %0, pmcntenset_el0" : "=r"(r));
        __asm__ __volatile__("msr pmcntenset_el0, %0" : : "r"(r | 1));
        init = 1;
    }
    __asm__ __volatile__("mrs %0, pmccntr_el0" : "=r"(r));

    return r;
}

#endif

uint64_t cpucycles_overhead(void);

#endif
