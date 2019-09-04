/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_FR_Pin GPIO_PIN_2
#define LED_FR_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_3
#define LED_R_GPIO_Port GPIOC
#define AD_FR_Pin GPIO_PIN_0
#define AD_FR_GPIO_Port GPIOA
#define AD_R_Pin GPIO_PIN_1
#define AD_R_GPIO_Port GPIOA
#define AD_FF_Pin GPIO_PIN_2
#define AD_FF_GPIO_Port GPIOA
#define LED_FF_Pin GPIO_PIN_3
#define LED_FF_GPIO_Port GPIOA
#define AD_L_Pin GPIO_PIN_4
#define AD_L_GPIO_Port GPIOA
#define AD_FL_Pin GPIO_PIN_5
#define AD_FL_GPIO_Port GPIOA
#define LED_L_Pin GPIO_PIN_6
#define LED_L_GPIO_Port GPIOA
#define LED_FL_Pin GPIO_PIN_7
#define LED_FL_GPIO_Port GPIOA
#define STBY_Pin GPIO_PIN_5
#define STBY_GPIO_Port GPIOC
#define MOTOR_L_DIR1_Pin GPIO_PIN_0
#define MOTOR_L_DIR1_GPIO_Port GPIOB
#define MOTOR_L_DIR2_Pin GPIO_PIN_1
#define MOTOR_L_DIR2_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_14
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_15
#define LED2_GPIO_Port GPIOB
#define ENCODER_L_CH__Pin GPIO_PIN_6
#define ENCODER_L_CH__GPIO_Port GPIOC
#define ENCODER_L_CH2_Pin GPIO_PIN_7
#define ENCODER_L_CH2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_8
#define LED3_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_9
#define BUZZER_GPIO_Port GPIOC
#define MOTOR_L_PWM_Pin GPIO_PIN_8
#define MOTOR_L_PWM_GPIO_Port GPIOA
#define MOTOR_R_DIR1_Pin GPIO_PIN_14
#define MOTOR_R_DIR1_GPIO_Port GPIOA
#define MOTOR_R_DIR2_Pin GPIO_PIN_15
#define MOTOR_R_DIR2_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_2
#define CS_GPIO_Port GPIOD
#define MOTOR_R_PWM_Pin GPIO_PIN_3
#define MOTOR_R_PWM_GPIO_Port GPIOB
#define LED4_Pin GPIO_PIN_4
#define LED4_GPIO_Port GPIOB
#define LED5_Pin GPIO_PIN_5
#define LED5_GPIO_Port GPIOB
#define ENCODER_R_CH1_Pin GPIO_PIN_6
#define ENCODER_R_CH1_GPIO_Port GPIOB
#define ENCODER_R_CH2_Pin GPIO_PIN_7
#define ENCODER_R_CH2_GPIO_Port GPIOB
#define BATTERY_Pin GPIO_PIN_8
#define BATTERY_GPIO_Port GPIOB
#define FUN_Pin GPIO_PIN_9
#define FUN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
