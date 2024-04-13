/**
  ******************************************************************************
  * @file    uart_gate_hal.c
  * @version V1.0.0
  * @date    11-04-2024
  * @brief   
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2024 </center></h2>
  ******************************************************************************
  */
#include "main.h"
#include "uart_gate_hal.h"
#include "stm32h7xx_hal_uart.h"
#include "stm32h7xx_ll_usart.h"

void uartGateRxData(uint8_t data);
void uartGateTxCpl(void);

/* UART handler declaration */
UART_HandleTypeDef UartHandle;

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA
  *         used for USART data transmission
  */
void USART6_IRQHandler(void)
{
  if ((LL_USART_IsActiveFlag_RXNE(USART6)) & (LL_USART_IsEnabledIT_RXNE(USART6)))
  {
	uartGateRxData(LL_USART_ReceiveData8(USART6));
  }
  HAL_UART_IRQHandler(&UartHandle);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to DMA
  *         used for USART data reception
  */
void DMA2_Stream7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UartHandle.hdmatx);
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{

}

/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  static DMA_HandleTypeDef hdma_tx;
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable GPIO TX/RX clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /* Enable USARTx clock */
  __HAL_RCC_USART6_CLK_ENABLE();

  /* Enable DMA clock */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_6;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART6;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART6;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = DMA2_Stream7;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  hdma_tx.Init.Request             = DMA_REQUEST_USART6_TX;

  HAL_DMA_Init(&hdma_tx);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, hdma_tx);

  /* NVIC configuration for DMA transfer complete interrupt (USART6_TX) */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 1);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

  LL_USART_EnableIT_RXNE(USART6);

  /* NVIC for USART, to catch the TX complete */
  HAL_NVIC_SetPriority(USART6_IRQn, 5, 1);
  HAL_NVIC_EnableIRQ(USART6_IRQn);
}

/**
  * @brief
  *
  * @param  None
  * @retval None
  */
void uartGateTxData(const uint8_t *pData, uint16_t Size)
{
      /* start the transmission process */
	  if(HAL_UART_Transmit_DMA(&UartHandle, pData, Size)!= HAL_OK)
	  {
	    Error_Handler();
	  }
}

/**
  * @brief configure the UART peripheral
  *
  * @param  None
  * @retval None
  */
void uartGateHalInit(void)
{
	 /*  Configure the UART peripheral */
	  UartHandle.Instance        = USART6;

	  UartHandle.Init.BaudRate   = 115200;
	  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	  UartHandle.Init.StopBits   = UART_STOPBITS_1;
	  UartHandle.Init.Parity     = UART_PARITY_NONE;
	  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	  UartHandle.Init.Mode       = UART_MODE_TX_RX;
	  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	  if(HAL_UART_DeInit(&UartHandle) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if(HAL_UART_Init(&UartHandle) != HAL_OK)
	  {
	    Error_Handler();
	  }

}
/******************* (C) COPYRIGHT 2024 *****END OF FILE****/
