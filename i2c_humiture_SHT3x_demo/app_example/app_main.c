#include "sht3x_demo.h"

void app_example(void)
{
    RTK_LOGI(TAG, "sht3x_task creat!\r\n");
    if (rtos_task_create(NULL, "sht3x_task", (rtos_task_t) sht3x_task, NULL, 1024 * 4, 1) != RTK_SUCCESS)
    {
        RTK_LOGE(TAG, "\n\r%s sht3x_task(init_thread) failed", __FUNCTION__);
    }

}
