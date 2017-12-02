
ifeq ($(EXTDRV_PARAM_FILE), ) 
    EXTDRV_PARAM_FILE:=../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif



#SRCS = $(wildcard *.c)
SRCS = pwm.c
INC := -I$(REL_INC)
INC += -I$(OSAL_ROOT)/linux/kernel/himedia
INC += -I$(OSAL_ROOT)/include

INC += -I$(DRV_ROOT)/extdrv/$(EXTDRVVER)/pwm/arch/$(ARCH_DIR)/
EXTDRV_CFLAGS += $(INC)





		
		
#*************************************************************************
TARGET := hi_pwm

#*************************************************************************
# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)

	