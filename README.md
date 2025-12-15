# FreeRTOS 临界区
1.不准进行耗时操作  
2.不准阻塞  
3.可以嵌套，但是避免嵌套太多次  
4.会损害系统的实时性  
## 1. 任务流程

1. 
2. 
3. 
4. 创建任务 (xTaskCreate)  
5. 启动调度器  

## 2. 关键代码

```c
taskENTER_CRITICAL(); 
taskEXIT_CRITICAL();

uint32_t saved_interrupt_status = taskENTER_CRITICAL_FROM_ISR();//返回之前的屏蔽等级
taskEXIT_CRITICAL_FROM_ISR(saved_interrupt_status);