# FreeRTOS 中断(串口版+DMA+队列(收发)) 

功能：接受数据后回传数据，使用队列确保数据安全性，队列里传的是结构体。  

1.区分清任务和函数。并非一定一对一，一个任务的函数里可以调用别的函数。  
2.任务不能运行结束，所以必须得用死循环把他困住。  
3.在 C 语言中，当你用双引号定义一个字符串时，编译器会自动在末尾加上一个 空字符 (Null Character)，也就是 \0 (ASCII 码为 0)，用来标记字符串的结束。所以使用sizeof时记得-1，如uint8_t my_data[] = "Hello...\r\n";   
4.定义完数组长度后，再赋值使用strcpy或memcpy  
中断的分类：  
1.发送接收类  
RXNE（接收数据寄存器非空）  
IDLE（空闲线路检测，非空且对方发完了）  
TXE（发送寄存器空，可以写下一字节了）  
TC (发送完成)  
2.错误类  
ORE (Overrun 溢出)： CPU 读慢了，旧数据没拿走，新数据又来了。这是丢包的主要原因。  
PE (Parity 校验错误)： 奇偶校验不对（通信受干扰）。  
FE (Framing 帧错误)： 没收到停止位（波特率对不上）。  
NE (Noise 噪声错误)： 信号上有毛刺干扰。  
## 1. 任务流程

1. 配置DMA参数(时钟，各属性，连接DMA与外设(USART)，优先级，开启中断)，串口同理  
2. 先写接收中断的逻辑，再写发送函数，再写发送中断的逻辑
3. 
4. 
5. 启动调度器  

## 2. 关键代码

```c
User_DMA_Config();//详见usart.c,dma.c
MX_USART1_UART_Init();

DMA2_Stream2_IRQHandler();
HAL_UARTEx_RxEventCallback();
HAL_UARTEx_RxEventCallback();

HAL_UARTEx_ReceiveToIdle_DMA(&huart1, RxBuffer, 256);//注意缓冲区大小，太小会被截断
__HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);//传输大数据时记得使用，否则会在一半处截断一次
/////////////////////////////////串口+DMA中断的开启与回调函数/////////////////////////////////////////////////
//normal模式用完要再开启，circle模式不用。
//但normal模式存在数据丢失风险（重启的短时间内再来数据就收不到）
//但idle模式下，中断触发是随机的，而circle只有满了指针才会跳到下标为0的地方，所以需额外处理。
HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);//定长接收，满了才回调。（启动的参数都一样）
//串口句柄，放数据的地方，缓冲区大小
HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);//全满回调

HAL_UART_RxHalfCpltCallback();//半满中断一次，全满中断一次
HAL_UART_RxHalfCpltCallback();//半满回调
HAL_UART_RxCpltCallback();//全满回调

HAL_UARTEx_ReceiveToIdle_DMA();//带idle检测的接收，停了也回调
HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size);//idle检测的回调，参数传入的是实际搬运的数量

///////////////////////中断发送
HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);//发送
HAL_UART_TxCpltCallback();//发送回调
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


xTaskGetCurrentTaskHandle()//获取被中断打断的那个任务的句柄
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////信号量相关API//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait );//拿
BaseType_t xSemaphoreGiveFromISR(
                     SemaphoreHandle_t xSemaphore,
                     BaseType_t *pxHigherPriorityTaskWoken
               );//中断中释放信号量，通知任务干活
BaseType_t xSemaphoreTakeFromISR( xSemaphore, pxHigherPriorityTaskWoken )
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////队列相关API////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
BaseType_t xQueueSendFromISR( 
    QueueHandle_t xQueue,                   // 队列句柄
    const void *pvItemToQueue,              // 要发送的数据的【地址/指针】
    BaseType_t *pxHigherPriorityTaskWoken   // [回传] 是否需要切换任务？
);//发送到队尾
BaseType_t xQueueSendToFrontFromISR( 
    QueueHandle_t xQueue,                   // 队列句柄
    const void *pvItemToQueue,              // 要发送的数据的【地址/指针】
    BaseType_t *pxHigherPriorityTaskWoken   // [回传] 是否需要切换任务？
);//发送到队头
xQueueOverwriteFromISR(
    QueueHandle_t xQueue, 
    const void *pvItemToQueue, 
    BaseType_t *pxHigherPriorityTaskWoken
);//覆写
BaseType_t xQueueReceiveFromISR( 
    QueueHandle_t xQueue,                   // 队列句柄
    void *pvBuffer,                         // 接收数据的缓冲区【地址】
    BaseType_t *pxHigherPriorityTaskWoken   // [回传] 是否需要切换任务？
);//接收
uxQueueMessagesWaitingFromISR(QueueHandle)//返回队列里现在有几个数据（或者信号量现在的计数值）
xQueueIsQueueFullFromISR(QueueHandle)
xQueueIsQueueEmptyFromISR(QueueHandle)
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////事件组///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
BaseType_t xEventGroupSetBitsFromISR( 
    EventGroupHandle_t xEventGroup,         // 事件组句柄
    const EventBits_t uxBitsToSet,          // 要置为 1 的位 (如 1<<0)
    BaseType_t *pxHigherPriorityTaskWoken   // [回传] 是否需要切换任务？
);//设置位
BaseType_t xEventGroupClearBitsFromISR( 
    EventGroupHandle_t xEventGroup,         // 事件组句柄
    const EventBits_t uxBitsToClear         // 要清零的位
); //清空位
EventBits_t xEventGroupGetBitsFromISR( EventGroupHandle_t xEventGroup )//读取位
// 注意：ClearBitsFromISR 通常不需要切换任务参数，因为它不唤醒任务。
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////任务通知///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void vTaskNotifyGiveFromISR( 
    TaskHandle_t xTaskToNotify,             // 目标任务句柄
    BaseType_t *pxHigherPriorityTaskWoken   // [回传] 是否需要切换任务？
);//发送简单通知
BaseType_t xTaskNotifyFromISR( 
    TaskHandle_t xTaskToNotify,             // 目标任务句柄
    uint32_t ulValue,                       // 要传的数据或掩码
    eNotifyAction eAction,                  // 动作 (eSetBits, eIncrement 等)
    BaseType_t *pxHigherPriorityTaskWoken   // [回传] 是否需要切换任务？
);//发送全能通知
xTaskNotifyAndQueryFromISR() //发送通知并获取发送任务通知前任务通知的通知值
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////软件定时器//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
BaseType_t xTimerStartFromISR( 
    TimerHandle_t xTimer,                   // 定时器句柄
    BaseType_t *pxHigherPriorityTaskWoken   // [回传] 是否需要切换任务？
);
BaseType_t xTimerStopFromISR( 
    TimerHandle_t xTimer,                   // 定时器句柄
    BaseType_t *pxHigherPriorityTaskWoken   // [回传] 是否需要切换任务？
);
xTimerResetFromISR() //在中断中复位软件定时器定时 
xTimerChangePeriodFromISR() //在中断中更改软件定时器的定时超时时间
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
BaseType_t *pxHigherPriorityTaskWoken = pdFALSE;//如果该函数唤醒了一个优先级比当前运行任务更高的任务，API会把这个变量置为 pdTRUE
portYIELD_FROM_ISR(xHigherPriorityTaskWoken)//放在回调函数的最后一行，用于中断结束时切换到高优先级任务



if (recv_pkg.len < RX_BUF_SIZE)
        recv_pkg.payload[recv_pkg.len] = '\0';//使用printf的时候记得在最后多加个/0，不然他自己不会停