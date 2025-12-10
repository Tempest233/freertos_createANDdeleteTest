# FreeRTOS 软件定时器

功能：使用两个按钮控制定时器的启停，定时器的回调函数为翻转LED0。  
心得  
1.创建软件定时器时，回调函数创建和创建普通任务不一样，不需要xTaskCreate创建。只需写出函数实体，再声明，再在创建软件定时器时把函数和定时器关联上即可。(因为这个回调函数由守护任务调用，已有任务了无需再创建任务)  
1.1区分任务和函数的概念：函数是死的，任务是活的。比喻：函数=一个剧本，任务=不同演员，何时演谁来演都是灵活可变的，但是演的内容不变。（即同一函数可被不同任务一起持有,函数内还可嵌套函数，状态机等）还需注意函数运行在任务的堆栈中，如果函数太大会撑爆。  
2.回调函数保持快进快出。
## 1. 任务流程

1. **配置config.h**
2. **创建软件定时器**
3. **编写回调函数实体(不需要xTaskCreate创建)** 
4. **完成其他任务逻辑** 
5. **启动调度器**

## 2. 关键代码

```c
#define configUSE_TIMERS                         1
#define configTIMER_TASK_PRIORITY                31
#define configTIMER_QUEUE_LENGTH                 10
#define configTIMER_TASK_STACK_DEPTH             256

TimerHandle_t timer0；
pdMS_TO_TICKS();//xx毫秒

TimerHandle_t xTimerCreate( 
    const char * const pcTimerName,   // 1. 名字（调试用）
    const TickType_t xTimerPeriodInTicks, // 2. 周期（Tick数，用 pdMS_TO_TICKS 转换）
    const UBaseType_t uxAutoReload,   // 3. 模式：pdTRUE(周期) / pdFALSE(单次)
    void * const pvTimerID,           // 4. ID号（通常填 NULL 或 0，用于区分不同定时器）
    TimerCallbackFunction_t pxCallbackFunction // 5. 回调函数名
);
timer0=(...);
xTimerStart(TimerHandle, 0);// 参数2：等待时间（如果命令队列满了等多久，通常填 0 不等待）
xTimerStop(TimerHandle, 0);
xTimerChangePeriod(TimerHandle, pdMS_TO_TICKS(500), 0);// 把周期改成 500ms，并立刻启动
BaseType_t xTimerReset( TimerHandle_t  xTimer, const TickType_t xTicksToWait);//复位
BaseType_t xTimerDelete( TimerHandle_t  xTimer, const TickType_t xTicksToWait); //删除