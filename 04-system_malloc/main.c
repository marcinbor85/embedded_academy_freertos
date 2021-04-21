#include <stddef.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

static void task_bad_service(void *pvParameters)
{
        while(1) {
                /* is it ok to use malloc here? */
                uint8_t *data = malloc(100);

                /* do some fancy operations here */
                
                free(data);

                /* maybe delay will solve problem? */
                // vTaskDelay(1);
        }
}

static void task_good_service(void *pvParameters)
{
        while(1) {
                /* is it ok to use pvPortMalloc here? */
                uint8_t *data = pvPortMalloc(100);

                /* do some fancy operations here */

                vPortFree(data);

                /* maybe delay will solve problem? */
                // vTaskDelay(1);
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
