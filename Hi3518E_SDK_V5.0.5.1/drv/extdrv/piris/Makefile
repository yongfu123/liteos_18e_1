


ifeq ($(EXTDRV_PARAM_FILE), ) 
    EXTDRV_PARAM_FILE:=../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif


SRCS = piris.c
#INC := -I$(REL_INC)
#INC += -I./arch/$(ARCH_DIR)/
#CFLAGS += -I$(OSAL_ROOT)/include

INC := -I$(REL_INC)
INC += -I$(DRV_ROOT)/extdrv/$(EXTDRVVER)/piris/arch/$(ARCH_DIR)/
INC += -I$(OSAL_ROOT)/include

EXTDRV_CFLAGS += $(INC)


		
		
#*************************************************************************
TARGET := hi_piris

#*************************************************************************
# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)	