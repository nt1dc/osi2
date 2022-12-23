#include "mes.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define VM_READ 0x00000001
#define VM_WRITE 0x00000002
#define VM_EXEC 0x00000004

int main(int argc, char **argv) {

    if (argc < 2 || (strcmp(argv[1], "help") == 0)) {
        printf("Usage %s <PID>\n", argv[0]);
        printf("<PID> must be integer more than zero\n");
        return 0;
    }
    int pid = atoi(argv[1]);
    if (pid) {
        printf("Entered <PID> is %d\n", pid);
    } else {
        printf("Wrong <PID>\n");
        return 1;
    }

    int fd = open(DEVICE_NAME, 0);

    if (fd < 0) {
        printf("Can't open device file: %s\n", DEVICE_NAME);
        exit(2);
    };
    struct buff_size_info *buffSizeInfo = malloc(sizeof(struct buff_size_info));
    buffSizeInfo->pid = pid;
    int r = ioctl(fd, IOCTL_GET_BUFF_SIZE, buffSizeInfo);
    printf("%d\n", buffSizeInfo->size);

    struct vm_area_struct_info *vasi = malloc(sizeof(struct vm_area_struct_info));
    printf("has\n");

    struct vm_area_pos_info *s = vasi->vapi;
    *s = realloc(s, sizeof(struct vm_area_pos_info) * (buffSizeInfo->size));
    vasi->pid = pid;
    printf("was here \n");
    int ret_val = ioctl(fd, IOCTL_GET_VM_AREA_STRUCT, vasi);
    printf("<-- VM AREA STRUCT -->\n");
    if (ret_val != 0) {
        printf("IOCTL_GET_VM_AREA_STRUCT failed %d", ret_val);
        if (ret_val == 1) {
            printf("Process with <PID> = %d doesn't exist\n", vasi->pid);
        }
        if (ret_val == 2) {
            printf("Can't find vm_area_struct for Process with <PID> = %d\n", vasi->pid);
        }
        exit(ret_val);
    }

    for (int i = 0; i < vasi->actual_count; i++) {
        printf("0x%0.8hx-0x%0.8hx\t", vasi->vapi[i].vm_start, vasi->vapi[i].vm_end);
        printf("%c%c%c",
               (vasi->vapi[i].permissions & VM_READ) ? 'r' : '-',
               (vasi->vapi[i].permissions & VM_WRITE) ? 'w' : '-',
               (vasi->vapi[i].permissions & VM_EXEC) ? 'x' : '-');
        printf("\t%1d\n", vasi->vapi[i].rb_subtree_gap);
    }

    struct lab_signal_struct_data *lsigsd = malloc(sizeof(struct lab_signal_struct_data));

    lsigsd->pid = pid;

    ret_val = ioctl(fd, IOCTL_GET_SIGNAL_INFO, lsigsd);
    if (ret_val != 0) {
        printf("IOCTL_GET_SYSCALL_INFO failed %d: process with <PID> = %d doesn't exist\n", ret_val, lsigsd->pid);
        exit(ret_val);
    }

    printf("<-- SIGNAL STRUCT -->\n");
    printf("FOR SIGNAL_STRUCT WITH PID = %d\n", lsigsd->pid);
    printf("FLASG = %x\n", lsigsd->result.flags);
    printf("GROUP EXIT CODE = %d\n", lsigsd->result.group_exit_code);
    printf("LEADER = %d\n", lsigsd->result.leader);
    printf("NOTIFY COUNT = %d\n", lsigsd->result.notify_count);
    printf("NR THREAD = %d\n", lsigsd->result.nr_threads);

    return 0;
}