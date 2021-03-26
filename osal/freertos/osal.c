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

#include "osal.h"

#include <stdbool.h>

os_task_t os_task_create(os_task_service_f service, const char *name, void *params, uint32_t stack_size, os_task_priority_t prio)
{
        BaseType_t s;
        os_task_t task;

        s = xTaskCreate(service, name, stack_size, params, prio, &task);
        if (s != pdPASS)
                task = NULL;

        return task;
}

void os_task_delay_ms(uint32_t ms)
{
        vTaskDelay(ms / portTICK_RATE_MS);
}

void os_task_delay_tick(uint32_t tick)
{
        vTaskDelay(tick);
}

os_queue_t os_queue_create(uint32_t capacity, uint32_t item_size)
{
        return xQueueCreate(capacity, item_size);
}

bool os_queue_send(os_queue_t queue, void *item, uint32_t wait_ms)
{
        bool ret = false;
        TickType_t wait = (wait_ms == OS_MAX_DELAY) ? portMAX_DELAY : wait_ms / portTICK_RATE_MS;

        BaseType_t s = xQueueSend(queue, item, wait);
        if (s == pdTRUE)
                ret = true;
        
        return ret;
}

bool os_queue_recv(os_queue_t queue, void *item, uint32_t wait_ms)
{
        bool ret = false;
        TickType_t wait = (wait_ms == OS_MAX_DELAY) ? portMAX_DELAY : wait_ms / portTICK_RATE_MS;

        BaseType_t s = xQueueReceive(queue, item, wait);
        if (s == pdTRUE)
                ret = true;
        
        return ret;
}

os_semaphore_t os_semaphore_create(void)
{
        return xSemaphoreCreateBinary();
}

void os_semaphore_give(os_semaphore_t sem)
{
        xSemaphoreGive(sem);
}

bool os_semaphore_take(os_semaphore_t sem, uint32_t wait_ms)
{
        bool ret = false;
        TickType_t wait = (wait_ms == OS_MAX_DELAY) ? portMAX_DELAY : wait_ms / portTICK_RATE_MS;

        BaseType_t s = xSemaphoreTake(sem, wait);
        if (s == pdTRUE)
                ret = true;
        
        return ret;
}

os_mutex_t os_mutex_create(void)
{
        return xSemaphoreCreateMutex();
}

void os_mutex_give(os_mutex_t mux)
{
        xSemaphoreGive(mux);
}

bool os_mutex_take(os_mutex_t mux, uint32_t wait_ms)
{
        bool ret = false;
        TickType_t wait = (wait_ms == OS_MAX_DELAY) ? portMAX_DELAY : wait_ms / portTICK_RATE_MS;

        BaseType_t s = xSemaphoreTake(mux, wait);
        if (s == pdTRUE)
                ret = true;
        
        return ret;
}

os_event_group_t os_event_group_create(void)
{
        return xEventGroupCreate();
}

uint32_t os_event_group_wait(os_event_group_t event_group, uint32_t bits, bool clear_on_exit, bool wait_for_all, uint32_t wait_ms)
{
        bool ret = false;
        TickType_t wait = (wait_ms == OS_MAX_DELAY) ? portMAX_DELAY : wait_ms / portTICK_RATE_MS;

        uint32_t res = xEventGroupWaitBits(event_group, bits, clear_on_exit, wait_for_all, wait);
        return res;
}

uint32_t os_event_group_set(os_event_group_t event_group, uint32_t bits)
{
        return xEventGroupSetBits(event_group, bits);
}

uint32_t os_event_group_get(os_event_group_t event_group)
{
        return xEventGroupGetBits(event_group);
}

uint32_t os_get_current_ms(void)
{
        uint32_t ticks;
        uint32_t ms;

        ticks = xTaskGetTickCount();
        ms = ticks * portTICK_RATE_MS;

        return ms;
}

uint32_t os_get_current_tick(void)
{
        return xTaskGetTickCount();
}

void* os_malloc(uint32_t size)
{
        return pvPortMalloc(size);
}

void os_free(void *ptr)
{
        vPortFree(ptr);
}

void os_scheduler_start(void)
{
        vTaskStartScheduler();
}
