# Hisilicon Hi3518e sample Makefile

include ../Makefile.param
#ifeq ($(SAMPLE_PARAM_FILE), )
#     SAMPLE_PARAM_FILE:=../Makefile.param
#     include $(SAMPLE_PARAM_FILE)
#endif

# target source
SRCS  := $(wildcard *.c) 
#OBJ  := $(SRC:%.c=%.o)

TARGET := $(SRCS:%.c=%)
MPI_LIBS += $(REL_LIB)/libtde.a
# compile linux or HuaweiLite
include $(PWD)/../Make.$(OSTYPE)





