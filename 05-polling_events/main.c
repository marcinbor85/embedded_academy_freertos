#include <stddef.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"

#define DELAY_MS           500UL
#define DELAY_TICKS        ( DELAY_MS / portTICK_RATE_MS )

static TimerHandle_t g_timer_event;
static TimerHandle_t g_timer_poll;

static EventGroupHandle_t g_event_group;
static bool g_poll_flag;

static void time_consuming_operation(const char *text)
{
        BaseType_t current_tick = xTaskGetTickCount();
        FreeRTOS_printf("%s: tick = %lu\n", text, current_tick);
        vTaskDelay(10); /* emulation of time-consuming operation */
}

static void task_bad_service(void *pvParameters)
{
        while(1) {
                /* maybe some delay? */
                while (g_poll_flag == false) {
                        // vTaskDelay(1);
                        continue;
                }
                
                g_poll_flag = false;
                time_consuming_operation("bad");
        }
}

static void task_good_service(void *pvParameters)
{
        while(1) {
                xEventGroupWaitBits(g_event_group, 1, pdTRUE, pdFALSE, portMAX_DELAY);

                time_consuming_operation("good");
        }
}

static void timer_event_callback(TimerHandle_t xTimer)
{
        xEventGroupSetBits(g_event_group, 1);
}

static void timer_poll_callback(TimerHandle_t xTimer)
{
        g_poll_flag = true;
}

int main(void)
{
        BaseType_t s;

        /* what about priorities? */

        s = xTaskCreate(task_bad_service, "task_bad", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);
        s = xTaskCreate(task_good_service, "task_good", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        g_timer_event = xTimerCreate("timer_event", DELAY_TICKS, pdTRUE, NULL, timer_event_callback);
        configASSERT(g_timer_event != NULL);
        g_timer_poll = xTimerCreate("timer_poll", DELAY_TICKS, pdTRUE, NULL, timer_poll_callback);
        configASSERT(g_timer_poll != NULL);

        g_event_group = xEventGroupCreate();
        configASSERT(g_event_group != NULL);

        xTimerStart(g_timer_event, 0);
        xTimerStart(g_timer_poll, 0);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
