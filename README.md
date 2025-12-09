# FreeRTOS 互斥锁

功能：设置低中高三个优先级的任务，低优先级用互斥锁运行一半，中优先级来打断他，高优先级再来拿锁发生优先级继承打断中优先级，然后低优先级高优先级都运行完才到中优先级。

注意互斥锁创建出来时是满的，二值信号量是空的。  
只有写在for循环里的，才会每次切回任务都执行，因为切换任务等于是暂停  
给信号量不会阻塞，满了会直接丢掉，返回错误 
 
互斥锁：  
1.解铃还须系铃人  
2.无法在中断里使用  
3.不能连锁两次及以上  
4.只有高优先级任务拿锁的那一刻，才会发生优先级继承，打断中优先级
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
myCountingSem = xSemaphoreCreateMutex();//创建
xSemaphoreGive(Mutex_Handle);//释放,+1,不阻塞 
xSemaphoreTake(Mutex_Handle, timeout);;//拿，-1，阻塞 死等：portMAX_DELAY
BaseType_t xSemaphoreGiveFromISR(
                     SemaphoreHandle_t xSemaphore,
                     BaseType_t *pxHigherPriorityTaskWoken
               );