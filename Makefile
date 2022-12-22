MODULE_NAME = best_device_ever
KERNEL_DIR ?= "/lib/modules/$(shell uname -r)/build"

KERNEL_SRCS = my_module.c
USER_SRCS = user.c
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o)

obj-m += $(MODULE_NAME).o

$(MODULE_NAME)-objs := $(KERNEL_OBJS)

CFLAGS=-std=gnu99 -Wno-declaration-after-statement
ccflags-y=-std=gnu99 -Wno-declaration-after-statement
USER_APP=user_app
DEVICE=best_device_ever
LAB_MAJOR_NUMBER_DEVICE=100

all: module create_dev install
user: $(USER_SRCS)
	gcc $(USER_SRCS) -o $(USER_APP) -std=c99
module: $(KERNEL_SRCS)
	make -C $(KERNEL_DIR) M=$(PWD) modules
install: $(MODULE_NAME).ko
	sudo insmod ./$(MODULE_NAME).ko
create_dev:
	sudo mknod $(DEVICE) c $(LAB_MAJOR_NUMBER_DEVICE) 0
remove:
	sudo rmmod $(MODULE_NAME)
clean: 
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f $(USER_APP)
	rm -f $(DEVICE)