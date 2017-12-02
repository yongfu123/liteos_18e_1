


ifeq ($(EXTDRV_PARAM_FILE), ) 
    EXTDRV_PARAM_FILE:=../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif


#ifeq ($(CONFIG_GPIO_I2C),y)
#	EXTRA_CFLAGS += -DHI_GPIO_I2C
#	EXTRA_CFLAGS+=-I$(PWD)/../gpio-i2c-ex
#else
#	EXTRA_CFLAGS += -DHI_I2C
#	EXTRA_CFLAGS+=-I$(PWD)/../hi_i2c
#endif

#SRCS = $(wildcard *.c)
SRCS := tlv320aic31.c



INC := -I$(REL_INC)

INC += -I$(OSAL_ROOT)/linux/kernel/himedia
INC += -I$(OSAL_ROOT)/include

INC += -I$(DRV_ROOT)/extdrv/$(EXTDRVVER)/tlv320aic31/arch/$(ARCH_DIR)/

ifeq ($(CONFIG_GPIO_I2C),y)
	INC += -DHI_GPIO_I2C
	INC+=-I$(PWD)/../gpio-i2c-ex
else
	INC += -DHI_I2C
	INC+=-I$(PWD)/../hi_i2c
endif



EXTDRV_CFLAGS += $(INC)



		
		
#*************************************************************************
TARGET := hi_tlv320aic31

#*************************************************************************
# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)	