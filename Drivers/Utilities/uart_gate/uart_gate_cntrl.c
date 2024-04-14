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

#define UART_DATA_TX_CPLT  (1)

#define UART_GATE_TX_DATA_SIZE   (100)
#define UART_GATE_RX_DATA_SIZE   (100)
uint8_t uartGateTxBuf[UART_GATE_TX_DATA_SIZE] = {0};
uint8_t uartGateRxBuf[UART_GATE_RX_DATA_SIZE] = {0};
uint32_t cntRxData = 0;

TaskHandle_t  TxCntrlHandle;
TaskHandle_t  RxCntrlHandle;


/**
 * @brief
 * @param  uint8_t data
 * @retval None
 */
void uartGateRxData(uint8_t data)
{
	uartGateRxBuf[cntRxData] = data;
	if ( data == 0 )
	{
          // отправка индекса в очередь сообщений

	}
	if (++cntRxData >= UART_GATE_RX_DATA_SIZE) cntRxData = 0;
}

/**
 * @brief sending notification of completion of transfer
 * @param  None
 * @retval None
 */
void uartGateTxCpl(void)
{
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
 * @brief  uart tx gate control thread
 * @param  None
 * @retval None
 */
void uartGateTxCntrlThread(void *arg)
{
    uint32_t NoteValue = 0;

	for (;;) {
		NoteValue = 0;
	    if ( xTaskNotifyWait(0x00000000, 0xFFFFFFFF,  &(NoteValue), 100 ) == pdFALSE )
	    { /* timed out */

	    }
	    else
	    {
	    	if( ( NoteValue & UART_DATA_TX_CPLT ) != 0 )
	    	{ /* notification of completion of transfer   */

	    	}
	    }
	}
}

/**
 * @brief  uart rx gate control thread
 * @param  None
 * @retval None
 */
void uartGateRxCntrlThread(void *arg)
{
	for (;;) {
		uartGateTxData(uartGateTxBuf,10);
		vTaskDelay(100);
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
