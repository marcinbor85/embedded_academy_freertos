#include <stddef.h>

#include "osal.h"

static void task_service(void *params)
{
        while(1) {
                os_delay_ms(500UL);

                uint32_t current_tick = os_get_current_tick();
                OS_PRINTF("hello: tick = %lu\n", current_tick);
        }
}

int main(void)
{
        os_task_t task = os_task_create(task_service, "task", NULL, OS_STACK_MINIMAL_SIZE, OS_TASK_PRIORITY_LOW);
        OS_ASSERT(task != NULL);
        
        os_scheduler_start();
        OS_ASSERT(false);
}
