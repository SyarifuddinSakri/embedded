PROJECT = prog
BUILD_DIR = bin

RTOS_DIR = ../import/FreeRTOS-Kernel/include \
						 ../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3

RTOS_CFILES += $(wildcard ../import/FreeRTOS-Kernel/*.c) \
					$(wildcard ../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3/*.c) \
					../import/FreeRTOS-Kernel/portable/MemMang/heap_4.c

W5500_DIR = ../import/ioLibrary_Driver/Ethernet
W5500_CFILES = $(wildcard ../import/ioLibrary_Driver/Ethernet/*.c)

DIG_DIR = ../import/digest
DIG_CFILES = $(wildcard ../import/digest/*.c)

LOG_DIR = ../import/log
LOG_CFILES = ../import/log/log.c

SOCK_DIR = ./sockets
SOCK_CFILES := $(wildcard ./sockets/*.c)

SHARED_DIR = ../import $(RTOS_DIR) $(W5500_DIR) $(LOG_DIR) $(SOCK_DIR) $(DIG_DIR)\
						 /usr/lib/gcc/arm-none-eabi/13.2.1/include \
						 /usr/include \
						 /usr/bin \
						 ./sockets
CFILES = main.c
CFILES += $(RTOS_CFILES) $(W5500_CFILES) $(LOG_CFILES) $(SOCK_CFILES) $(DIG_CFILES)

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
