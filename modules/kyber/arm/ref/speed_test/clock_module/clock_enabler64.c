#include <linux/module.h>
#include <linux/kernel.h>

/* #define from /lib/modules/uname-r/source/arch/arm64/include/asm/perf_event.h in ASM Aarch 64 */
#define ARMV8_PMCR_MASK         0x3f
#define ARMV8_PMCR_E            (1 << 0) /*  Enable all counters */
#define ARMV8_PMCR_P            (1 << 1) /*  Reset all counters */
#define ARMV8_PMCR_C            (1 << 2) /*  Cycle counter reset */
#define ARMV8_PMCR_N_MASK       0x1f

#define ARMV8_PMUSERENR_EN_EL0  (1 << 0) /*  EL0 access enable */
#define ARMV8_PMUSERENR_CR      (1 << 2) /*  Cycle counter read enable */
#define ARMV8_PMUSERENR_ER      (1 << 3) /*  Event counter read enable */

#define ARMV8_PMCNTENSET_EL0_ENABLE (1<<31) /* *< Enable Perf count reg */

#define PERF_DEF_OPTS (1 | 16)

int clock_init(void)
{
    /*Enable user-mode access to counters. */
    __asm__ __volatile__ ("msr pmuserenr_el0, %0" : : "r"((unsigned long long)ARMV8_PMUSERENR_EN_EL0|ARMV8_PMUSERENR_ER|ARMV8_PMUSERENR_CR));

    /*   Performance Monitors Count Enable Set register bit 30:0 disable, 31 enable. Can also enable other event counters here. */
    __asm__ __volatile__ ("msr pmcntenset_el0, %0" : : "r" (ARMV8_PMCNTENSET_EL0_ENABLE));

    /* Enable counters */
    unsigned long long val=0;
    __asm__ __volatile__ ("mrs %0, pmcr_el0" : "=r" (val));
    __asm__ __volatile__ ("msr pmcr_el0, %0" : : "r" (val|ARMV8_PMCR_E));

    pr_alert("Clock enabler started\n");

    return 0;
}

void clock_exit(void)
{
    pr_alert("Clock enabler ended\n");
}

module_init(clock_init);
module_exit(clock_exit);

MODULE_LICENSE("GPL");

