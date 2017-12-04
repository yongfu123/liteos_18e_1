include $(LITEOSTOPDIR)/config.mk

ARFLAGS = cr

all:
	mkdir -p $(ROOTOUT)/lib/
	cp -rf $(LITEOS_PLATFORM)/*.a $(ROOTOUT)/lib


clean:
	rm -rf $(OUT)/lib/*.a

.PHONY: all clean
