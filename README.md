# FreeRTOS 二值信号量

功能：使用两个按钮控制两个 LED 任务的删除，同时串口打印创建任务的流程。
注意互斥锁创建出来时是满的，二值信号量是空的
## 1. 任务流程

1. #include "semphr.h" 
2. SemaphoreHandle_t myBinarySem_Handle; 
3. myBinarySem_Handle = xSemaphoreCreateBinary();
4. **创建任务** (xTaskCreate)
5. **启动调度器**

## 2. 关键代码

```c
#include "semphr.h" 
SemaphoreHandle_t myBinarySem_Handle; 
myBinarySem_Handle = xSemaphoreCreateBinary();//创建
BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );//释放
BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait );//拿
BaseType_t xSemaphoreGiveFromISR(
                     SemaphoreHandle_t xSemaphore,
                     BaseType_t *pxHigherPriorityTaskWoken
               );