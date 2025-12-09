/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "semphr.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile int flag[2] = {1, 1};
TaskHandle_t task_low; // 原生句柄
TaskHandle_t task_medium;
TaskHandle_t task_high;

SemaphoreHandle_t myBinarySem_Handle;
SemaphoreHandle_t myCountingSem;

void task_low_Entry(void *pvParameters); // 函数声明
void task_medium_Entry(void *pvParameters);
void task_high_Entry(void *pvParameters);


/* USER CODE END Variables */
osThreadId StartTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  xTaskCreate((TaskFunction_t)task_low_Entry,
              (const char *)"LED0",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)5, // 假设优先级为 2
              (TaskHandle_t *)&task_low);

  xTaskCreate((TaskFunction_t)task_medium_Entry,
              (const char *)"LED1",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)10,
              (TaskHandle_t *)&task_medium);

  xTaskCreate((TaskFunction_t)task_high_Entry,
              (const char *)"KEY0",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)15,
              (TaskHandle_t *)&task_high);

  

  myBinarySem_Handle = xSemaphoreCreateBinary();
  if (myBinarySem_Handle == NULL)
  {
    printf("Create Error!\r\n");
  }
  xSemaphoreGive(myBinarySem_Handle);
  myCountingSem = xSemaphoreCreateCounting(5,0);
  if (myCountingSem == NULL)
  {
    printf("Create Error!\r\n");
  }
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of StartTask */
  osThreadDef(StartTask, StartDefaultTask, osPriorityNormal, 0, 128);
  StartTaskHandle = osThreadCreate(osThread(StartTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the StartTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
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
/* USER CODE END Application */
