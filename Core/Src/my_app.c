#include "my_app.h"
#include "main.h"
#include "FreeRTOS.h" 
#include "task.h"  
#include "queue.h"     
#include "semphr.h" 
#include <stdio.h> 
extern SemaphoreHandle_t myCountingSem;
extern SemaphoreHandle_t myBinarySem_Handle;
extern SemaphoreHandle_t mymutex;

void task_low_Entry(void *pvParameters)
{
  
  for (;;)
  {
  if(xSemaphoreTake(mymutex, portMAX_DELAY) == pdTRUE)
  {
    printf("[Low] : Got Semaphore! Working...\r\n");
    for(int i=0; i<10; i++)
        {
            printf("[Low] : Working %d/10... (My Prio is %d)\r\n", i+1,(int)uxTaskPriorityGet(NULL));
            HAL_Delay(500); 
        }
    printf("[Low] : Give Semaphore.(My Prio is %d)\r\n",(int)uxTaskPriorityGet(NULL));
        xSemaphoreGive(mymutex);
  }
  vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
void task_medium_Entry(void *pvParameters)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  for (;;)
  {
    printf("[Middle]: I am Running! (Blocking Low)\r\n");
      HAL_Delay(2000);
   printf("[Middle]: Job Done. Sleep.\r\n");
    
    // 睡很久，方便观察下一轮
    vTaskDelay(pdMS_TO_TICKS(10000)); 
  }
}

void task_high_Entry(void *pvParameters)
{
  vTaskDelay(pdMS_TO_TICKS(2000));
  for (;;)
  {
     printf("[High] : I want Semaphore!\r\n");
     xSemaphoreTake(mymutex, portMAX_DELAY);
    printf("[High] : Finally Got it!!!\r\n");
    xSemaphoreGive(mymutex);
    
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
