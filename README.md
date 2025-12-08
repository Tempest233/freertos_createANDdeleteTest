# FreeRTOS 任务创建与删除测试

这是我的第一个 STM32 FreeRTOS 练习项目。
功能：使用两个按钮控制两个 LED 任务的删除，同时串口打印创建任务的流程。
111
## 1. 任务流程

1. **定义任务句柄** (身份证)
2. **声明任务函数**
3. **编写任务函数实体** (任务 A: 负责闪灯)
4. **创建任务** (xTaskCreate)
5. **启动调度器**

## 2. 关键代码

```c
TaskHandle_t LED_Task_Handle;
void LED0_Entry(void *pvParameters);
void LED_Task_Func(void *pvParameters)
{
    for(;;)
    {
        // 翻转电平
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
        // 延时 500ms
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

xTaskCreate(
    (TaskFunction_t )LED_Task_Func,     // 任务函数
    (const char*    )"LED_Task",        // 名字
    (uint16_t       )128,               // 堆栈大小
    (void*          )NULL,              // 传入参数
    (UBaseType_t    )1,                 // 优先级
    (TaskHandle_t*  )&LED_Task_Handle   // 句柄
)