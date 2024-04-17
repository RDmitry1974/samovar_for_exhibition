/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_dac.h"
#include "stm32l0xx.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_cortex.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_dma.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
    
#define     IZVRAT_MIRROR_LEDS    
    
#define     BIT(n) (1u << n)
#define     LSHIFT(v,n) (((unsigned int)(v) << n))
#define     LEDS_NUM             298
#define     COLRS                3
#define     K_LOST_SAMOVAR       -0.08
#define     K_LOST_CUP           -0.1
#define     V_MAX_SAMOVAR        100.0
#define     V_MAX_CUP            10.0
//объем за квант времени поступающей жидкости
#define     V_INP_SAMOVAR        (V_MAX_SAMOVAR / 3000)
//объем за квант времени убывающей жидкости
//#define     V_OUT_SAMOVAR        (V_MAX_SAMOVAR / 10000)
#define     V_OUT_SAMOVAR        (V_MAX_SAMOVAR / 3000)
#define     V_INP_CUP            V_OUT_SAMOVAR
#define     V_OUT_CUP            V_MAX_CUP / 100
#define     CUP_LED_LINE         3
#define     SAMOVAR_LED_LINE     17
#define     ADC_4mA_VALUE        800
#define     ADC_20mA_VALUE       3900
#define     DAC_4mA_VALUE        800
#define     DAC_20mA_VALUE       3835
#define     SAMOVAR_MAX_HEATING  75.0
#define     MAX_BUBBLES_STRIMS   15
#define     MIN_TEMPER_BUBBLES_PROCESS   50.0
//в максимальном кол-ве пузырьков часть приход€ща€с€ на температурную составл€ющую
#define     TEMPER_BUBBLES_PART   (1/3)
    // периодичность  основного цикла в мс
#define     MAIN_CYCLE_TIME      10
extern const unsigned short SAMOVAR_LED_PER_LINE_TBL[];    
extern const unsigned short SAMOVAR_LED_VS_NUMBER_LINE_TBL[];    
extern float q_samovar_heating;    
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
