
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*#include "stm32f10x.h"  // Use the correct STM32 header file*/

#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCPU_CLOCK_HZ                      ( 72000000UL )  // 72 MHz for STM32F103
#define configTICK_RATE_HZ                      ( 1000 )        // 1 ms tick
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                ( 128 )
#define configTOTAL_HEAP_SIZE                   ( 8 * 1024 )  // 8 KB heap
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1

// Mutex and Semaphore support
#define configUSE_MUTEXES                       1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_RECURSIVE_MUTEXES             1

// Enable hooks for debugging
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configASSERT(x) if((x) == 0) { taskDISABLE_INTERRUPTS(); for( ;; ); }

// Cortex-M3 specific definitions
#define configPRIO_BITS                         4  // STM32F103 has 4 priority bits
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY         (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

// Enable FreeRTOS API functions
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelayUntil                 1

#endif /* FREERTOS_CONFIG_H */
