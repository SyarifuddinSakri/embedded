PROJECT = prog
BUILD_DIR = bin

SHARED_DIR = ../import/FreeRTOS-Kernel \
						 ../import/FreeRTOS-Kernel/include \
						 ../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3

CFILES = main.c
CFILES += ../import/FreeRTOS-Kernel/tasks.c \
					../import/FreeRTOS-Kernel/timers.c \
					../import/FreeRTOS-Kernel/queue.c \
					../import/FreeRTOS-Kernel/list.c \
					../import/FreeRTOS-Kernel/croutine.c \
					../import/FreeRTOS-Kernel/event_groups.c \
					../import/FreeRTOS-Kernel/portable/GCC/ARM_CM3/port.c \
					../import/FreeRTOS-Kernel/portable/MemMang/heap_4.c

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
