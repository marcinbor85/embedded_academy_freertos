#include <stddef.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

static QueueHandle_t g_queue_int;
static QueueHandle_t g_queue_char;

static QueueHandle_t g_queue_common;
static QueueSetHandle_t g_queue_set;

typedef enum {
        TYPE_CHAR,
        TYPE_INT,
} queue_item_type_t;

typedef struct {
        queue_item_type_t type;
        union {
                char char_val;
                int int_val;
        };
} queue_item_t;

static void print_int(int i)
{
        FreeRTOS_printf("int_val: %d\n", i);
}

static void print_char(char ch)
{
        FreeRTOS_printf("char_val: %c\n", ch);
}

static void task_consumer_bad_service(void *pvParameters)
{
        while(1) {
                BaseType_t recv;

                int i;
                recv = xQueueReceive(g_queue_int, &i, 0); /* without blocking */
                if (recv != pdFALSE)
                        print_int(i);

                char ch;
                recv = xQueueReceive(g_queue_char, &ch, 0); /* without blocking */
                if (recv != pdFALSE)
                        print_char(ch);
                
                FreeRTOS_printf("bad loop\n");

                /* what will happen if we remove this delay? */
                /* when we could ommit it */
                vTaskDelay(1);
        }
}

static void task_consumer_better_service(void *pvParameters)
{
        while(1) {
                queue_item_t item;
                BaseType_t recv = xQueueReceive(g_queue_common, &item, portMAX_DELAY); /* blocking */
                /* is checking flag required? */
                if (recv == pdFALSE)
                        continue;

                switch (item.type) {
                case TYPE_INT:
                        print_int(item.int_val);
                        break;
                case TYPE_CHAR:
                        print_char(item.char_val);
                        break;
                default:
                        /* what if unsupported type? */
                        break;
                }

                FreeRTOS_printf("loop\n");

                /* no delay required */
        }
}

static void task_consumer_best_service(void *pvParameters)
{
        while(1) {
                BaseType_t recv;
                QueueSetMemberHandle_t selected_queue;
                selected_queue = xQueueSelectFromSet(g_queue_set, portMAX_DELAY); /* blocking */
                
                if (selected_queue == g_queue_int) {
                        int i;
                        recv = xQueueReceive(g_queue_int, &i, 0); /* without blocking */
                        /* is checking flag required? */
                        if (recv != pdFALSE)
                                print_int(i);
                } else if (selected_queue == g_queue_char) {
                        char ch;
                        recv = xQueueReceive(g_queue_char, &ch, 0); /* without blocking */
                        /* is checking flag required? */
                        if (recv != pdFALSE)
                                print_char(ch);
                } else {
                        /* what if non of supported queues? */
                }

                FreeRTOS_printf("loop\n");
                
                /* no delay required */
        }
}

static void task_producer_int_service(void *pvParameters)
{
        int i = 0;
        queue_item_t item = {
                .int_val = 0,
                .type = TYPE_INT,
        };

        while(1) {
                xQueueSend(g_queue_int, &i, 0);
                xQueueSend(g_queue_common, &item, 0);

                item.int_val++;
                i++;
                
                vTaskDelay(5);
        }
}

static void task_producer_char_service(void *pvParameters)
{
        char ch = 'a';
        queue_item_t item = {
                .char_val = 'a',
                .type = TYPE_CHAR,
        };

        while(1) {
                xQueueSend(g_queue_char, &ch, 0);
                xQueueSend(g_queue_common, &item, 0);
                
                item.char_val++;
                ch++;
                
                vTaskDelay(7);
        }
}

int main(void)
{
        BaseType_t s;

        s = xTaskCreate(task_consumer_bad_service, "task_consumer", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        // s = xTaskCreate(task_consumer_better_service, "task_consumer", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        // s = xTaskCreate(task_consumer_best_service, "task_consumer", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        s = xTaskCreate(task_producer_int_service, "task_producer1", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);
        s = xTaskCreate(task_producer_char_service, "task_producer2", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        g_queue_int = xQueueCreate(4, sizeof(int));
        configASSERT(g_queue_int != NULL);
        g_queue_char = xQueueCreate(4, sizeof(char));
        configASSERT(g_queue_char != NULL);
        g_queue_common = xQueueCreate(4, sizeof(queue_item_t));
        configASSERT(g_queue_common != NULL);
        
        // g_queue_set = xQueueCreateSet(8);
        // configASSERT(g_queue_set != NULL);
        
        // xQueueAddToSet(g_queue_int, g_queue_set);
        // xQueueAddToSet(g_queue_char, g_queue_set);

        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
