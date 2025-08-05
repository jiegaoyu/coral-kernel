KERNEL_DIR := /home/yujiegao/android-kernel/out/android-msm-pixel-4.14/private/msm-google
PWD := $(shell pwd)
ccflags-y += -I$(src)/include
ccflags-y += -I$(src)
obj-$(CONFIG_TIMED_TASK) += timed_task.o

yu:
	make  ARCH=arm64 CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=aarch64-linux-android- CROSS_COMPILE_ARM32=arm-linux-androideabi- CC=clang HOSTCC=clang LD=ld.lld O=$(PWD)/out -C $(KERNEL_DIR) M=$(PWD) modules


