PROJECT = prog
BUILD_DIR = bin

RTOS_DIR = ../import/FreeRTOS-Kernel/include \
						 ../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3 \

RTOS_CFILES += $(wildcard ../import/FreeRTOS-Kernel/*.c) \
					$(wildcard ../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3/*.c) \
					../import/FreeRTOS-Kernel/portable/MemMang/heap_4.c

W5500_DIR = ../import/ioLibrary_Driver/Ethernet 

W5500_CFILES = $(wildcard ../import/ioLibrary_Driver/Ethernet/*.c)

SHARED_DIR = ../import $(RTOS_DIR) $(W5500_DIR) \
						 /usr/lib/gcc/arm-none-eabi/13.2.1/include \
						 /usr/include
CFILES = main.c log.c
CFILES += $(RTOS_CFILES) $(W5500_CFILES) ./spi_w5500.c ./web_server.c ./sha.c ./ws_server.c

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
