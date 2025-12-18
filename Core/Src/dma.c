#include <stdio.h>
#include <dma.h>
#include <usart.h>
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef  hdma_usart1_tx;
void User_DMA_Config(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE();    // DMA控制器 (注意：F4的串口1/6在DMA2，其他在DMA1)

    

    hdma_usart1_rx.Instance = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);

    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    hdma_usart1_tx.Instance = DMA2_Stream7;             
    hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;        
    // 注意方向反了：是从内存(数组)搬到外设(串口)
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH; 
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;   // 外设地址固定
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;       // 内存地址递增
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;              // 正常模式(发完停)
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);

    HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

