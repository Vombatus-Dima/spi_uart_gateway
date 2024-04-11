/**
  ******************************************************************************
  * @file    uart_gate_cntrl.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_GATE_HAL_H
#define __UART_GATE_HAL_H

void uartGateTxData(const uint8_t *pData, uint16_t Size);
void uartGateHalInit(void);

#endif /* __UART_GATE_CNTRL_H */
/******************* (C) COPYRIGHT 2024 *****END OF FILE****/
