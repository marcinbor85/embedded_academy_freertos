#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "event_groups.h"

#define DELAY_MS           500UL
#define DELAY_TICKS        ( DELAY_MS / portTICK_RATE_MS )

struct point {
        uint32_t x;
        uint32_t y;
};

static TimerHandle_t g_timer_good;
static TimerHandle_t g_timer_bad;

static QueueHandle_t g_queue_good;
static QueueHandle_t g_queue_bad;

static void print_point(struct point *point, const char *text)
{
        FreeRTOS_printf("%s: point = (%lu, %lu)\n", text, point->x, point->y);
}

static void some_common_operation(void)
{
        /* what about delays? */
        //vTaskDelay(1000);
}

static void task_consumer_service(void *pvParameters)
{
        /* can we use static declarations here? */

        struct point *point;
        QueueHandle_t queue = pvParameters;

        while(1) {
                BaseType_t s = xQueueReceive(queue, &point, portMAX_DELAY);
                if (s != pdPASS)
                        continue;

                if (queue == g_queue_good) {
                        print_point(point, "good");
                        vPortFree(point);
                } else if (queue == g_queue_bad) {
                        print_point(point, "bad");
                }

                some_common_operation();
        }
}

static void timer_good_callback(TimerHandle_t xTimer)
{
        static uint32_t cntr = 0;

        struct point *point = pvPortMalloc(sizeof(struct point));
        point->x = cntr++;
        point->y = cntr++;

        xQueueSend(g_queue_good, &point, portMAX_DELAY); /* caution!!! */
}

static void timer_bad_callback(TimerHandle_t xTimer)
{
        static uint32_t cntr = 0;

        /* maybe static? */
        struct point point;
        point.x = cntr++;
        point.y = cntr++;

        struct point *ptr_point = &point;

        xQueueSend(g_queue_bad, &ptr_point, portMAX_DELAY); /* caution!!! */
}

int main(void)
{
        BaseType_t s;

        /* when queue items should be pointers, when not */

        g_queue_good = xQueueCreate(4, sizeof(struct point*));
        configASSERT(g_queue_good != NULL);
        g_queue_bad  = xQueueCreate(4, sizeof(struct point*));
        configASSERT(g_queue_bad != NULL);

        /* common but parametrized consumer services tasks */
        s = xTaskCreate(task_consumer_service, "task_good", configMINIMAL_STACK_SIZE, g_queue_good, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);
        s = xTaskCreate(task_consumer_service, "task_bad", configMINIMAL_STACK_SIZE, g_queue_bad, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        g_timer_good = xTimerCreate("timer_good", DELAY_TICKS, pdTRUE, NULL, timer_good_callback);
        configASSERT(g_timer_good != NULL);
        g_timer_bad = xTimerCreate("timer_bad", DELAY_TICKS, pdTRUE, NULL, timer_bad_callback);
        configASSERT(g_timer_bad != NULL);

        /* we can use 0 delay before sheduler start */
        xTimerStart(g_timer_good, 0);
        xTimerStart(g_timer_bad, 0);

        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
