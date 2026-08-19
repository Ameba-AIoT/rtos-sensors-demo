#include "ameba_soc.h"
#include "os_wrapper.h"
#include "apds9960_demo.h"

void app_example(void)
{
    sys_init();
    RTK_LOGI("APDS", "apds_task creat!\r\n");
    if (rtos_task_create(NULL, "app_apds_read", (rtos_task_t)app_apds, NULL, (3072), (1)) != RTK_SUCCESS)
    {
        RTK_LOGI("apds1080 task", "Cannot create\n\r");
    }

}
