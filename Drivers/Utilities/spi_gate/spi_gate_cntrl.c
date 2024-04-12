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

/**
 * @brief  spi gate control thread
 * @param  None
 * @retval None
 */
void spiGateCntrlThread(void *arg)
{
	spiGateHalInit();
	for (;;) {
		vTaskDelay(100);
	}
}

/**
 * @brief  spi gate control init
 * @param  None
 * @retval None
 */
void spiGateCntrlInit(void)
{
	xTaskCreate(spiGateCntrlThread, (const char*)"S_cntrl", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioHigh, NULL);
}
/******************* (C) COPYRIGHT 2024 *****END OF FILE****/

