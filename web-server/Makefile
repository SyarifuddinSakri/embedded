PROJECT = prog
BUILD_DIR = bin

RTOS_DIR = ../import/FreeRTOS-Kernel/include \
						 ../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3 \

RTOS_CFILES += $(wildcard ../import/FreeRTOS-Kernel/*.c) \
					$(wildcard ../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3/*.c) \
					../import/FreeRTOS-Kernel/portable/MemMang/heap_4.c

W5500_DIR = ../import/ioLibrary_Driver/Ethernet \
						../import/ioLibrary_Driver/Internet/httpServer 

W5500_CFILES = $(wildcard ../import/ioLibrary_Driver/Ethernet/*.c) \
							 $(wildcard ../import/ioLibrary_Driver/Internet/httpServer/*.c)

SHARED_DIR = ../import $(RTOS_DIR) $(W5500_DIR)
CFILES = main.c log.c
CFILES += $(RTOS_CFILES) $(W5500_CFILES) ./spi_w5500.c ./web_server.c

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
