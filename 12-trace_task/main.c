#include <stddef.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

static int g_counter;

static void task_1_service(void *pvParameters)
{
        while(1) {
                /* extensive task which operate as fast as possible without blocking but with low priority */
                g_counter++;
                continue;
        }
}

static void task_2_service(void *pvParameters)
{
        while(1) {
                vTaskDelay(10UL);
                FreeRTOS_printf("task_2_service, tick = %lu\n", xTaskGetTickCount());
        }
}

int main(void)
{
        BaseType_t s;

        /* low priority task */
        s = xTaskCreate(task_1_service, "task_1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        configASSERT(s != pdFALSE);

        /* high priority task */
        s = xTaskCreate(task_2_service, "task_2", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
