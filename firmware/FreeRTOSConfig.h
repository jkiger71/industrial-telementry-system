#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

// --------------------------------------------------------------------
// ⚙️ Hardware and Clock Settings
// --------------------------------------------------------------------
// The RP2040 standard system clock runs at 125 MHz
#define configCPU_CLOCK_HZ                         125000000UL
#define configTICK_RATE_HZ                         ((TickType_t)1000)
#define configMAX_PRIORITIES                       5
#define configMINIMAL_STACK_SIZE                   ((unsigned short)128)

// --------------------------------------------------------------------
// 🧠 Memory Allocation (Heap 4 Model)
// --------------------------------------------------------------------
// Allocate 64KB of the Pico's 264KB SRAM explicitly to the FreeRTOS heap
#define configTOTAL_HEAP_SIZE                      ((size_t)(64 * 1024))
#define configAPPLICATION_ALLOCATED_HEAP           0

// --------------------------------------------------------------------
// 🔀 Kernel Scheduling Behavior
// --------------------------------------------------------------------
#define configUSE_PREEMPTION                       1
#define configUSE_IDLE_HOOK                        0
#define configUSE_TICK_HOOK                        0
#define configUSE_CO_ROUTINES                      0
#define configUSE_16_BIT_TICKS                     0
#define configUSE_MUTEXES                          1
#define configUSE_TIMERS                           1
#define configTIMER_TASK_PRIORITY                  (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                   10
#define configTIMER_TASK_STACK_DEPTH               256

// --------------------------------------------------------------------
// 🛠️ Optional Kernel Features (Enables Tasks & Time Management)
// --------------------------------------------------------------------
#define INCLUDE_vTaskPrioritySet                   1
#define INCLUDE_uxTaskPriorityGet                  1
#define INCLUDE_vTaskDelete                        1
#define INCLUDE_vTaskCleanUpResources              1
#define INCLUDE_vTaskSuspend                       1
#define INCLUDE_vTaskDelayUntil                    1
#define INCLUDE_vTaskDelay                         1
#define INCLUDE_uxTaskGetStackHighWaterMark        1

// --------------------------------------------------------------------
// 💥 Critical Error Handling Hooks
// --------------------------------------------------------------------
// Halts execution if a task overflows its allocated stack memory
#define configUSE_MALLOC_FAILED_HOOK               0
#define configCHECK_FOR_STACK_OVERFLOW             2

// --------------------------------------------------------------------
// 📌 RP2040 ARM Cortex-M0+ Specific Interrupt Settings
// --------------------------------------------------------------------
#define configKERNEL_INTERRUPT_PRIORITY            255
#define configMAX_SYSCALL_INTERRUPT_PRIORITY       191

// Map FreeRTOS handlers smoothly to standard ARM exception vectors
#define vPortSVCHandler                             isr_svc
#define xPortPendSVHandler                          isr_pendsv
#define xPortSysTickHandler                         isr_systick

#endif /* FREERTOS_CONFIG_H */
