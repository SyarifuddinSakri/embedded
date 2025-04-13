PROJECT = prog
BUILD_DIR = bin

W5500_DIR = ../import/ioLibrary_Driver/Ethernet
W5500_CFILES = $(wildcard ../import/ioLibrary_Driver/Ethernet/*.c) $(wildcard ../import/ioLibrary_Driver/Ethernet/W5500/*.c)

HTTP_DIR = ../import/ioLibrary_Driver/Internet/httpServer
HTTP_CFILES = $(wildcard ../import/ioLibrary_Driver/Internet/httpServer/*.c)

LOG_DIR = ../import/log
LOG_CFILES = ../import/log/log.c

# FAT_DIR = ../import/fatfs
# FAT_CFILES = $(wildcard ../import/fatfs/*.c)

SOCK_DIR = ./sockets
SOCK_CFILES := $(wildcard ./sockets/*.c)

SHARED_DIR = ../import $(W5500_DIR) $(LOG_DIR) $(SOCK_DIR) $(HTTP_DIR) \
						 /usr/lib/gcc/arm-none-eabi/13.2.1/include \
						 /usr/include \
						 /usr/bin \
						 ./sockets
CFILES = main.c
CFILES += $(W5500_CFILES) $(LOG_CFILES) $(SOCK_CFILES) $(HTTP_CFILES)

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
