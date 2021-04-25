#include <stddef.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t g_semphr;

void timeout_callback(void)
{
        BaseType_t should_yield = pdFALSE;

        xSemaphoreGiveFromISR(g_semphr, &should_yield);

        /* we cannot use portYIELD_FROM_ISR here because it is not end of ISR */
        portYIELD_FROM_ISR(should_yield);
}

void timer_isr_handler(union sigval arg)
{
        void (*func)(void) = arg.sival_ptr;

        /* something before soft isr handler */
        func();
        /* something after soft isr handler */

        /* should never reach here in real app */
}

void create_timer(void (*func)(void), long long ms)
{
        long long t_sec = ms / 1000ULL;
        long long t_nsec = (ms * 1000000ULL) % 1000000000000ULL;

        timer_t timer_id = {0};
        struct itimerspec ts = {0};
        struct sigevent se = {0};

        se.sigev_notify = SIGEV_THREAD;
        se.sigev_value.sival_ptr = (void*)func;
        se.sigev_notify_function = timer_isr_handler;
        se.sigev_notify_attributes = NULL;

        ts.it_value.tv_sec = t_sec;
        ts.it_value.tv_nsec = t_nsec;
        ts.it_interval.tv_sec = t_sec;
        ts.it_interval.tv_nsec = t_nsec;

        timer_create(CLOCK_REALTIME, &se, &timer_id);
        timer_settime(timer_id, 0, &ts, 0);
}

static void task_service(void *pvParameters)
{
        while(1) {
                /* wait for notify from ISR */
                xSemaphoreTake(g_semphr, portMAX_DELAY);

                FreeRTOS_printf("timeout notify: tick = %lu\n", xTaskGetTickCount());
        }
}

int main(void)
{
        xTaskCreate(task_service, "task_high", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);

        g_semphr = xSemaphoreCreateBinary();
        
        create_timer(timeout_callback, 500);
        
        vTaskStartScheduler();
}
