#Created by liucan, 2012/12/26

ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../mpp/Makefile.param
	include $(PARAM_FILE)
endif

sub_dir:= mipi hisi-irda rtc wtdg

clean_dir:= mipi hisi-irda rtc wtdg

.PHONY: all clean $(sub_dir) $(clean_dir)

all: mipi 
	@for dir in $(sub_dir); do \
		pushd  $$dir; make || exit 1; popd; \
	done
	@cp mipi/hi_mipi.h $(REL_INC)


$(sub_dir):
	@cd $@; make || exit1; 

clean: $(clean_dir)

$(clean_dir):
	@cd $(patsubst %_clean, %, $@) ; make clean;
