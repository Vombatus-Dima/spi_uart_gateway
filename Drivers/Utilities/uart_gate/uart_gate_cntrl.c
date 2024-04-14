/**
  ******************************************************************************
  * @file    uart_gate_cntrl.c
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
#include "uart_gate_cntrl.h"
#include "uart_gate_hal.h"

#define UART_DATA_TX_CPLT 0x00000004



TaskHandle_t  TxCntrlHandle;
TaskHandle_t  RxCntrlHandle;

extern QueueHandle_t QueueSpiToUart;
extern QueueHandle_t QueueUartToSpi;

volatile uint32_t indexWrUartRxData = 0;
#define UART_GATE_RX_DATA_SIZE   (100)
uint8_t uartGateRxBuf[UART_GATE_RX_DATA_SIZE] = {0};

uint32_t indexStartRxBox = 0;
uint32_t indexStopRxBox = 0;
uint32_t indexSearchBox = 0;
uint32_t indexRdUartRxData = 0;

/**
  * @brief  fsm searching for frame borders
  */
typedef enum
{
  INIT = 0,
  SEARCH_START,
  SEARCH_STOP,
}fsmSearchFrameBoard_e;

fsmSearchFrameBoard_e fsmCntrlUartRx = INIT;

/**
  * @brief fsm control rx UART rx gate
  */
typedef enum
{
  SEARCH = 0,
  SEND,
}fsmCntlFrame_e;

fsmCntlFrame_e fsmCntlFrameUartRx = SEARCH;

/**
 * @brief searching for frame borders
 * @param  None
 * @retval None
 */
uint8_t searchFrameBorders(void)
{
	if (indexSearchBox == indexWrUartRxData)
		return 0;

	switch (fsmCntrlUartRx)
	{
	case INIT:
		fsmCntrlUartRx = SEARCH_START;
		indexSearchBox = indexStopRxBox;
		return 1;

	case SEARCH_START:
		if (uartGateRxBuf[indexSearchBox] != 0)
		{
			indexRdUartRxData = indexSearchBox;

			fsmCntrlUartRx = SEARCH_STOP;
		}
		if (indexSearchBox < (UART_GATE_RX_DATA_SIZE - 1))
			indexSearchBox++;
		else
			indexSearchBox = 0;
		return 1;

	case SEARCH_STOP:
		if (uartGateRxBuf[indexSearchBox] == 0)
		{
			indexStopRxBox = indexSearchBox;
			fsmCntrlUartRx = INIT;
			return 2;
		}
		else
		{
		if (indexSearchBox < (UART_GATE_RX_DATA_SIZE - 1))
			indexSearchBox++;
		else
			indexSearchBox = 0;
		return 1;
		}

	}

	return 0;
}

/**
 * @brief filling for frame
 * @param  pData Pointer to data buffer u8
 * @retval None
 */
uint8_t fillFrameBorders(uint8_t *pData)
{
	uint8_t codeRtrn = 1;

   for (uint32_t cntic = 0; cntic < BUFF_SIZE; cntic++)
   {
	   if ( indexRdUartRxData != indexStopRxBox )
	     {
		   pData[cntic] = uartGateRxBuf[indexRdUartRxData];
		   	if (indexRdUartRxData < (UART_GATE_RX_DATA_SIZE - 1))
		   		indexRdUartRxData++;
			else
				indexRdUartRxData = 0;
	     }
	   else
	     {
		   pData[cntic] = 0;
		   codeRtrn = 0;
	     }
   }
   return codeRtrn;
}

/**
 * @brief returns the frame size
 * @param  uint32_t strtBorder, uint32_t stpBorder
 * @retval uint32_t - size frame
 */
uint32_t getSizeFrame( uint32_t strtBorder, uint32_t stpBorder )
{
	if ( strtBorder <= stpBorder  ) return 	stpBorder - strtBorder;
                                	 else return stpBorder + UART_GATE_RX_DATA_SIZE - stpBorder;
}


/**
 * @brief  uart rx gate control thread
 * @param  None
 * @retval None
 */
void uartGateRxCntrlThread(void *arg)
{
	//uint32_t index_start =0;
	//uint32_t index_stop =0;
	uint8_t stSearch = 0;
	uint8_t stSend = 0;
	uint8_t uartGateRxTempBuf[BUFF_SIZE] =
	{ 0 };

	for (;;)
	{
		switch (fsmCntlFrameUartRx)
		{
		case SEARCH:

			do
			{
				stSearch = searchFrameBorders();
			} while (stSearch == 1);

			if (stSearch == 0) 	break;
			else
			{
				fsmCntlFrameUartRx = SEND;
			}
		case SEND:
			do
			{
				if (QueueUartToSpi == NULL) break;
				if (uxQueueSpacesAvailable(QueueUartToSpi) == 0) break;
				stSend = fillFrameBorders(uartGateRxTempBuf);
				xQueueSend(QueueUartToSpi, uartGateRxTempBuf, (TickType_t) 0);

			} while (stSend == 1);
			fsmCntlFrameUartRx = SEARCH;
			break;
		}
		vTaskDelay(1);
	}
}



/**
 * @brief  writing a received byte to a buffer
 * @param  uint8_t data
 * @retval None
 */
void uartGateRxData(uint8_t data)
{
	uartGateRxBuf[indexWrUartRxData] = data;
	if ( indexWrUartRxData <  ( UART_GATE_RX_DATA_SIZE - 1)  )
	{
		indexWrUartRxData++;
	}
	else
	{
		indexWrUartRxData = 0;
	}
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
 /* sending notification of completion of transfer */
 if ( TxCntrlHandle != NULL )
  {
	BaseType_t IrqHighPrTskWoken = pdFALSE;
    xTaskNotifyFromISR( TxCntrlHandle,
                       UART_DATA_TX_CPLT,
                       eSetBits,
                       &IrqHighPrTskWoken );
    portYIELD_FROM_ISR( IrqHighPrTskWoken );
  }
}

/**
  * @brief loading data into UART DMA and waiting for completion of sending
  * @param pData Pointer to data buffer u8.
  * @param Size  Amount of data elements u8 to be sent.
  * @retval None
  */
void loadUartDmaAndWait(const uint8_t *pData, uint16_t Size)
{
    uint32_t NoteValue = 0;
	uartGateTxData( pData, Size);
	for (;;)
	{
		NoteValue = 0;
	    xTaskNotifyWait(0x00000000, 0xFFFFFFFF,  &(NoteValue), portMAX_DELAY );
	    if( ( NoteValue & UART_DATA_TX_CPLT ) != 0 ) return;
	}
}

/**
 * @brief  uart tx gate control thread
 * @param  None
 * @retval None
 */
void uartGateTxCntrlThread(void *arg)
{
	uint8_t uartGateTxTempBuf[BUFF_SIZE] = {0};
	bool flagReqZero = false;
	bool flagFindStart = false;
	bool flagFindStop = false;
	uint32_t index_start =0;
	uint32_t index_stop =0;

	for (;;) {

		if (QueueSpiToUart != NULL)
		{
			index_start =0;
		    index_stop =0;

			xQueueReceive(QueueSpiToUart,&uartGateTxTempBuf, portMAX_DELAY );
			if ( flagReqZero && ( uartGateTxTempBuf[0] == 0))
			{
				loadUartDmaAndWait(uartGateTxTempBuf,1);
				index_start = 1;
			}

			for (;;) {
				//find start
				flagFindStart = false;
				for(uint32_t cntic = index_start; cntic < BUFF_SIZE; cntic++ )
				{
					if ( uartGateTxTempBuf[cntic] != 0 )
					{
						index_start = cntic;
						flagFindStart = true;
						break;
					}
				}

				if (!flagFindStart) break;
				//find stop

				flagFindStop = false;
				for(uint32_t cntic = index_start; cntic < BUFF_SIZE; cntic++ )
				{
					if ( uartGateTxTempBuf[cntic] == 0 )
					{
						index_stop = cntic + 1;
						flagFindStop = true;
						break;
					}
				}

				if (!flagFindStop)
					{
					   loadUartDmaAndWait(&(uartGateTxTempBuf[index_start]),BUFF_SIZE - index_start);
					   break;
					}
				else
				    {
					   loadUartDmaAndWait(&(uartGateTxTempBuf[index_start]),index_stop - index_start);
					   index_start = index_stop;
				    }
			}
			if ( uartGateTxTempBuf[BUFF_SIZE-1] != 0) flagReqZero = true;
			else flagReqZero = false;
		}
		else
		{
			vTaskDelay(1);
		}
	}
}

/**
 * @brief  uart gate control init
 * @param  None
 * @retval None
 */
void uartGateCntrlInit(void)
{
	uartGateHalInit();
	xTaskCreate(uartGateTxCntrlThread, (const char*)"U_TxCntrl", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioHigh, &TxCntrlHandle);
	xTaskCreate(uartGateRxCntrlThread, (const char*)"U_RxCntrl", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioHigh, &RxCntrlHandle);
}
/******************* (C) COPYRIGHT 2024 *****END OF FILE****/
