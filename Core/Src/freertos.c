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
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint16_t len;         // 长度
  uint8_t payload[512]; // 数据本体
} UartPacket_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RX_BUF_SIZE 512
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile int flag[2] = {1, 1};
uint8_t send_data[256];
uint8_t rx_byte;
uint8_t RxBuffer[RX_BUF_SIZE];

TaskHandle_t LED0_Handle; // 原生句柄
TaskHandle_t LED1_Handle;
TaskHandle_t KEY0_Handle;
TaskHandle_t KEY1_Handle;
TaskHandle_t USART_Handle;
TaskHandle_t USART_Send_Handle;

SemaphoreHandle_t myBinarySem_Handle;
SemaphoreHandle_t myQueue;
SemaphoreHandle_t myMutex;
SemaphoreHandle_t mybufferQueue;
SemaphoreHandle_t Transmit_flag;

void LED0_Entry(void *pvParameters); // 函数声明
void LED1_Entry(void *pvParameters);
void KEY0_Entry(void *pvParameters);
void KEY1_Entry(void *pvParameters);
void USART_Send_Entry(void *pvParameters);
void USART_Receive_Entry(void *pvParameters);
void DMA_Send_Entry(uint8_t *data,uint16_t len);

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

  xTaskCreate((TaskFunction_t)USART_Receive_Entry,
              (const char *)"Usart",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)2,
              (TaskHandle_t *)&USART_Handle);

  xTaskCreate((TaskFunction_t)USART_Send_Entry,
              (const char *)"Usart",
              (uint16_t)128,
              (void *)NULL,
              (UBaseType_t)2,
              (TaskHandle_t *)&USART_Send_Handle);

 

  myBinarySem_Handle = xSemaphoreCreateBinary();
  myQueue = xQueueCreate(1, sizeof(UartPacket_t));
  mybufferQueue = xQueueCreate(128, sizeof(uint16_t));
  myMutex = xSemaphoreCreateMutex();
  Transmit_flag = xSemaphoreCreateBinary();
  xSemaphoreGive(Transmit_flag);
  if (myBinarySem_Handle == NULL)
  {
    printf("Create Error!\r\n");
  }
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxBuffer, RX_BUF_SIZE);
  __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
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
    // 1. 死等信号量（被中断唤醒）
    if (xSemaphoreTake(myBinarySem_Handle, portMAX_DELAY) == pdTRUE)
    {
      // 2. 醒来后，先延时 20ms (避开抖动期)
      vTaskDelay(pdMS_TO_TICKS(20));

      // 3. 再次确认引脚电平 (确认真的按下了，而不是误触)
      // 注意：KEY1 (PE3) 是低电平有效
      if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == GPIO_PIN_RESET)
      {
        // 真的按下了，执行业务
        printf("LED Toggled!\r\n");
        HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);

        // 4. 【重要】加一个长延时，防止连击
        // 比如 200ms 内不准再按
        vTaskDelay(pdMS_TO_TICKS(200));

        // 5. 【进阶技巧】清空信号量
        // 如果在延时期间中断又给了一次信号量，这里可以把它清掉，防止循环回去又执行一次
        xQueueReset((QueueHandle_t)myBinarySem_Handle);
      }
    }
  }
}
void LED1_Entry(void *pvParameters)
{
  vTaskSuspend(NULL);
  for (;;)
  {
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void KEY0_Entry(void *pvParameters)
{
  vTaskSuspend(NULL);
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
  vTaskSuspend(NULL);
  for (;;)
  {
    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3) == GPIO_PIN_RESET)
    {
      HAL_Delay(20); // 简单的消抖
      if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3) == GPIO_PIN_RESET)
      {
        // 按下 KEY0：点亮 LED0 (置低电平)，发送串口消息
        // 串口发送
        printf("KEY1 Pressed! \r\n");
        flag[1] = 0;
        while (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3) == GPIO_PIN_RESET)
        {
          // 这里什么都不用写，就是为了卡住 CPU
        }
        vTaskDelay(pdMS_TO_TICKS(20));
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void USART_Receive_Entry(void *pvParameters)
{
  UartPacket_t recv_pkg;
  for (;;)
  {
    if (xQueueReceive(myQueue, &recv_pkg, portMAX_DELAY) == pdTRUE)
    {
      xSemaphoreTake(myMutex, portMAX_DELAY);
      printf("ernter entry\r\n");
      xSemaphoreGive(myMutex);

      if (recv_pkg.len < RX_BUF_SIZE)
        recv_pkg.payload[recv_pkg.len] = '\0';//必须加，否则printf会停不下来

      xSemaphoreTake(myMutex, portMAX_DELAY);
      printf("DMA Recv (%d bytes): %s\r\n", recv_pkg.len, recv_pkg.payload);
      xSemaphoreGive(myMutex);
    }
  }
}
void DMA_Send_Entry(uint8_t *data,uint16_t len)
{

    if(xSemaphoreTake(Transmit_flag,portMAX_DELAY) == pdTRUE)
    {
      HAL_UART_Transmit_DMA(&huart1, data, len);
    }
}

void USART_Send_Entry(void *pvParameters)
{
  char *str = "hello,world!\r\n";
  uint16_t len = strlen(str); 
  memcpy(send_data,str,len);
  for(;;)
  {
    
    DMA_Send_Entry(send_data,len);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

  if (GPIO_Pin == GPIO_PIN_4)
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(myBinarySem_Handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

  if (huart->Instance == USART1)
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    UartPacket_t msg;
    msg.len = Size;
    memcpy(msg.payload, RxBuffer, Size);
    xQueueSendFromISR(myQueue, &msg, &xHigherPriorityTaskWoken);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxBuffer, RX_BUF_SIZE);
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      xSemaphoreGiveFromISR(Transmit_flag, &xHigherPriorityTaskWoken);
        
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
/* USER CODE END Application */
