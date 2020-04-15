#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/i2c.h>
#include <linux/string.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#include "my_ds1307_ioctl.h"

struct ds1307 {
    struct i2c_client *client;
	dev_t dev_num;
	struct cdev *cdev_p;
	struct class *class_p;
	struct device *device_p;
	unsigned int open_count;
	char buf[20];
	int status;
};

struct ds1307 *clock;

static int ds1307_open(struct inode *inodep, struct file *filep)
{
	clock->open_count++;
	pr_info("Handle open event %d\n", clock->open_count);
	return 0;
}

static int ds1307_release(struct inode *inodep, struct file *filep)
{
	pr_info("Handle close event\n");
	return 0;
}

static ssize_t ds1307_write(struct file *filep, const char *user_buf, size_t len, loff_t *offset){
	int error_count = 0;
	error_count = copy_from_user(clock->buf, user_buf, len);
	if(error_count)
	{
		pr_info("Device file write failed\n");
		return -EFAULT;
	}
	clock->buf[len] = 0;
	return len;
}

static ssize_t ds1307_read(struct file *filep, char *user_buf, size_t len, loff_t *offset){
	int error_count = 0;
	// copy_to_user has the format ( * to, *from, size) and returns 0 on success
	error_count = copy_to_user(user_buf, clock->buf, strlen(clock->buf));
	if (error_count) {
		pr_info("Device file read failed\n");
		return -EFAULT;
	}
	return 0;
}

static long ds1307_ioctl(struct file *fd, unsigned int cmd, unsigned long arg) {
	switch (cmd)
	{
	case DS1307_SET_STATUS:
		// copy_from_user(&clock->status, (int *)arg, sizeof(clock->status));
		get_user(clock->status, (int *)arg);
		break;
	case DS1307_GET_STATUS:
		// copy_to_user((int *)arg, &clock->status, sizeof(clock->status));
		put_user(clock->status, (int *)arg);
		break;
	default:
		return -ENOTTY;
	}

	return 0;
}

static struct file_operations fops = {
	.open = ds1307_open,
	.release = ds1307_release,
	.write = ds1307_write,
	.read = ds1307_read,
	.unlocked_ioctl = ds1307_ioctl,
};

static int __init ds1307_init(void) {
	int ret = 0;

	// clock->dev_num = 0;

	clock = kzalloc(sizeof(*clock), GFP_KERNEL);
	if (clock == NULL)
		goto clock_allocate_failed;

	ret = alloc_chrdev_region(&clock->dev_num, 0, 1, "myds1307");
	if (ret < 0) {
		pr_info("Failed to register device number\n");
		goto alloc_dev_failed;
	}	

	clock->class_p = class_create(THIS_MODULE, "ds1307_class");
	if (IS_ERR(clock->class_p)) {
		pr_info("Error occurred, can not create device class\n");
		goto create_class_failed;
	}

	clock->device_p = device_create(clock->class_p, NULL, clock->dev_num, NULL, "ds1307_dev");
	if (IS_ERR(clock->device_p)) {
		pr_info("Can not create device\n");
		goto create_device_failed;
	}

	clock->cdev_p = cdev_alloc();
	if(clock->cdev_p == NULL) {
		pr_info("Failed to allocate cdev structure\n");
		goto alloc_cdev_failed;
	}

	cdev_init(clock->cdev_p, &fops);
	ret = cdev_add(clock->cdev_p, clock->dev_num, 1);
	if (ret < 0) {
		pr_err("error occur when add properties for struct cdev\n");
		goto cdev_add_failed;
	}

	// clock->open_count = 0;
	pr_info("Initialize driver successfully\n");
	return 0;

cdev_add_failed:
	cdev_del(clock->cdev_p);
alloc_cdev_failed:
	device_destroy(clock->class_p, clock->dev_num);
create_device_failed:
	class_destroy(clock->class_p);
create_class_failed:
	unregister_chrdev_region(clock->dev_num, 1);
alloc_dev_failed:
	kfree(clock);
clock_allocate_failed:
	return -1;
}

static void __exit ds1307_exit(void) {
	cdev_del(clock->cdev_p);
	device_destroy(clock->class_p, clock->dev_num);
	class_destroy(clock->class_p);
	unregister_chrdev_region(clock->dev_num, 1);
	kfree(clock);
	pr_info("Exit driver\n");
}

module_init(ds1307_init);
module_exit(ds1307_exit);

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Lucas Vu");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("DS1307 driver");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users
