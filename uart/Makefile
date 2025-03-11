PROJECT = prog
BUILD_DIR = bin

SHARED_DIR = ../import \
	     /usr/include/ \
	     ../import/ioLibrary_Driver/Ethernet/
CFILES = main.c
CFILES += ./sysout.c \
	./blink.c \
	./spi_w5500.c \
	./st7735.c \
	../import/ioLibrary_Driver/Ethernet/w5500.c \
	../import/ioLibrary_Driver/Ethernet/socket.c \
	../import/ioLibrary_Driver/Ethernet/wizchip_conf.c

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
