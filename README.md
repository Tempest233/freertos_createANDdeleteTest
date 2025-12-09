# FreeRTOS 计数型信号量

功能：设计两个任务，一个每一秒停一辆车进入，一个三秒移出一辆车。

注意互斥锁创建出来时是满的，二值信号量是空的
注意只有写在for循环里的，才会每次切回任务都执行，因为切换任务等于是暂停
注意给信号量不会阻塞，满了会直接丢掉，返回错误
## 1. 任务流程

1. #include "semphr.h" 
2. SemaphoreHandle_t myCountingSem;
3. myCountingSem = xSemaphoreCreateCounting(最大值, 初始值); 
4. **创建任务** (xTaskCreate)
5. **启动调度器**

## 2. 关键代码

```c
#include "semphr.h" 
SemaphoreHandle_t myCountingSem;
myCountingSem = xSemaphoreCreateCounting(最大值, 初始值); //创建
BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );//释放,+1,不阻塞
BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait );//拿，-1，阻塞
BaseType_t xSemaphoreGiveFromISR(
                     SemaphoreHandle_t xSemaphore,
                     BaseType_t *pxHigherPriorityTaskWoken
               );
UBaseType_t uxSemaphoreGetCount( SemaphoreHandle_t xSemaphore );//读取计数信号量的当前值