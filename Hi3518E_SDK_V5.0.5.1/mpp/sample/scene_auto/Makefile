# Hisilicon Hi35xx sample Makefile

include ../Makefile.param

SRCS += $(wildcard *.c ./src/common/*.c) 
SRCS += $(wildcard ./src/adapt/$(HIARCH)/*.c) 
CFLAGS += -I./src/include -I$(REL_INC) 
TARGET  := sample_scene

# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)


