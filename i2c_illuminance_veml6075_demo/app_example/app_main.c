#include "ameba_soc.h"
#include "os_wrapper.h"
#include "veml6075_demo.h"

void app_example(void)
{
    sensor_sys_init();
    RTK_LOGI("VEML6075", "veml6075_task creat!\r\n");

    if (rtos_task_create(NULL, "app_uv_read", (rtos_task_t)app_uv_read, NULL, (3072), (1)) != RTK_SUCCESS)
    {
        RTK_LOGI("VEML6075 task", "Cannot create\n\r");
    }
}
