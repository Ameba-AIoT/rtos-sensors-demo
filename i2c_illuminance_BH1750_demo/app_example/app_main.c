#include "ameba_soc.h"
#include "os_wrapper.h"
#include "bh1750_demo.h"

void app_example(void)
{
    sys_init();
    RTK_LOGI("BH1750", "bh1750_task creat!\r\n");
    // Create light reading task
    if (rtos_task_create(NULL, "app_lightread", (rtos_task_t)app_lightread, NULL, (3072), (1)) != RTK_SUCCESS)
    {
        RTK_LOGI("bh1750 task", "Cannot create\n\r");
    }

}
