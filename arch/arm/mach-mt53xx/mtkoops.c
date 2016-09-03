#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/console.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kmsg_dump.h>
#include <linux/slab.h>

static unsigned long record_size = 4096;
module_param(record_size, ulong, 0400);
MODULE_PARM_DESC(record_size,
		"record size for MTD OOPS pages in bytes (default 4096)");

static char emmcdev[80];
module_param_string(emmcdev, emmcdev, 80, 0400);
MODULE_PARM_DESC(emmcdev,
		"name or index number of the eMMC device to use");

static int dump_oops = 1;
module_param(dump_oops, int, 0600);
MODULE_PARM_DESC(dump_oops,
		"set to 1 to dump oopses, 0 to only dump panics (default 1)");

extern int emmcoops_write(void *ptr, sector_t lba, sector_t blknr);

#define DUMP_BUF_SZ (4096*8)
#define EMMC_BLOCK_SIZE	(512)
#define BYTE_TO_BLOCK(v) (v >> 9)
static void* dump_buf;
/* How much of the console log to snapshot */
static unsigned long kmsg_bytes = DUMP_BUF_SZ;

static void emmcoops_do_dump(struct kmsg_dumper *dumper,
			    enum kmsg_dump_reason reason)
{
	size_t len;
	unsigned long total = 0;
	unsigned long i;
	unsigned char* tmp;
	static unsigned int once = 0;

	if (once)
	    return;
	once = 1;

	pr_debug("(yjdbg) emmcoops_do_dump\n");
	/* Only dump oopses if dump_oops is set */
	if (!dump_oops)
		return;

	pr_debug("(yjdbg) emmcoops_do_dump:0\n");
	while (total < kmsg_bytes) {
		tmp = (unsigned char*)dump_buf + total;
		if (!kmsg_dump_get_buffer(dumper, true, tmp, DUMP_BUF_SZ, &len))
			break;
		for (i=0; i<len; i++) {
			pr_debug("%c", tmp[i]);
		}
		total += len;
	}
	printk("(yjdbg) emmcoops_do_dump:1, cnt: %lu\n", total);

	tmp = (unsigned char*)dump_buf;
	for (i=0; i<total; i++) {
		pr_debug("%c", tmp[i]);
	}

	// dump to eMMC
	total = (total + EMMC_BLOCK_SIZE - 1) & ~(EMMC_BLOCK_SIZE - 1);
	emmcoops_write(dump_buf, (sector_t)0, (sector_t)BYTE_TO_BLOCK(total));
}

static struct kmsg_dumper emmc_dumper = {
	.dump = emmcoops_do_dump,
};

static int __init emmcoops_init(void)
{
	int err;

	dump_buf = kmalloc(DUMP_BUF_SZ, GFP_KERNEL);
	if (!dump_buf) {
		printk("emmcoops_init alloc buffer fail\n");
		return 1;
	}
	printk("emmcoops_init (0x%p)\n", &emmc_dumper);
	err = kmsg_dump_register(&emmc_dumper);
	return err;
}

static void __exit emmcoops_exit(void)
{
	if (kmsg_dump_unregister(&emmc_dumper) < 0)
		printk(KERN_WARNING "emmcoops: could not unregister kmsg_dumper\n");
	kfree(dump_buf);
}


module_init(emmcoops_init);
module_exit(emmcoops_exit);

