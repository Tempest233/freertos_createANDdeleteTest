# FreeRTOS 队列的创建，读取和写入

这是一个 STM32 FreeRTOS 练习项目。
功能：写入队列的时打印，读取队列时也打印。

## 1. 任务流程

1. **定义任务句柄** (身份证)
2. **声明任务函数**
3. **编写任务函数实体** (任务 A: 负责闪灯)
4. **创建任务** (xTaskCreate)
5. **启动调度器**

## 2. 关键代码

```c
#include "queue.h"
QueueHandle_t xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize );//创建队列
MyNativeQueue_Handle = xQueueCreate(5, sizeof(int));
BaseType_t xQueueSend( QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait );//发送进入队列
BaseType_t xQueueReceive( QueueHandle_t xQueue, void * pvBuffer, TickType_t xTicksToWait );//接收队列数据

