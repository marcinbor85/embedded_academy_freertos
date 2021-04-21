#include <stddef.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#define MESSAGE_MAX_SIZE        100UL

static MessageBufferHandle_t g_message_buf;

const char *g_dict1[] = {
        "1 - message",
        "1 - something important",
        "1 - very very huuuuuge message",
};

const char *g_dict2[] = {
        "2 - test",
        "2 - this is a cat",
        "2 - i dont know what im doing",
};

typedef struct {
        char const **dict;
        size_t dict_size;
        BaseType_t period;
} init_data_t;

static const init_data_t g_init[] = {
        {
                .dict = g_dict1,
                .dict_size = sizeof(g_dict1) / sizeof(g_dict1[0]),
                .period = 500UL / portTICK_RATE_MS,
        },
        {
                .dict = g_dict2,
                .dict_size = sizeof(g_dict2) / sizeof(g_dict2[0]),
                .period = 1000UL / portTICK_RATE_MS,
        },
};

static void print_message(const char *message)
{
        FreeRTOS_printf("message: %s\n", message);
}

static void task_consumer_service(void *pvParameters)
{
        char message[MESSAGE_MAX_SIZE];

        while(1) {
                size_t s = xMessageBufferReceive(g_message_buf, message, sizeof(message), portMAX_DELAY);
                configASSERT(s > 0);

                print_message(message);
        }
}

static void task_producer_service(void *pvParameters)
{
        init_data_t const *init = pvParameters;
        int index = 0;

        while(1) {
                char const *message = init->dict[index++];
                index %= init->dict_size;

                /* maybe add some mutex? */
                xMessageBufferSend(g_message_buf, message, strlen(message) + 1, portMAX_DELAY);

                /* what will happen if we remove/change delay? */
                vTaskDelay(init->period);
        }
}

int main(void)
{
        BaseType_t s;

        /* message buffers instead of queue of pointers */
        g_message_buf = xMessageBufferCreate(MESSAGE_MAX_SIZE);

        s = xTaskCreate(task_consumer_service, "task_consumer", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        /* common but parametrized producer services tasks */
        s = xTaskCreate(task_producer_service, "task_producer1", configMINIMAL_STACK_SIZE, (void*)&g_init[0], configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);
        s = xTaskCreate(task_producer_service, "task_producer2", configMINIMAL_STACK_SIZE, (void*)&g_init[1], configMAX_PRIORITIES - 1, NULL);
        configASSERT(s != pdFALSE);

        vTaskStartScheduler();
        configASSERT(pdFALSE);
}
