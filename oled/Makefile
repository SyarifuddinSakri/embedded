PROJECT = prog
BUILD_DIR = bin

SHARED_DIR = ../import \
	     /usr/include/
CFILES = main.c
CFILES += ./sysout.c ./st77xx.c ./blink.c ./font1.c ./donut.c

# TODO - you will need to edit these two lines!
DEVICE=stm32f103c8
OOCD_FILE = board/stm32f1discovery.cfg

# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
