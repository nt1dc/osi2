
#ifndef CHARACTER_DEV_H
#define CHARACTER_DEV_H

#include <linux/ioctl.h>

#define MAJOR_NUM 100
#define MAX_COUNT_VM_AREA_STRUCTES 50

#define IOCTL_GET_SIGNAL_INFO _IOR(MAJOR_NUM, 0, char *)
#define IOCTL_GET_VM_AREA_STRUCT _IOR(MAJOR_NUM, 1, char *)
#define IOCTL_GET_BUFFER_SIZE _IOR(MAJOR_NUM, 2, char *)

struct lab_signal_struct {
    int nr_threads;
    int group_exit_code;
    int notify_count;
    unsigned int flags;
    int leader;
};
struct lab_signal_struct_data {
    int pid;
    struct lab_signal_struct result;
};

struct vm_area_pos_info {
    unsigned long vm_start;
    unsigned long vm_end;
    unsigned long permissions;
    unsigned long rb_subtree_gap;
};
struct vm_area_struct_info {
    int pid;
    int actual_count;
    struct vm_area_pos_info vapi[MAX_COUNT_VM_AREA_STRUCTES];
};

struct buffer_size_struct_info {
    int pid;
    int size;
};
#define DEVICE_NAME "best_device_ever"

#endif