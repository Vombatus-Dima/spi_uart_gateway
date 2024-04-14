/**
  ******************************************************************************
  * @file    spi_gate_cntrl.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_GATE_HAL_H
#define __SPI_GATE_HAL_H

/**
  * @brief configure the UART peripheral
  * @param  pTxData: pointer to transmission data buffer
  * @param  pRxData: pointer to reception data buffer
  * @param  Size   : amount of data to be sent
  * @retval None
  */
void spiGateHalInit(const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);

#endif /* __SPI_GATE_CNTRL_H */
/******************* (C) COPYRIGHT 2024 *****END OF FILE****/
