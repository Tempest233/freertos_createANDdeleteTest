# FreeRTOS 中断(二值信号量版) 

功能：中断触发按键，翻转LED。  
注意互斥锁创建出来时是满的，二值信号量是空的.
## 1. 任务流程

1. 配置GPIO为中断触发模式  
2. 配置中断优先级，使能  
3. 编写函数：中断服务入口 and 回调函数  
4. 创建任务 (xTaskCreate)  
5. 启动调度器  

## 2. 关键代码

```c
HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
HAL_NVIC_EnableIRQ(EXTI4_IRQn);
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){}；
SemaphoreHandle_t myBinarySem_Handle; 
myBinarySem_Handle = xSemaphoreCreateBinary();//创建信号量

BaseType_t *pxHigherPriorityTaskWoken = pdFALSE;//如果该函数唤醒了一个优先级比当前运行任务更高的任务，API会把这个变量置为 pdTRUE
portYIELD_FROM_ISR(xHigherPriorityTaskWoken)//放在回调函数的最后一行，用于中断结束时切换到高优先级任务

///////////////////////////////信号量相关API//////////////////////////////////////
BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait );//拿
BaseType_t xSemaphoreGiveFromISR(
                     SemaphoreHandle_t xSemaphore,
                     BaseType_t *pxHigherPriorityTaskWoken
               );//中断中释放信号量，通知任务干活
BaseType_t xSemaphoreTakeFromISR( xSemaphore, pxHigherPriorityTaskWoken )
///////////////////////////////队列相关API////////////////////////////////////////////
xQueueSendFromISR
xQueueSendToFrontFromISR
xQueueOverwriteFromISR
xQueueReceiveFromISR