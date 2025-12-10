# FreeRTOS 任务创建与删除测试

这是我的第一个 STM32 FreeRTOS 练习项目。
功能：使用两个按钮控制两个 LED 任务的删除，同时串口打印创建任务的流程。
111
## 1. 任务流程

1. **配置config.h**
2. **创建软件定时器**
3. **编写回调函数实体** 
4. **启动软件定时器** (xTaskCreate)
5. **启动调度器**

## 2. 关键代码

```c
#define configUSE_TIMERS                         1
#define configTIMER_TASK_PRIORITY                31
#define configTIMER_QUEUE_LENGTH                 10
#define configTIMER_TASK_STACK_DEPTH             256

pdMS_TO_TICKS();//xx毫秒

TimerHandle_t xTimerCreate( 
    const char * const pcTimerName,   // 1. 名字（调试用）
    const TickType_t xTimerPeriodInTicks, // 2. 周期（Tick数，用 pdMS_TO_TICKS 转换）
    const UBaseType_t uxAutoReload,   // 3. 模式：pdTRUE(周期) / pdFALSE(单次)
    void * const pvTimerID,           // 4. ID号（通常填 NULL 或 0，用于区分不同定时器）
    TimerCallbackFunction_t pxCallbackFunction // 5. 回调函数名
);

xTimerStart(TimerHandle, 0);// 参数2：等待时间（如果命令队列满了等多久，通常填 0 不等待）
xTimerStop(TimerHandle, 0);
xTimerChangePeriod(TimerHandle, pdMS_TO_TICKS(500), 0);// 把周期改成 500ms，并立刻启动
BaseType_t xTimerReset( TimerHandle_t  xTimer, const TickType_t xTicksToWait);//复位
BaseType_t xTimerDelete( TimerHandle_t  xTimer, const TickType_t xTicksToWait); //删除