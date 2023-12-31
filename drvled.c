#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/types.h>

#define DRIVER_NAME   "drvled"

#define LED_OFF 0
#define LED_ON  1

static struct{
	dev_t devnum;
	struct cdev cdev;
	uint8_t led_status;
}drvled_data;

static void drvled_setled(uint8_t status)
{
	drvled_data.led_status = status;
}

static ssize_t drvled_read(struct file *file, char __user *buf,
						size_t count, loff_t *ppos)
{
	static const char * const msg[] = {"OFF\n", "ON\n"};
	int size;
	
	if(*ppos > 0)
		return 0;
	
	size = strlen(msg[drvled_data.led_status]);
	if(size > count)
		size = count;
	
	if(copy_to_user(buf, msg[drvled_data.led_status],count))
		return -EFAULT;
	
	*ppos += size;
	
	return size;
}

static ssize_t drvled_write(struct file *file, const char __user *buf,
						size_t count, loff_t *ppos)
{
	char kbuf = 0;
	
	if(copy_from_user(&kbuf, buf,1))
		return -EFAULT;
	
	if(kbuf == '1')
	{
		drvled_setled(1);
		printk("LED is ON\n");
	}
	else
	{
		drvled_setled(0);
		printk("LED is OFF\n");
	}
	return count;
}

static const struct file_operations drvled_fops = {
	.owner = THIS_MODULE,
	.read  = drvled_read,
	.write = drvled_write,
};

static int __init drvled_init(void)
{
	int result;
	
	result = alloc_chrdev_region(&drvled_data.devnum, 0, 1, DRIVER_NAME);
	if(result > 0)
	{
		pr_err("%s driver initialization is failed\n",DRIVER_NAME);
		return result;
	}
	
	cdev_init(&drvled_data.cdev, &drvled_fops);
	
	result = cdev_add(&drvled_data.cdev, drvled_data.devnum, 1);
	
	if(result > 0)
	{
		pr_err("%s char device registration is failed\n",DRIVER_NAME);
		unregister_chrdev_region(drvled_data.devnum,1);
		return result;
	}
	
	drvled_setled(LED_OFF);
	
	pr_info("%s: initialized.\n", DRIVER_NAME);
	return 0;		
}

static void __exit drvled_exit(void)
{
	cdev_del(&drvled_data.cdev);
	unregister_chrdev_region(drvled_data.devnum,1);
	pr_info("%s: Unregistered\n", DRIVER_NAME);
}

module_init(drvled_init);
module_exit(drvled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vinod Jadhav <vinodjadhav@mirafra.com>");
MODULE_DESCRIPTION("This is Dummy LED Driver");
