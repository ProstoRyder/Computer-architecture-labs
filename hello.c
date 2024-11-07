#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Denys Denysiuk");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint print_count = 1;
module_param(print_count, uint, 0444);
MODULE_PARM_DESC(print_count, "Number of times to print 'Hello, world!'");

struct hello_entry {
    struct list_head list;
    ktime_t time;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
    struct hello_entry *entry;
    uint i;

    if (print_count == 0 || (print_count >= 5 && print_count <= 10)) {
	printk(KERN_WARNING "Warning: print_count is %u\n", print_count);
    } else if (print_count > 10) {
	printk(KERN_ERR "Error: print_count is too large\n");
	return -EINVAL;
    }

    for (i = 0; i < print_count; i++) {
	entry = kmalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	entry->time = ktime_get();
	list_add_tail(&entry->list, &hello_list);
	printk(KERN_EMERG "Hello, world!\n");
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_entry *entry, *tmp;

    list_for_each_entry_safe(entry, tmp, &hello_list, list) {
	printk(KERN_INFO "Time: %llu ns\n", ktime_to_ns(entry->time));
	list_del(&entry->list);
	kfree(entry);
    }
}

module_init(hello_init);
module_exit(hello_exit);
