#include "my_app.h"
#include "main.h"
#include "FreeRTOS.h" 
#include "task.h"  
#include "queue.h"     
#include "semphr.h" 
#include <stdio.h> 
extern QueueHandle_t myCountingSem;
extern SemaphoreHandle_t myBinarySem_Handle;

void task_low_Entry(void *pvParameters)
{
  
  for (;;)
  {
  if(xSemaphoreTake(myBinarySem_Handle, portMAX_DELAY) == pdTRUE)
  {
    printf("[Low] : Got Semaphore! Working...\r\n");
    for(int i=0; i<3; i++)
        {
            printf("[Low] : Working %d/3...\r\n", i+1);
            HAL_Delay(1000); 
        }
    printf("[Low] : Give Semaphore.\r\n");
        xSemaphoreGive(myBinarySem_Handle);
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
void task_medium_Entry(void *pvParameters)
{
  vTaskDelay(pdMS_TO_TICKS(1000));
  for (;;)
  {
    printf("[Middle]: I am Running! (Blocking Low)\r\n");
      HAL_Delay(5000);
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
     xSemaphoreTake(myBinarySem_Handle, portMAX_DELAY);
    printf("[High] : Finally Got it!!!\r\n");
    xSemaphoreGive(myBinarySem_Handle);
    
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
