PATH := /home/yujiegao/android-kernel/prebuilts-master/misc/linux-x86/libufdt:/home/yujiegao/android-kernel/prebuilts-master/misc/linux-x86/dtc:/home/yujiegao/android-kernel/prebuilts-master/misc/linux-x86/lz4:/home/yujiegao/android-kernel/prebuilts-master/clang/host/linux-x86/clang-r416183b/bin/:/home/yujiegao/android-kernel/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.9/bin:/home/yujiegao/android-kernel/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin:/home/yujiegao/WORKING_DIRECTORY/out/host/linux-x86/bin:/home/yujiegao/android-kernel/build/build-tools/path/linux-x86:/home/yujiegao/bin:/home/yujiegao/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin:$(PATH)
KERNEL_DIR := /home/yujiegao/android-kernel/out/android-msm-pixel-4.14/private/msm-google
PWD := $(shell pwd)
ccflags-y += -I$(src)/include
ccflags-y += -I$(src)
obj-$(CONFIG_TIMED_TASK) += timed_task2.o

yu:
	make  ARCH=arm64 CLANG_TRIPLE=aarch64-linux-gnu- CROSS_COMPILE=aarch64-linux-android- CROSS_COMPILE_ARM32=arm-linux-androideabi- CC=clang HOSTCC=clang LD=ld.lld O=$(PWD)/out -C $(KERNEL_DIR) M=$(PWD) modules


