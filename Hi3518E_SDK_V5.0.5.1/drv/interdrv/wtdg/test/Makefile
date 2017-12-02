# Hisilicon Hi35xx sample Makefile

ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../mpp/Makefile.param
	include $(PARAM_FILE)
endif

PWD=$(realpath ./)

INC_FLAGS += -I../
INC_FLAGS += -I$(REL_INC)
INC_FLAGS += -I$(SDK_PATH)/$(EXTDRV)/sensor_spi

CFLAGS += -Wall -g $(INC_FLAGS) -D$(HIARCH) -DHICHIP=$(HICHIP) -D$(HIDBG) -D$(HI_FPGA) -lpthread -ldl

SRCS := $(wildcard *.c)
TARGET := wtdg_test

# *********************************
# compile linux or HuaweiLite
# *********************************
include $(PWD)/Make.$(OSTYPE)
