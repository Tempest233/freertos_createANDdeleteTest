# FreeRTOS 队列的创建，读取和写入

这是一个 STM32 FreeRTOS 练习项目。
功能：写入队列的时打印，读取队列时也打印。
队列锁：在任务操作队列期间，允许中断正常‘读写数据’，但将中断触发的‘任务唤醒（调度）’动作，推迟到锁解开之后统一执行
## 1. 任务流程

1. **定义任务句柄** (身份证)
2. **声明任务函数**
3. **编写任务函数实体** 
4. **创建任务** (xTaskCreate)
5. **启动调度器**

## 2. 关键代码

```c
#include "queue.h"
QueueHandle_t xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize );//创建队列
MyNativeQueue_Handle = xQueueCreate(5, sizeof(int));
BaseType_t xQueueSend( QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait );//发送进入队列
BaseType_t xQueueReceive( QueueHandle_t xQueue, void * pvBuffer, TickType_t xTicksToWait );//接收队列数据

void Queue_write_Entry(void *pvParameters)
{
  int send_data=100;
  for (;;)
  {
    printf("send data: %d  \n",send_data);
    if(xQueueSend(Queue_count,&send_data,0) == pdPASS)
   // printf("send data: %d  \n",send_data);
		send_data++;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
void Queue_read_Entry(void *pvParameters)
{
  int recv_buffer=0;
  for(;;)
  {
    if(xQueueReceive(Queue_count,&recv_buffer,portMAX_DELAY) == pdTRUE )
    {
      printf("receive data: %d\r\n",recv_buffer);
      HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    }
  }

}