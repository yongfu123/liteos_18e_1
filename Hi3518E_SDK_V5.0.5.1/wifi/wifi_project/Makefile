################################################################################
# Select Wi-Fi device
#
# Supported device:
#   1) usb_rtl8188eus
#   3) sdio_ap6212
#   4) sdio_ap6212a
#   5) sdio_ap6214a
#   6) sdio_ap6474
#   8) sdio_8801
#   9) sdio_hi1131sv100
################################################################################
#WIFI_DEVICE ?= usb_mt7601u
WIFI_DEVICE ?= sdio_hi1131sv100

################################################################################
# 编译 wifi 驱动时，需要自行指定 liteos 系统的路径
################################################################################
#LITEOSTOPDIR ?= /home/os/liushanping/bvt/liteos/liteos
#LITEOSTOPDIR ?= $(CURDIR)/../liteos

CURDIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
LITEOSTOPDIR ?= $(CURDIR)/../liteos

include ${LITEOSTOPDIR}/config.mk
ROOTOUT := $(CURDIR)/out/${LITEOS_PLATFORM}
COMPLIE_ROOT := $(CURDIR)
WIFI_DRIVER := $(WIFI_DEVICE)
ifeq ($(WIFI_DEVICE), sdio_ap6212)
WIFI_DRIVER = sdio_bcm
endif
ifeq ($(WIFI_DEVICE), sdio_ap6212a)
WIFI_DRIVER = sdio_bcm
endif
ifeq ($(WIFI_DEVICE), sdio_ap6214a)
WIFI_DRIVER = sdio_bcm
endif
ifeq ($(WIFI_DEVICE), sdio_ap6474)
WIFI_DRIVER = sdio_bcm
endif

export LITEOSTOPDIR
export ROOTOUT
export COMPLIE_ROOT

# 每次要发布时，即执行 make release 之前，需要人工配置下要支持的芯片
RELEASE_WIFI_DEVICE = usb_rtl8188eus usb_mt7601u sdio_ap6212 sdio_ap6212a \
sdio_ap6214a sdio_ap6474 sdio_rtl8189ftv sdio_8801 sdio_hi1131sv100

all: driver tools

$(ROOTOUT):
	mkdir -p $(ROOTOUT)
	mkdir -p $(ROOTOUT)/lib

driver: $(ROOTOUT)
	$(MAKE)	-C drv/$(WIFI_DRIVER) DEVICE_TYPE=$(WIFI_DEVICE)

tools: $(ROOTOUT)
	$(MAKE)	-C tools/wpa_supplicant-2.2 DEVICE_TYPE=$(WIFI_DEVICE)
	$(MAKE) -C tools/iperf-2.0.5 DEVICE_TYPE=$(WIFI_DEVICE)

sample:
	$(MAKE)	-C sample/$(WIFI_DRIVER) DEVICE_TYPE=$(WIFI_DEVICE)

clean:
	$(RM) -rf $(CURDIR)/out

driver_clean:
	$(RM) -rf $(ROOTOUT)/obj/$(WIFI_DRIVER) DEVICE_TYPE=$(WIFI_DEVICE)

tools_clean:
	$(RM) -rf $(ROOTOUT)/obj/wpa_supplicant-2.2 DEVICE_TYPE=$(WIFI_DEVICE)

sample_clean:
	$(MAKE)	-C sample/$(WIFI_DRIVER) DEVICE_TYPE=$(WIFI_DEVICE) clean

release: $(ROOTOUT)
	for dir in $(RELEASE_WIFI_DEVICE);\
	do \
	WIFI_DRIVER=$$dir;\
	if [ $$dir == sdio_ap6212 ];then \
		WIFI_DRIVER=sdio_bcm;\
	elif [ $$dir == sdio_ap6212a ];then \
		WIFI_DRIVER=sdio_bcm;\
	elif [ $$dir == sdio_ap6214a ];then \
		WIFI_DRIVER=sdio_bcm;\
	elif [ $$dir == sdio_ap6474 ];then \
		WIFI_DRIVER=sdio_bcm;\
	fi; \
	$(MAKE)	-C drv/$$WIFI_DRIVER DEVICE_TYPE=$$dir release;\
	if [ $$dir == usb_rtl8188eus ] || [ $$dir == sdio_rtl8189ftv ] || [ $$dir == usb_mt7601u ];then \
		$(MAKE) -C tools/wpa_supplicant-2.2 DEVICE_TYPE=$$dir release;\
	fi; \
	$(RM) -rf $(ROOTOUT)/lib/*;\
	$(RM) -rf $(ROOTOUT)/obj;\
	done
	$(RM) -rf $(CURDIR)/out
	@echo "=============== make release lib done ==============="

release_clean:
	$(RM) -rf $(CURDIR)/out
	$(RM) -rf $(CURDIR)/release
	@echo "=============== make release clean done ==============="


hi1131s_release:
	@echo "===start to make release==="
	rm -rf sdk_lib_release/new
	mkdir -p $(CURDIR)/sdk_lib_release/new
	cp -rf drv sample tools $(CURDIR)/sdk_lib_release/new
	cp Makefile $(CURDIR)/sdk_lib_release/new
#python delete name
	cp sdk_lib_release/hi1131s_tools/cp_wanted_ftree.py sdk_lib_release/hi1131s_tools/dst_src_dirs.conf $(CURDIR)
	python cp_wanted_ftree.py dst_src_dirs.conf
	rm -rf cp_wanted_ftree.py dst_src_dirs.conf
	cp drv/sdio_hi1131sv100/driver/hi3518ev200/libhi1131wifi.a sdk_lib_release/new_no_name/drv/sdio_hi1131sv100/driver/hi3518ev200/libhi1131wifi.a
#rename new_no_name
	rm -rf sdk_lib_release/new
	mv sdk_lib_release/new_no_name sdk_lib_release/new 

hi1131s_release_clean:
	rm -rf sdk_lib_release/new_no_name sdk_lib_release/new

.PHONY: all driver tools clean driver_clean tools_clean sample sample_clean release release_clean
