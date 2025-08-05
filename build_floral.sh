BUILD_BOOT_IMG=1 \
KERNEL_BINARY=Image.lz4 \
VENDOR_RAMDISK_BINARY=ramdisk \
BOOT_IMAGE_HEADER_VERSION=2 \
BUILD_CONFIG=private/msm-google/build.config.floral_no-cfi \
KERNEL_CMDLINE="console=ttyMSM0,115200n8 androidboot.console=ttyMSM0 printk.devkmsg=on msm_rtb.filter=0x237 ehci-hcd.park=3 service_locator.enable=1 androidboot.memcg=1 cgroup.memory=nokmem usbcore.autosuspend=7 androidboot.usbcontroller=a600000.dwc3 swiotlb=2048 androidboot.boot_devices=soc/1d84000.ufshc loop.max_part=7 buildvariant=user" \
BASE_ADDRESS=0x00000000 \
PAGE_SIZE=4096 \
os_version=13.0.0 \
build/build.sh "$@"
