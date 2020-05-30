PROJECT = template
BUILD_DIR = bin

CFILES = template.c

DEVICE=stm32f042k6t6
OOCD_FILE = stm32f0_usb.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

debug:		$(PROJECT).debug
%.debug:	%.elf
	($(OOCD) -f $(OOCD_FILE) &); \
	    $(PREFIX)gdb $(PROJECT).elf -x gdbinit.gdb;
.PHONY:	debug

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
