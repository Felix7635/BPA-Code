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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern uint8_t DMX_Buffer_IT[513];
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_SD_Pin GPIO_PIN_2
#define LED_SD_GPIO_Port GPIOE
#define LED_STATE_Pin GPIO_PIN_3
#define LED_STATE_GPIO_Port GPIOE
#define LED_RX_Pin GPIO_PIN_4
#define LED_RX_GPIO_Port GPIOE
#define LED_TX_Pin GPIO_PIN_5
#define LED_TX_GPIO_Port GPIOE
#define LED_PWM_Pin GPIO_PIN_6
#define LED_PWM_GPIO_Port GPIOE
#define DMX_TX_Pin GPIO_PIN_0
#define DMX_TX_GPIO_Port GPIOA
#define DMX_RX_Pin GPIO_PIN_1
#define DMX_RX_GPIO_Port GPIOA
#define DMX_DE_Pin GPIO_PIN_2
#define DMX_DE_GPIO_Port GPIOA
#define BTN_BACK_Pin GPIO_PIN_5
#define BTN_BACK_GPIO_Port GPIOA
#define BTN_ENTER_Pin GPIO_PIN_6
#define BTN_ENTER_GPIO_Port GPIOA
#define BTN_DOWN_Pin GPIO_PIN_7
#define BTN_DOWN_GPIO_Port GPIOA
#define BTN_UP_Pin GPIO_PIN_4
#define BTN_UP_GPIO_Port GPIOC
#define ENB_B_Pin GPIO_PIN_12
#define ENB_B_GPIO_Port GPIOB
#define ENC_A_Pin GPIO_PIN_13
#define ENC_A_GPIO_Port GPIOB
#define SDIO_DETECT_Pin GPIO_PIN_9
#define SDIO_DETECT_GPIO_Port GPIOC
#define LCD_PWM_Pin GPIO_PIN_11
#define LCD_PWM_GPIO_Port GPIOA
#define LCD_RS_Pin GPIO_PIN_4
#define LCD_RS_GPIO_Port GPIOD
#define LCD_E_Pin GPIO_PIN_5
#define LCD_E_GPIO_Port GPIOD
#define LCD_D0_Pin GPIO_PIN_6
#define LCD_D0_GPIO_Port GPIOD
#define LCD_D1_Pin GPIO_PIN_7
#define LCD_D1_GPIO_Port GPIOD
#define LCD_D2_Pin GPIO_PIN_9
#define LCD_D2_GPIO_Port GPIOG
#define LCD_D3_Pin GPIO_PIN_10
#define LCD_D3_GPIO_Port GPIOG
#define LCD_D4_Pin GPIO_PIN_11
#define LCD_D4_GPIO_Port GPIOG
#define LCD_D5_Pin GPIO_PIN_12
#define LCD_D5_GPIO_Port GPIOG
#define LCD_D6_Pin GPIO_PIN_13
#define LCD_D6_GPIO_Port GPIOG
#define LCD_D7_Pin GPIO_PIN_14
#define LCD_D7_GPIO_Port GPIOG
/* USER CODE BEGIN Private defines */

#define UART_BUFFER_SIZE 513
#define DMA_RX_BUFFER_SIZE (2 * UART_BUFFER_SIZE)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
