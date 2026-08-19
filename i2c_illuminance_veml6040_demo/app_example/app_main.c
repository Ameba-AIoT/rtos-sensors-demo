#include "ameba_soc.h"
#include "os_wrapper.h"
#include "veml6040_demo.h"

void app_example(void)
{
    sensor_sys_init();
    RTK_LOGI("VEML6040", "veml6040_task creat!\r\n");
    if (rtos_task_create(NULL, "app_illuminance_read", (rtos_task_t)app_illuminance_read, NULL, (3072), (1)) != RTK_SUCCESS)
    {
        RTK_LOGI("veml6040 task", "Cannot create\n\r");
    }
}
