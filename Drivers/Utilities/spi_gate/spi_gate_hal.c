/**
  ******************************************************************************
  * @file    spi_gate_hal.c
  * @version V1.0.0
  * @date    12-04-2024
  * @brief   
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2024 </center></h2>
  ******************************************************************************
  */
#include "main.h"
#include "spi_gate_hal.h"
#include "stm32h7xx_hal_spi.h"
#include "pin_dbg.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* SPI handler declaration */

SPI_HandleTypeDef SpiHandle;
#define BUFF_SIZE 50
uint8_t spiTxBuf[BUFF_SIZE] =  { 0,0xFF,0xFF,0xAA,0x55,1,2,3,4,5,  6,7,8,9,0,1,2,3,4,5,  6,7,8,9,0,1,2,3,4,5,  6,7,8,9,0,1,2,3,4,5, 6,7,8,9,0,0x55,0xAA,0xFF,0xFF,0};

uint8_t spiRxBuf[BUFF_SIZE] = {0};

static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function handles DMA Rx interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(SpiHandle.hdmarx);
}

/**
  * @brief  This function handles DMA Tx interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(SpiHandle.hdmatx);
}

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  SPI DMA transfer complete callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
 T2_LO;T1_HI;
}

/**
  * @brief  SPI DMA Half transfer complete callback
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
 T2_HI;T1_LO;
}

/**
  * @brief SPI MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param hspi: SPI handle pointer
  * @retval None
  */

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

    /* Enable GPIO TX/RX clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* Enable SPI1 clock */
    __HAL_RCC_SPI1_CLK_ENABLE();
    /* Enable DMA clock */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_5;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure the DMA handler for Transmission process */
    hdma_tx.Instance                 = DMA2_Stream3;
    hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
    hdma_tx.Init.Request             = DMA_REQUEST_SPI1_TX;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_tx.Init.Mode                = DMA_CIRCULAR;
    hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&hdma_tx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmatx, hdma_tx);

    /* Configure the DMA handler for Transmission process */
    hdma_rx.Instance                 = DMA2_Stream2;

    hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
    hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;
    hdma_rx.Init.Request             = DMA_REQUEST_SPI1_RX;
    hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx.Init.Mode                = DMA_CIRCULAR;
    hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;


    HAL_DMA_Init(&hdma_rx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(hspi, hdmarx, hdma_rx);

    /* NVIC configuration for DMA transfer complete interrupt (SPI1_TX) */
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 1);
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt (SPI1_RX) */
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

    /* NVIC configuration for SPI transfer complete interrupt (SPI1) */
    //HAL_NVIC_SetPriority(SPI1_IRQn, 5, 0);
    //HAL_NVIC_EnableIRQ(SPI1_IRQn);
}

/**
  * @brief configure the UART peripheral
  *
  * @param  None
  * @retval None
  */
void spiGateHalInit(void)
{
	 /*  Configure the SPI peripheral */
	  /* Set the SPI parameters */
	  SpiHandle.Instance               = SPI1;
	  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
	  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	  SpiHandle.Init.CRCPolynomial     = 7;
	  SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
	  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
	  SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
	  SpiHandle.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
	  SpiHandle.Init.Mode              = SPI_MODE_MASTER;

	  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
	  {
	    /* Initialization Error */
	    Error_Handler();
	  }

	  /* While the SPI */
	  if(HAL_SPI_TransmitReceive_DMA(&SpiHandle, spiTxBuf, spiRxBuf, BUFF_SIZE) != HAL_OK)
	  {
	    /* Transfer error in transmission process */
	    Error_Handler();
	  }
}
/******************* (C) COPYRIGHT 2024 *****END OF FILE****/
