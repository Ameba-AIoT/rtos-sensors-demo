#include "ameba_soc.h"
#include "os_wrapper.h"
#include "hdc1080_demo.h"

void app_example(void)
{     
    sys_init();
    RTK_LOGI("HDC1080", "hdc1080_task creat!\r\n");   
    // 创建温湿度读取任务
	if (rtos_task_create(NULL, "app_temp_humi_read", (rtos_task_t)app_temp_humi_read, NULL, (3072), (1)) != RTK_SUCCESS) {
		RTK_LOGI("hdc1080 task", "Cannot create\n\r");
	}
    
}
