#include "stm32f4xx_hal.h"

volatile unsigned long ulHighFrequencyTimerTicks = 0;
TIM_HandleTypeDef htim3_stats;

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
    HAL_NVIC_SetPriority(TIM3_IRQn, 9, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    // --- D. 启动定时器中断 ---
    HAL_TIM_Base_Start_IT(&htim3_stats);
}
unsigned long GetRunTimeCounterValue(void)
{
    return ulHighFrequencyTimerTicks;
}