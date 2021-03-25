#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"

#define DELAY_MS           500UL
#define DELAY_TICKS        ( DELAY_MS / portTICK_RATE_MS )

static void time_consuming_operation(const char *text)
{
        BaseType_t current_tick = xTaskGetTickCount();
        FreeRTOS_printf("%s: tick = %lu\n", text, current_tick);
        vTaskDelay(10); /* emulation of time-consuming operation */
}

static void task_bad_service(void *pvParameters)
{
        while(1) {
                vTaskDelay(DELAY_TICKS);

                time_consuming_operation("bad");
        }
}

static void task_good_service(void *pvParameters)
{
        TickType_t last_tick = xTaskGetTickCount();

        while(1) {
                vTaskDelayUntil(&last_tick, DELAY_TICKS);

                time_consuming_operation("good");
        }
}

/* can we use timers instead of tasks services? */

int main(void)
{
        BaseType_t s;

        s = xTaskCreate(task_bad_service, "task_bad", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(task_good_service, "task_good", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        configASSERT(s != pdFALSE);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
