
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/version.h>
#include <linux/fs.h>

#include "mes.h"

#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/syscalls.h>
#include <linux/pci.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_VERSION("1.1.1");
MODULE_AUTHOR("nt1dc");
MODULE_DESCRIPTION("OS LAB2");
struct mutex etx_mutex;

static int lab_dev_open(struct inode *inode, struct file *file);

static int lab_dev_release(struct inode *inode, struct file *file);

static ssize_t lab_dev_read(struct file *filp, char __user

*buf,
size_t len, loff_t
*off);

static ssize_t lab_dev_write(struct file *filp, const char *buf, size_t len, loff_t *off);

static long lab_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*
** This function will be called when we open the Device file
*/
static int lab_dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO
    "Device File Opened...!!!\n");
    return 0;
}

/*
** This function will be called when we close the Device file
*/
static int lab_dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO
    "Device File Closed...!!!\n");
    return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t lab_dev_read(struct file *filp, char __user

*buf,
size_t len, loff_t
*off)
{
printk(KERN_INFO
"Read Function\n");
return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t lab_dev_write(struct file *filp, const char __user

*buf,
size_t len, loff_t
*off){
printk(KERN_INFO
"Write function\n");
return
len;
}
struct mutex etx_mutex;
mutex_init(&etx_mutex);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35))
static int device_ioctl(struct inode *inode,
                        struct file *file,
                        unsigned int ioctl_num,
                        unsigned long ioctl_param)
#else

static long lab_dev_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
#endif
{
    mutex_lock(&etx_mutex);
    printk(KERN_INFO"lab_dev_ioctl(%p,%lu,%lu)", file, ioctl_num, ioctl_param);
//    if (ioctl_num == get) {
////        todo: ну чтобы нормально  буфер выделить
//    }

    if (ioctl_num == IOCTL_GET_VM_AREA_STRUCT)
    {
        struct vm_area_struct_info *vasi = vmalloc(sizeof(struct vm_area_struct_info));
        copy_from_user(vasi, (struct vm_area_struct_info *) ioctl_param, sizeof(struct vm_area_struct_info));
        struct task_struct *task;

        task = get_pid_task(find_get_pid(vasi->pid), PIDTYPE_PID);
        if (task == NULL) {
            pr_err("Process with <PID> = %d doesn't exist\n", vasi->pid);
            mutex_unlock(&etx_mutex);
            return 1;
        }

        if (task->mm == NULL) {
            printk(KERN_INFO
            "Can't find vm_area_struct with this pid\n");
            mutex_unlock(&etx_mutex);
            return 2;
        }
        printk(KERN_INFO
        "vm area struct\n");
        struct vm_area_struct *pos = NULL;
        int i = 0;
        for (pos = task->mm->mmap, i = 0; pos != NULL && i < MAX_COUNT_VM_AREA_STRUCTES; pos = pos->vm_next, i++) {
            vasi->vapi[i].permissions = pos->vm_flags;
            vasi->vapi[i].vm_start = pos->vm_start;
            vasi->vapi[i].vm_end = pos->vm_end;
            vasi->vapi[i].rb_subtree_gap = pos->rb_subtree_gap;
        }
        vasi->actual_count = i - 1;
        copy_to_user((struct vm_area_struct_info *) ioctl_param, vasi, sizeof(struct vm_area_struct_info));
        vfree(vasi);
    }
    if (ioctl_num == IOCTL_GET_SIGNAL_INFO){
        struct lab_signal_struct_data *lsigsd = vmalloc(sizeof(struct lab_signal_struct_data));
        copy_from_user(lsigsd, (struct lab_signal_struct_data *) ioctl_param, sizeof(struct lab_signal_struct_data));
        struct task_struct *t = get_pid_task(find_get_pid(lsigsd->pid), PIDTYPE_PID);
        if (t == NULL) {
            printk(KERN_ERR
            "task_struct with pid=%d does not exist\n", lsigsd->pid);
            vfree(lsigsd);
            mutex_unlock(&etx_mutex);
            return 2;
        }
        printk(KERN_INFO
        "signals info\n");
        lsigsd->result.flags = t->signal->flags;
        lsigsd->result.group_exit_code = t->signal->group_exit_code;
        lsigsd->result.leader = t->signal->leader;
        lsigsd->result.notify_count = t->signal->notify_count;
        lsigsd->result.nr_threads = t->signal->nr_threads;

        copy_to_user((struct lab_signal_struct_data *) ioctl_param, lsigsd, sizeof(struct lab_signal_struct_data));
        vfree(lsigsd);
    }
    mutex_unlock(&etx_mutex);
    return 0;
}

struct file_operations file_ops =
        {
                .owner = THIS_MODULE,
                .read = lab_dev_read,
                .write = lab_dev_write,
                .open = lab_dev_open,
                .release = lab_dev_release,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35))
                .ioctl = device_ioctl
#else
                .unlocked_ioctl = lab_dev_ioctl
#endif
        };

int init_module() {
    int ret_val;
    ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &file_ops);
    if (ret_val < 0) {
        printk(KERN_ALERT
        "%s failed with %d\n", "Sorry, registering the character device \n", ret_val);
        return ret_val;
    }
    mutex_init(&etx_mutex);
    return 0;
}

void cleanup_module() {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

//proc cat devices
// dev sudo cat kmsg