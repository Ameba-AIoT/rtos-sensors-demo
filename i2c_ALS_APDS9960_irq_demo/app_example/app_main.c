#include "apds_9960_demo.h"

void app_example(void)
{
    RTK_LOGI(TAG, "apds9960_demo_task creat!\r\n");
    if (rtos_task_create(NULL, "apds9960_demo_task", (rtos_task_t) apds9960_demo_task, NULL, 1024 * 4, 1) != RTK_SUCCESS)
    {
        RTK_LOGE(TAG, "\n\r%s apds9960_demo_task(init_thread) failed", __FUNCTION__);
    }

}
