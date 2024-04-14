/**
  ******************************************************************************
  * @file    spi_gate_cntrl.c
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
#include "spi_gate_cntrl.h"
#include "spi_gate_hal.h"

TaskHandle_t  spiCntrlHandle;

/**
  * @brief  enumeration notification spi gate
  */
typedef enum
{
  NOTE_TRANSFER_CPLT      = 0x00000001,
  NOTE_TRANSFER_HALF_CPLT = 0x00000002,
}note_spi_e;

#define MAX_SIZE_QUEUE (10)
QueueHandle_t QueueUartToSpi;
QueueHandle_t QueueSpiToUart;


uint8_t spiTxBuf[2][BUFF_SIZE] = {0};
uint8_t spiRxBuf[2][BUFF_SIZE] = {0};

/**
 * @brief sending notification spi gate
 * @param  None
 * @retval None
 */
void spiGateNote(note_spi_e noteValue)
{
	  if ( spiCntrlHandle != NULL )
	  {
		BaseType_t IrqHighPrTskWoken = pdFALSE;
	    xTaskNotifyFromISR( spiCntrlHandle,
	    		           noteValue,
	                       eSetBits,
	                       &IrqHighPrTskWoken );
	    portYIELD_FROM_ISR( IrqHighPrTskWoken );
	  }
}

/**
  * @brief  SPI DMA transfer complete callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	spiGateNote(NOTE_TRANSFER_CPLT);
}

/**
  * @brief  SPI DMA Half transfer complete callback
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
	spiGateNote(NOTE_TRANSFER_HALF_CPLT);
}

/**
  * @brief  filling the SPI_DMA buffer for transmission
  * @param  uint8_t* pData -  pointer to buffer
  * @retval None
  */
void filingTransmitBufSpiDMA(uint8_t* pData)
{
	if (QueueUartToSpi != NULL)
	{
		if (xQueueReceive(QueueUartToSpi, pData, 0) == pdTRUE)
		{
			return;
		}
	}

	for (uint32_t cntic = 0; cntic < BUFF_SIZE; cntic++)
	{
		pData[cntic] = 0;
	}
}




/**
  * @brief  reading receive buffer SPI_DMA
  * @param  uint8_t* pData -  pointer to buffer
  * @retval None
  */
void readingReceiveBufSpiDMA(uint8_t* pData)
{
	bool flagDataNotEmpty = false;
	bool flagReqZero = false;

	  for(uint32_t cntic = 0; cntic < BUFF_SIZE; cntic++ )
	  {
		  if ( pData[cntic] != 0 )
		  {
			  flagDataNotEmpty = true;
			  break;
		  }
	  }

	  if ( (flagDataNotEmpty) || (flagReqZero ) )
	  {
		  if (QueueUartToSpi != NULL)
		  {
			  xQueueSend(QueueSpiToUart, pData, (TickType_t) 0);
		  }
	  }

	  if ( pData[BUFF_SIZE - 1] != 0 ) flagReqZero = true;
	                             else  flagReqZero = false;
}

/**
 * @brief  spi gate control thread
 * @param  None
 * @retval None
 */
void spiGateCntrlThread(void *arg)
{
    uint32_t NoteValue = 0;

    QueueUartToSpi = xQueueCreate( MAX_SIZE_QUEUE, BUFF_SIZE);
    QueueSpiToUart = xQueueCreate( MAX_SIZE_QUEUE, BUFF_SIZE);

	for (;;) {
		NoteValue = 0;
	    if ( xTaskNotifyWait(0x00000000, 0xFFFFFFFF,  &(NoteValue), 100 ) == pdFALSE )
	    { /* timed out */

	    }
	    else
	    {
	    	if( ( NoteValue & NOTE_TRANSFER_CPLT ) != 0 )
	    	{ /* notification of completion of transfer   */
	    		filingTransmitBufSpiDMA(spiTxBuf[1]);
	    		readingReceiveBufSpiDMA(spiRxBuf[1]);
	    	}

	    	if( ( NoteValue & NOTE_TRANSFER_HALF_CPLT ) != 0 )
	    	{ /* notification of half completion of transfer   */
	    		filingTransmitBufSpiDMA(spiTxBuf[0]);
	    		readingReceiveBufSpiDMA(spiRxBuf[0]);
	    	}
	    }
	}
}

/**
 * @brief  spi gate control init
 * @param  None
 * @retval None
 */
void spiGateCntrlInit(void)
{
	spiGateHalInit((const uint8_t *)spiTxBuf,spiRxBuf,BUFF_SIZE*2);
	xTaskCreate(spiGateCntrlThread, (const char*)"S_cntrl", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioHigh, &spiCntrlHandle);
}
/******************* (C) COPYRIGHT 2024 *****END OF FILE****/

