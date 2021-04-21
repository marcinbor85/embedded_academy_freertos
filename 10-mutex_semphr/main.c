#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t g_mutex;
static SemaphoreHandle_t g_semphr;

static int g_shared_data;

void print_value_thread_safe(void)
{
        xSemaphoreTake(g_mutex, portMAX_DELAY);
        FreeRTOS_printf("g_shared_data = %d, tick = %lu\n", g_shared_data, xTaskGetTickCount());
        xSemaphoreGive(g_mutex);
}

static void set_value_thread_safe(int val)
{
        /* really? */

        xSemaphoreTake(g_mutex, portMAX_DELAY);
        g_shared_data = val;
        xSemaphoreGive(g_mutex);
}

static void task_read_service(void *pvParameters)
{
        while(1) {
                /* simple synchronization, great! */
                xSemaphoreTake(g_semphr, portMAX_DELAY);

                print_value_thread_safe();
        }
}

static void task_write1_service(void *pvParameters)
{
        while(1) {
                vTaskDelay(1000UL);

                set_value_thread_safe(1000);
                xSemaphoreGive(g_semphr);

                // FreeRTOS_printf("does it even work?? YES\n");
        }
}

static void task_write2_service(void *pvParameters)
{
        while(1) {
                /* is the delay value important? */
                vTaskDelay(500UL);

                set_value_thread_safe(500);
                xSemaphoreGive(g_semphr);
        }
}

int main(void)
{
        BaseType_t s;

        /* is the task creating order important? */

        s = xTaskCreate(task_read_service, "task_read", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(task_write1_service, "task_write1", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(task_write2_service, "task_write2", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        /* can we replace mutex with semaphore? */
        g_mutex = xSemaphoreCreateMutex(); /* mutex created in the 'given' state */
        configASSERT(g_mutex != NULL);
        // xSemaphoreGive(g_mutex);

        /* can we replace semaphore with mutex? */
        g_semphr = xSemaphoreCreateBinary(); /* semaphore created in the 'taken' state */
        configASSERT(g_semphr != NULL);
        // xSemaphoreGive(g_semphr);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
