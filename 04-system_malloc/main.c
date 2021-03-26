#include <stddef.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

static void task_bad_service(void *pvParameters)
{
        while(1) {
                uint8_t *data = malloc(1000);

                free(data);
        }
}

static void task_good_service(void *pvParameters)
{
        while(1) {
                uint8_t *data = pvPortMalloc(1000);

                vPortFree(data);
        }
}

int main(void)
{
        BaseType_t s;

        s = xTaskCreate(task_bad_service, "task_bad", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(task_good_service, "task_good", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
