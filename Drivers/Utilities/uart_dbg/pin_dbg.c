/**
  ******************************************************************************
  * @file    pin_dbg.c
  * @version V1.0.0
  * @date    07-09-2020
  * @brief   Инициализация драйвера GPIO для отладки
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "pin_dbg.h"

#ifdef DBG_PIN_ENABLE
/**
  * @brief  Инициализация GPIO для отладки.
  * @param  None
  * @retval None
  */
void debug_pin_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;	
		
	T1_CLK_ENABLE();
	GPIO_InitStruct.Pin       = T1_PIN;
	HAL_GPIO_Init(T1_PORT, &GPIO_InitStruct);
	T1_LO;
	
	T2_CLK_ENABLE();
	GPIO_InitStruct.Pin       = T2_PIN;
	HAL_GPIO_Init(T2_PORT, &GPIO_InitStruct);
	T2_LO;
	
	//T3_CLK_ENABLE();
	//GPIO_InitStruct.Pin       = T3_PIN;
	//HAL_GPIO_Init(T3_PORT, &GPIO_InitStruct);
	//T3_LO;
	
	//T4_CLK_ENABLE();
	//GPIO_InitStruct.Pin       = T4_PIN;
	//HAL_GPIO_Init(T4_PORT, &GPIO_InitStruct);
	//T4_LO;
	
}

#endif  /* DBG_PIN_ENABLE */
/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/
