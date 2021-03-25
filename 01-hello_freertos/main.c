#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"

static void task_service(void *pvParameters)
{
        while(1) {
                vTaskDelay(500UL / portTICK_RATE_MS);

                BaseType_t current_tick = xTaskGetTickCount();
                FreeRTOS_printf("hello freertos: tick = %lu\n", current_tick);
        }
}

int main(void)
{
        BaseType_t s = xTaskCreate(task_service, "task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        configASSERT(s != pdFALSE);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
