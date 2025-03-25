PROJECT = prog
BUILD_DIR = bin

RTOS_DIR = ../import/FreeRTOS-Kernel/include \
						 ../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3

RTOS_CFILES += $(wildcard ../import/FreeRTOS-Kernel/*.c) \
					$(wildcard ../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3/*.c) \
					../import/FreeRTOS-Kernel/portable/MemMang/heap_4.c

W5500_DIR = ../import/ioLibrary_Driver/Ethernet 
W5500_CFILES = $(wildcard ../import/ioLibrary_Driver/Ethernet/*.c)

LOG_DIR = ../import/log
LOG_CFILES = ../import/log/log.c

DIG_DIR = ../import/digest
DIG_CFILES = $(wildcard ../import/digest/*.c)

SHARED_DIR = ../import $(RTOS_DIR) $(W5500_DIR) $(LOG_DIR) $(DIG_DIR)\
						 /usr/lib/gcc/arm-none-eabi/13.2.1/include \
						 /usr/include \
						 /usr/include/mbedtls \
						 /usr/bin
CFILES = main.c
CFILES += $(RTOS_CFILES) $(W5500_CFILES) $(LOG_CFILES) $(DIG_CFILES) ./spi_w5500.c ./open_socket.c ./ws.c

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
