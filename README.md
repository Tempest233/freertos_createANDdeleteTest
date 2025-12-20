# FreeRTOS 中断(串口版+DMA+队列(收发)+性能监控) 

功能：定期发送系统状态。  


## 1. 任务流程

1. 修改config.h  
2. 编写定时器驱动代码timer.c.h
3. 编写中断服务函数 (ISR)
4. 编写监控任务函数
5. 启动调度器  

## 2. 关键代码

```c

#define configGENERATE_RUN_TIME_STATS        1   
#define configUSE_TRACE_FACILITY             1  // 这个通常也需要开启  
#define configUSE_STATS_FORMATTING_FUNCTIONS 1  // 开启格式化输出函数 (vTaskGetRunTimeStats)  
extern void ConfigureTimerForRunTimeStats(void);    
extern    unsigned long GetRunTimeCounterValue(void);    
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()   ConfigureTimerForRunTimeStats()    
#define portGET_RUN_TIME_COUNTER_VALUE()           GetRunTimeCounterValue()  

//timer.c
#include "stm32f4xx_hal.h"
/* 1. 定义全局计数器变量 */
volatile unsigned long ulHighFrequencyTimerTicks = 0;
/* 2. 定义 TIM3 句柄 */
TIM_HandleTypeDef htim3_stats;
/* 3. 实现配置函数 (对应 portCONFIGURE_TIMER_FOR_RUN_TIME_STATS) */
void ConfigureTimerForRunTimeStats(void)
{
    ulHighFrequencyTimerTicks = 0;
    // --- A. 开启时钟 ---
    __HAL_RCC_TIM3_CLK_ENABLE();
    // --- B. 配置参数 ---
    // F407 的 TIM3 挂在 APB1，时钟通常是 84MHz
    // 目标：频率 20kHz (50us)
    htim3_stats.Instance = TIM3;
    // 预分频：84MHz / 84 = 1MHz (1us 一个脉冲)
    htim3_stats.Init.Prescaler = 84 - 1;
    // 计数周期：数 50 个脉冲 = 50us
    htim3_stats.Init.Period = 50 - 1;
    htim3_stats.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3_stats.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    // 初始化硬件
    HAL_TIM_Base_Init(&htim3_stats);
    // --- C. 配置中断 (NVIC) ---
    // 优先级设低一点 (比如 6 或 7)，不要抢占正常的业务中断
    HAL_NVIC_SetPriority(TIM3_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
    // --- D. 启动定时器中断 ---
    HAL_TIM_Base_Start_IT(&htim3_stats);
}
/* 4. 实现获取时间函数 (对应 portGET_RUN_TIME_COUNTER_VALUE) */
unsigned long GetRunTimeCounterValue(void)
{
    return ulHighFrequencyTimerTicks;
}

//it.c
void TIM3_IRQHandler(void)
{
    // 检查是否是更新中断 (Update Event)
    if (__HAL_TIM_GET_FLAG(&htim3_stats, TIM_FLAG_UPDATE) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&htim3_stats, TIM_IT_UPDATE) != RESET)
        {
            __HAL_TIM_CLEAR_IT(&htim3_stats, TIM_IT_UPDATE);
            
            // 核心动作：计数器 + 1
            ulHighFrequencyTimerTicks++;
        }
    }
}

//freertos.c
void Monitor_Task(void *param)
{
    // 申请 buffer (每个任务大约占用 40 字节，根据任务数估算)
    char *pcWriteBuffer = pvPortMalloc(512);
    for(;;)
    {
        vTaskDelay(5000); // 5秒打印一次
        if (pcWriteBuffer != NULL)
        {
            // 获取统计信息字符串
            vTaskGetRunTimeStats(pcWriteBuffer);
            
            // 打印
            printf("==================================\r\n");
            printf("Task          Abs Time      %% Time\r\n");
            printf("----------------------------------\r\n");
            printf("%s", pcWriteBuffer);
            printf("==================================\r\n");
        }
    }
}