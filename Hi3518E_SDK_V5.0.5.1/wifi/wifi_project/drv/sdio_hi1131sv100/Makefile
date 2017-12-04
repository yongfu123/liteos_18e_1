include $(LITEOSTOPDIR)/config.mk

ARFLAGS = cr
LIBOUT = $(ROOTOUT)/obj/$(WIFI_DEVICE)

RM = -rm -rf

hisi_driver_subdir = data_backup_lib  driver  hisi_app

all: $(LIBOUT) hisi_driver

hisi_driver:
	for dir in $(hisi_driver_subdir); \
		do $(MAKE) -C $$dir all || exit 1; \
	done

$(LIBOUT):
	mkdir -p $(LIBOUT) 

clean:
	for dir in $(hisi_driver_subdir); \
		do $(MAKE) -C $$dir clean || exit 1;\
	done
	@$(RM) $(LIBOUT) *.bak *~

.PHONY: all clean
