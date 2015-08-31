#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/kfifo.h>


#define DEVICE_NAME "suicide"
#define CLASS_NAME "helper"
#define BUDDIES_COUNT 64


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Senkovich <dmitrysenkovich@gmail.com>");
MODULE_DESCRIPTION("Pretty task suicide helper");
MODULE_VERSION("0.1");


static int major_number = -1;
static struct class* helper_class = NULL;
static struct device* suicide_device = NULL;
static dev_t dev_number = -1;
static pid_t current_pid = -1;
static DECLARE_KFIFO(suicide_queue, pid_t, BUDDIES_COUNT);
static DEFINE_MUTEX(lock_suicide_queue);


static int dev_open(struct inode*, struct file*);
static void get_buddy(void);
static void suicide(void);


static struct file_operations fops =
{
    .open = dev_open
};


static int __init suicide_init(void)
{
    printk(KERN_INFO "Suicide helper: Trying to initialize your suicide helper\n");

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        printk(KERN_ALERT "Suicide helper: Hm.. Looks like we can't start help "
            "poor living bastards because of error while getting major number:c\n");
        return major_number;
    }

    printk(KERN_INFO "Suicide helper: Got major number\n");

    helper_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(helper_class))
    {
       unregister_chrdev(major_number, DEVICE_NAME);
       printk(KERN_ALERT "Suicide helper: Failed to register device class:c\n");
       return PTR_ERR(helper_class);
    }

    printk(KERN_INFO "Suicide helper: Created helper class\n");

    dev_number = MKDEV(major_number, 0);
    suicide_device = device_create(helper_class, NULL, dev_number, NULL, DEVICE_NAME);
    if (IS_ERR(suicide_device))
    {
       class_destroy(helper_class);
       unregister_chrdev(major_number, DEVICE_NAME);
       printk(KERN_ALERT "Suicide helper: Failed to create the device:c\n");
       return PTR_ERR(suicide_device);
    }

    printk(KERN_INFO "Suicide helper: Created suicide device\n");

    printk(KERN_INFO "Suicide helper: Welcome! Suicide helper is waiting for you!!!=)\n");
    return 0;
}


static void __exit suicide_exit(void)
{
    device_destroy(helper_class, dev_number);
    class_destroy(helper_class);
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "Suicide helper: See you next time!\n");
}


static int dev_open(struct inode* inodep, struct file* filep)
{
    get_buddy();
    suicide();
    return 0;
}


/*
 * Getting new buddy pid. But what if our buddy was rescheduled?
 * We don't want to kill another person:\ So we're using kernel
 * preemption.
 */
static void get_buddy(void)
{
    preempt_disable();
    kfifo_put(&suicide_queue, current->pid);
    preempt_enable();
}


/*
 * Actual suicide function. Here we enable preemption as
 * we've already got pid but still don't want race among
 * suicide clients. Everyone will have his time! Also we
 * we want to kill out buddy for sure without a chance to
 * handle death signal. So SIGKILL is used instead of
 * SIGTERM. No hesistancy!
 */
static void suicide(void)
{
    mutex_lock(&lock_suicide_queue);
    kfifo_get(&suicide_queue, &current_pid);
    printk(KERN_INFO "Suicide helper: Die, die my darling %ld!\n", (long)current_pid);
    send_sig(SIGKILL, current, 0);
    printk(KERN_INFO "Suicide helper: Our dear client %ld is killed:)\n", (long)current_pid);
    mutex_unlock(&lock_suicide_queue);
}


module_init(suicide_init);
module_exit(suicide_exit);
