
ifeq ($(EXTDRV_PARAM_FILE), ) 
    EXTDRV_PARAM_FILE:=../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif



#SRCS = $(wildcard *.c)
SRCS := sensor_i2c.c

INC := -I$(REL_INC)
#INC += -I./arch/$(ARCH_DIR)/
INC += -I$(DRV_ROOT)/extdrv/$(EXTDRVVER)/sensor_i2c/arch/$(ARCH_DIR)/
INC += -I$(OSAL_ROOT)/include

EXTDRV_CFLAGS += $(INC)



		
#*************************************************************************
TARGET := hi_sensor_i2c

#*************************************************************************
# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)

