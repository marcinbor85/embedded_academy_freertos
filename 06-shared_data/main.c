#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t g_mutex;

struct point {
        uint32_t x;
        uint32_t y;
};

static struct point g_shared_protected_point;
static struct point g_shared_unprotected_point;

static void check_point_thread_unsafe(struct point *point, const char *name)
{
        /* how can we make it thread-safe? */

        if (point->x != point->y) {
                /* in case of failure lets start self-destruction procedure */

                FreeRTOS_printf("%s: mismatch: %lu != %lu\n",
                        name,
                        point->x,
                        point->y
                );
        }
}

static void check_point(struct point *point, const char *name)
{
        struct point p = {
                .x = point->x,
                .y = point->y,
        };

        /* can we use memcpy? */
        // memcpy((uint8_t*)&p, (uint8_t*)point, sizeof(p));

        if (p.x != p.y) {
                /* in case of failure lets start self-destruction procedure */

                FreeRTOS_printf("%s: mismatch: %lu != %lu\n",
                        name,
                        p.x,
                        p.y
                );
        }
}

static void modify_point(struct point *point)
{
        /* thread-safe or not? */

        point->x++;
        point->y++;
}

static void task_read_service(void *pvParameters)
{
        /* extensive task which operate on global shared variable as fast as possible */

        while(1) {
                check_point_thread_unsafe(&g_shared_unprotected_point, "bad");

                xSemaphoreTake(g_mutex, portMAX_DELAY);
                check_point_thread_unsafe(&g_shared_protected_point, "good");
                xSemaphoreGive(g_mutex);

                /* maybe add some evil delay? */
                // vTaskDelay(1);
        }
}

static void task_write_service(void *pvParameters)
{
        while(1) {
                modify_point(&g_shared_unprotected_point);

                xSemaphoreTake(g_mutex, portMAX_DELAY);
                modify_point(&g_shared_protected_point);
                xSemaphoreGive(g_mutex);

                vTaskDelay(1);
        }
}

int main(void)
{
        BaseType_t s;

        /* maybe some play with priorities? */

        s = xTaskCreate(task_read_service, "task_read", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(task_write_service, "task_write", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        g_mutex = xSemaphoreCreateMutex();
        configASSERT(g_mutex != NULL);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
