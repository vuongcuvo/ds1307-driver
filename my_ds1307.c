#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/i2c.h>
#include <linux/string.h>

struct ds1307 {
    struct i2c_client *client;

};

static int __init ds1307_init(void) {
	printk("Initialize driver successfully\n");
	return 0;
}

static void __exit ds1307_exit(void) {
	printk("Exit driver\n");
}

module_init(ds1307_init);
module_exit(ds1307_exit);

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Lucas Vu");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("DS1307 driver");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users
