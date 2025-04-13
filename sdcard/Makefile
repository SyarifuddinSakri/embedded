PROJECT = prog
BUILD_DIR = bin

LOG_DIR = ../import/log
LOG_CFILES = ../import/log/log.c

FAT_DIR = ./fatfs
FAT_CFILES = $(wildcard ./fatfs/*.c)

SOCK_DIR = ./spi
SOCK_CFILES := $(wildcard ./spi/*.c)

SHARED_DIR = ../import $(LOG_DIR) $(SOCK_DIR) $(FAT_DIR)\
						 /usr/lib/gcc/arm-none-eabi/13.2.1/include \
						 /usr/include \
						 /usr/bin \
						 ./sockets
CFILES = main.c
CFILES += $(LOG_CFILES) $(SOCK_CFILES) $(FAT_CFILES)

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
