ifeq ($(PARAM_FILE), )
        PARAM_FILE:=../../../../mpp/Makefile.param
        include $(PARAM_FILE)
		
endif

ifeq ($(KERNELRELEASE),)
export CIPHER_SRC_BASE=$(PWD)
endif

CONFIG_CIPHER_MULTICIPHER_SUPPORT=y
CONFIG_CIPHER_HASH_SUPPORT=y
CONFIG_CIPHER_RSA_SUPPORT=y
CFG_HI_CIPHER_RNG_SUPPORT=y


KBUILD_EXTRA_SYMBOLS +="$(OSAL_ROOT)/$(OSTYPE)/kernel/Module.symvers"

EXTRA_CFLAGS += -I$(CIPHER_SRC_BASE)
EXTRA_CFLAGS += -I$(CIPHER_SRC_BASE)/drv
EXTRA_CFLAGS += -I$(CIPHER_SRC_BASE)/drv/cipher
EXTRA_CFLAGS += -I$(CIPHER_SRC_BASE)/drv/hash
EXTRA_CFLAGS += -I$(CIPHER_SRC_BASE)/drv/efuse
EXTRA_CFLAGS += -I$(CIPHER_SRC_BASE)/include
EXTRA_CFLAGS += -I$(CIPHER_SRC_BASE)/../include
EXTRA_CFLAGS += -I$(CIPHER_SRC_BASE)/../arch/$(INTERDRVVER)
EXTRA_CFLAGS += -I$(OSAL_ROOT)/include

DRV_OBJ := drv/drv_cipher_intf.o drv/efuse/hal_efuse.o
DRV_OBJ += ../../../$(INTERDRVVER)/init/$(OSTYPE)/cipher_init.o
DRV_OBJ += drv/cipher/drv_cipher.o drv/cipher/hal_cipher.o
DRV_OBJ += drv/hash/drv_hash_intf.o

EXTRA_CFLAGS += -DCHIP_TYPE_$(INTERDRVVER)

REMOVED_FILES := "*.o" "*.ko" "*.order" "*.symvers" "*.mod" "*.mod.*" "*.cmd" ".tmp_versions" "modules.builtin"

ifeq ($(OSTYPE),HuaweiLite)
EXTRA_CFLAGS += $(CFLAGS)
endif

API_OBJS := api/unf_cipher.o api/mpi_cipher.o api/hi_rsa_bignum.o api/hi_rsa.o

OBJS = $(DRV_OBJ) $(API_OBJS)

TARGET := hi_cipher
obj-m := hi_cipher.o
hi_cipher-y += $(DRV_OBJ)

#*************************************************************************
# all source file in this module
SRCS   := $(patsubst %.o,%.c,$(OBJ))

.PHONY: all clean

all: $(OSTYPE)_build

clean : 
	@$(AT)find ./ -name "*.d" $(foreach file, $(REMOVED_FILES), -o -name $(file)) | xargs rm -rf
	@rm -f  libhi_cipher.a
	@for x in `find ../../../$(INTERDRVVER)/init/$(OSTYPE) -name "*cipher_init.o.cmd"`;do rm -rf $$x;done
	@for x in `find ../../../$(INTERDRVVER)/init/$(OSTYPE) -name "*cipher_init.o*"`;do rm -rf $$x;done
###################################
HuaweiLite_build: $(OBJS)
	@$(AR) $(ARFLAGS) lib$(TARGET).a $(OBJS)
	@mkdir -p $(REL_KO) && cp -rf libhi_cipher.a $(REL_KO)
	@cp $(CIPHER_SRC_BASE)/../arch/$(INTERDRVVER)/hi_unf_cipher.h $(REL_INC)

linux_build: $(API_OBJS)
	@echo -e "\e[0;32;1m... Configs as follow:\e[0;36;1m"
	@echo ---- CROSS=$(CROSS)
	@echo ---- HIARCH=$(HIARCH), HICHIP=$(HICHIP), CVER=$(CVER), DBG=$(HIDBG), HI_FPGA=$(HI_FPGA)
	@echo ---- CPU_TYPE=$(CPU_TYPE)
	@echo ---- MPP_CFLAGS=$(MPP_CFLAGS)
	@echo ---- SDK_PATH=$(SDK_PATH) , PARAM_FILE=$(PARAM_FILE)
	@echo ---- KERNEL_ROOT=$(KERNEL_ROOT)
	@echo ---- ARCH_ROOT=$(ARCH_ROOT), ARCH_HAL=$(ARCH_HAL)
	@@echo -e "\e[0m"
	@make -C $(KERNEL_ROOT) M=$(PWD) modules
	@mkdir -p $(REL_KO) && cp -rf $(TARGET).ko $(REL_KO)	
	@$(AR) -rv libhi_cipher.a $(API_OBJS)
	@cp $(CIPHER_SRC_BASE)/../arch/$(INTERDRVVER)/hi_unf_cipher.h $(REL_INC)
	
$(OBJS): %.o : %.c
	@echo $(CC) $< ...
	@$(CC) $(EXTRA_CFLAGS) -c $< -o $@
