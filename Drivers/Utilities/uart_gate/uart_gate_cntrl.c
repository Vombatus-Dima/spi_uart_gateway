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

uint8_t data[10] = {0x35,0x36,0x37,0x38,0x39,0x38,0x37,0x36,0x35,0x34};

/**
 * @brief  uart gate control thread
 * @param  None
 * @retval None
 */
void uartGateCntrlThread(void *arg)
{
	uartGateHalInit();

	for (;;) {

		uartGateTxData(data,10);
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
	xTaskCreate(uartGateCntrlThread, (const char*)"U_cntrl", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioHigh, NULL);
}


/******************* (C) COPYRIGHT 2024 *****END OF FILE****/
