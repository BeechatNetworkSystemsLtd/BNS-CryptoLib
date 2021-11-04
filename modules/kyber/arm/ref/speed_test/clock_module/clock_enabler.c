#include <linux/module.h>
#include <linux/kernel.h>

int clock_init(void)
{
    asm ("mcr p15, 0, %0, C9, C14, 0\n\t" :: "r"(1));
    asm ("mcr p15, 0, %0, C9, C14, 2\n\t" :: "r"(0x8000000f));

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

