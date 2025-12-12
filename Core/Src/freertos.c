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
TaskHandle_t LED0_Handle; // 原生句柄
TaskHandle_t LED1_Handle;
TaskHandle_t KEY0_Handle;
TaskHandle_t KEY1_Handle;
TaskHandle_t USART_Handle;
SemaphoreHandle_t myBinarySem_Handle;
SemaphoreHandle_t myCountingSem;

void LED0_Entry(void *pvParameters); // 函数声明
void LED1_Entry(void *pvParameters);
void KEY0_Entry(void *pvParameters);
void KEY1_Entry(void *pvParameters);
void USART_Entry(void *pvParameters);

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

  xTaskCreate((TaskFunction_t)LED0_Entry,
              (const char *)"LED0",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)2, // 假设优先级为 2
              (TaskHandle_t *)&LED0_Handle);

  xTaskCreate((TaskFunction_t)LED1_Entry,
              (const char *)"LED1",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)2,
              (TaskHandle_t *)&LED1_Handle);

  xTaskCreate((TaskFunction_t)KEY0_Entry,
              (const char *)"KEY0",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)2,
              (TaskHandle_t *)&KEY0_Handle);

  xTaskCreate((TaskFunction_t)KEY1_Entry,
              (const char *)"KEY1",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)2,
              (TaskHandle_t *)&KEY1_Handle);

  xTaskCreate((TaskFunction_t)USART_Entry,
              (const char *)"Usart",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)2,
              (TaskHandle_t *)&USART_Handle);

  myBinarySem_Handle = xSemaphoreCreateBinary();
  if (myBinarySem_Handle == NULL)
  {
    printf("Create Error!\r\n");
  }
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
void LED0_Entry(void *pvParameters)
{
  
  for (;;)
  {
    xSemaphoreTake(myBinarySem_Handle,portMAX_DELAY);
    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    printf("led toggle!\r\n");
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
void LED1_Entry(void *pvParameters)
{
  int car_number=0;
  for (;;)
  {
    car_number=uxSemaphoreGetCount(myCountingSem);
    printf("now there are %d cars in this area.trying to park...\r\n",car_number);
    if (xSemaphoreGive(myCountingSem) == pdTRUE)
    {
    printf("parking success!\r\n");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void KEY0_Entry(void *pvParameters)
{
  
  for (;;)
  {
    //        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET)
    {
      vTaskDelay(pdMS_TO_TICKS(20)); // 简单的消抖
      if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET)
      {
        // 按下 KEY0：点亮 LED0 (置低电平)，发送串口消息
        // 串口发送
        printf("KEY0 Pressed! \r\n");
        xSemaphoreGive(myBinarySem_Handle);

        while (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET)
        {
          // 这里什么都不用写，就是为了卡住 CPU
        }
        vTaskDelay(pdMS_TO_TICKS(20));
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
void KEY1_Entry(void *pvParameters)
{
   
  for (;;)
  {
    vTaskDelay(pdMS_TO_TICKS(3000));
    xSemaphoreTake(myCountingSem,portMAX_DELAY);
    printf("car out!\r\n");
    
  }
}

void USART_Entry(void *pvParameters)
{
   vTaskSuspend(NULL);
  for (;;)
  {
    if (flag[0] == 0)
    {
      vTaskDelete(LED0_Handle);
      printf("delete task0 successfully\r\n");
      flag[0] = 1;
    }
    else if (flag[1] == 0)
    {
      vTaskDelete(LED1_Handle);
      printf("delete task1 successfully\r\n");
      flag[1] = 1;
    }
  }
}
/* USER CODE END Application */
