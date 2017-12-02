

ifeq ($(EXTDRV_PARAM_FILE), ) 
    EXTDRV_PARAM_FILE:=../Makefile.param
    include $(EXTDRV_PARAM_FILE)
endif



#SRCS = $(wildcard *.c)
SRCS := adv7179.c
#INC := -I$(REL_INC)
#INC += -I./arch/$(ARCH_DIR)/
INC := -I$(DRV_ROOT)/extdrv/$(EXTDRVVER)/adv7179/arch/$(ARCH_DIR)/

EXTDRV_CFLAGS += $(INC)


		
		
#*************************************************************************
TARGET := hi_adv7179

#*************************************************************************
# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)

	
	