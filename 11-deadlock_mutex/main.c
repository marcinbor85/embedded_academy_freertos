#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

typedef struct  {
        int data;
        char const *name;
        SemaphoreHandle_t mutex;
} resource_t;

static resource_t g_resource1;
static resource_t g_resource2;

static void resource_init(resource_t *resource, const char *name)
{
        resource->data = 0;
        resource->name = name;
        resource->mutex = xSemaphoreCreateMutex();
        configASSERT(resource->mutex != NULL);
}

static void resource_lock(resource_t *resource)
{
        TaskStatus_t status;
        vTaskGetInfo(NULL, &status, 0, 0);

        FreeRTOS_printf("wait for [%s] mutex from %s\n", resource->name, status.pcTaskName);
        xSemaphoreTake(resource->mutex, portMAX_DELAY);
}

static void resource_operation(resource_t *resource)
{
        TaskStatus_t status;
        vTaskGetInfo(NULL, &status, 0, 0);

        FreeRTOS_printf("[%s] operation from %s: VAL = %d\n", resource->name, status.pcTaskName, resource->data);
        /* some dummy operation */
        resource->data++;

        vTaskDelay(10UL);
}

static void resource_unlock(resource_t *resource)
{
        xSemaphoreGive(resource->mutex);
}

static void task1_service(void *pvParameters)
{
        while(1) {
                /* is playing with delays will help? */
                vTaskDelay(100UL);

                /* maybe use one common mutex? */
                resource_lock(&g_resource1);
                {
                        resource_operation(&g_resource1);

                        /* maybe release previous one before waiting for next one? */

                        /* nested lock */                        
                        resource_lock(&g_resource2);
                        resource_operation(&g_resource2);
                        resource_unlock(&g_resource2);
                }
                resource_unlock(&g_resource1);
        }
}

static void task2_service(void *pvParameters)
{
        while(1) {
                vTaskDelay(100UL);

                resource_lock(&g_resource2);
                {
                        resource_operation(&g_resource2);
                       
                        resource_lock(&g_resource1);
                        resource_operation(&g_resource1);
                        resource_unlock(&g_resource1);
                }
                resource_unlock(&g_resource2);
        }
}

int main(void)
{
        BaseType_t s;

        /* is playing with priorities will help? */
        s = xTaskCreate(task1_service, "task1", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(task2_service, "task2", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        resource_init(&g_resource1, "res1");
        resource_init(&g_resource2, "res2");
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
