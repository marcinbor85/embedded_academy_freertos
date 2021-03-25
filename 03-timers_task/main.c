#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"

#define DELAY_MS           500UL
#define DELAY_TICKS        ( DELAY_MS / portTICK_RATE_MS )

static TimerHandle_t g_timer_slow_1;
static TimerHandle_t g_timer_slow_2;
static TimerHandle_t g_timer_fast;

static EventGroupHandle_t g_event_group;

static void time_consuming_operation(const char *text)
{
        BaseType_t current_tick = xTaskGetTickCount();
        FreeRTOS_printf("%s: tick = %lu\n", text, current_tick);
        vTaskDelay(10); /* emulation of time-consuming operation */
}

static void task_service(void *pvParameters)
{
        while(1) {
                xEventGroupWaitBits(g_event_group, 1, pdTRUE, pdFALSE, portMAX_DELAY);

                time_consuming_operation("almost good");
        }
}

static void timer_slow_callback(TimerHandle_t xTimer)
{
        const char *id = pvTimerGetTimerID(xTimer);
        time_consuming_operation(id);
}

static void timer_fast_callback(TimerHandle_t xTimer)
{
        xEventGroupSetBits(g_event_group, 1);
}

int main(void)
{
        BaseType_t s;

        s = xTaskCreate(task_service, "task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        g_timer_slow_1 = xTimerCreate("timer_slow_1", DELAY_TICKS, pdTRUE, "bad_1", timer_slow_callback);
        configASSERT(g_timer_slow_1 != NULL);
        g_timer_slow_2 = xTimerCreate("timer_slow_2", DELAY_TICKS, pdTRUE, "bad_2", timer_slow_callback);
        configASSERT(g_timer_slow_2 != NULL);
        g_timer_fast = xTimerCreate("g_timer_fast", DELAY_TICKS, pdTRUE, NULL, timer_fast_callback);
        configASSERT(g_timer_fast != NULL);

        g_event_group = xEventGroupCreate();
        configASSERT(g_event_group != NULL);

        xTimerStart(g_timer_slow_1, 0);
        xTimerStart(g_timer_slow_2, 0);
        xTimerStart(g_timer_fast, 0);
        
        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
