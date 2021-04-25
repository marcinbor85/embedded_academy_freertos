#include <stddef.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t g_mutex;

static void some_short_operation(void *params)
{
        TickType_t delay = (TickType_t)params;

        xSemaphoreTake(g_mutex, portMAX_DELAY);
        vTaskDelay(delay);
        xSemaphoreGive(g_mutex);
}

static TickType_t measure_run_time(void (*func)(void *), void *params)
{
        TickType_t start_ticks = xTaskGetTickCount();
        func(params);
        TickType_t end_ticks = xTaskGetTickCount();

        return end_ticks - start_ticks;
}

static void task_hp_service(void *pvParameters)
{
        while(1) {
                /* take shared resource for short time */
                TickType_t run_time;
                run_time = measure_run_time(some_short_operation, (void*)1);
                FreeRTOS_printf("run_time: %lu\n", run_time);

                /* repeat this quite rarely */
                vTaskDelay(500);
        }
}

static void task_mp_service(void *pvParameters)
{
        while(1) {
                /* not related to others, but medium priority task */
                vTaskDelay(10);

                /* extensive task, high cpu usage */
                int n = 10000000;
                while (n-- > 0) {};
        }
}

static void task_lp_service(void *pvParameters)
{
        while(1) {
                /* take shared resource for short time */
                some_short_operation((void*)1);
                /* repeat this quite often */
                vTaskDelay(1);
        }
}

int main(void)
{
        xTaskCreate(task_hp_service, "task_high", configMINIMAL_STACK_SIZE, NULL,       3, NULL);
        xTaskCreate(task_mp_service, "task_medium", configMINIMAL_STACK_SIZE, NULL,     2, NULL);
        xTaskCreate(task_lp_service, "task_low", configMINIMAL_STACK_SIZE, NULL,        1, NULL);

        g_mutex = xSemaphoreCreateBinary();
        xSemaphoreGive(g_mutex);
        // g_mutex = xSemaphoreCreateMutex();
        
        vTaskStartScheduler();
}
