#LITEOSTOPDIR ?= ..

SAMPLE_OUT = .
TARGET = sample

include $(LITEOSTOPDIR)/config.mk
RM = -rm -rf

SAMPLE_INCLUDE := -I $(COMPLIE_ROOT)/drv/$(DEVICE_TYPE)/ \
			-I $(COMPLIE_ROOT)/drv/$(DEVICE_TYPE)/driver/include/common/hisi_wifi_api \
                        -I $(COMPLIE_ROOT)/drv/$(DEVICE_TYPE)/hisi_app/include/hilink \
                        -I $(COMPLIE_ROOT)/drv/$(DEVICE_TYPE)/hisi_app/include/hisilink \
                        -I $(COMPLIE_ROOT)/tools/wpa_supplicant-2.2/include \
			-I $(COMPLIE_ROOT)/tools/wpa_supplicant-2.2/include/src


LITEOS_CFLAGS += -DCONFIG_DRIVER_HI1131
LITEOS_CFLAGS += -D_PRE_WLAN_FEATURE_DATA_BACKUP
LITEOS_CFLAGS += -DCONFIG_NO_CONFIG_WRITE
LITEOS_CFLAGS += -D_PRE_WLAN_PM_FEATURE_MCU
LITEOS_CFLAGS +=-D_HI113X_SW_DEBUG=1
LITEOS_CFLAGS +=-D_HI113X_SW_RELEASE=2
ifeq ($(V_DEBUG), y)
LITEOS_CFLAGS +=-D_HI113X_SW_VERSION=_HI113X_SW_DEBUG
else
ifeq ($(V_RELEASE), y)
LITEOS_CFLAGS +=-D_HI113X_SW_VERSION=_HI113X_SW_RELEASE
else
LITEOS_CFLAGS +=-D_HI113X_SW_VERSION=_HI113X_SW_RELEASE
endif
endif

ifeq ($(HISI_WIFI_PLATFORM_HI3518EV200), y)
LITEOS_CFLAGS += -DHISI_WIFI_PLATFORM_HI3518EV200
endif

LITEOS_LIBS += -lwpa -lhi1131wifi -lhilink_adapt -lhilinksmartlink -lhisidata_backup -lhisilink -liperf
LITEOS_LDFLAGS += -L$(ROOTOUT)/lib
LITEOS_CFLAGS += $(SAMPLE_INCLUDE)
LITEOS_LIBDEP := --start-group $(LITEOS_LIBS) --end-group

SRCS = $(wildcard mcu_uart/*.c hi1131_wifi/hisi_app_demo/*.c)

SRCS += $(wildcard *_$(LITEOS_PLATFORM).c)
OBJS = $(patsubst %.c,$(SAMPLE_OUT)/%.o,$(SRCS))

all: $(TARGET)

clean:
	@$(RM) *.o  sample *.bin *.map *.asm
	@$(RM) $(SAMPLE_OUT)/hi1131_wifi/hisi_app_demo/*.o
	@$(RM) $(SAMPLE_OUT)/mcu_uart/*.o

$(TARGET): $(OBJS)
	$(LD) $(LITEOS_LDFLAGS) --gc-sections -Map=$(SAMPLE_OUT)/sample.map -o $(SAMPLE_OUT)/sample $(OBJS) $(LITEOS_LIBDEP)
	$(OBJCOPY) -O binary $(SAMPLE_OUT)/sample $(SAMPLE_OUT)/sample.bin
	$(OBJDUMP) -d $(SAMPLE_OUT)/sample >$(SAMPLE_OUT)/sample.asm

$(OBJS): $(SAMPLE_OUT)/%.o : %.c
	$(CC) $(LITEOS_CFLAGS) -c $< -o $@
.PHONY: all clean
