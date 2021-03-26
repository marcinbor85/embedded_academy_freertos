/*
MIT License

Copyright (c) 2021 Marcin Borowicz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef OSAL_TYPES_H
#define OSAL_TYPES_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"

typedef TaskFunction_t          os_task_service_f;

typedef TaskHandle_t            os_task_t;
typedef SemaphoreHandle_t       os_semaphore_t;
typedef SemaphoreHandle_t       os_mutex_t;
typedef QueueHandle_t           os_queue_t;
typedef EventGroupHandle_t      os_event_group_t;   

typedef enum {
    OS_TASK_PRIORITY_LOW        = 1,
    OS_TASK_PRIORITY_MEDIUM     = configMAX_PRIORITIES / 2,
    OS_TASK_PRIORITY_HIGH       = configMAX_PRIORITIES - 1,
} os_task_priority_t;

#define OS_STACK_MINIMAL_SIZE   ((uint32_t)configMINIMAL_STACK_SIZE)
#define OS_MAX_DELAY            ((uint32_t)portMAX_DELAY)

#define OS_PRINTF(...)          FreeRTOS_printf(__VA_ARGS__)
#define OS_ASSERT(...)          configASSERT(__VA_ARGS__)

#endif /* OSAL_TYPES_H */
