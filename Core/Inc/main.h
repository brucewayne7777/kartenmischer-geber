/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

// Globaler Systemzustand für Not-Stopp / Run
typedef enum {
    SYSTEM_RUN = 0,
    SYSTEM_EMERGENCY_STOP
} SystemState_t;

extern volatile SystemState_t g_SystemState;
extern volatile uint8_t g_RestartFromTop;

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
void delay_us(uint16_t us);
void Check_Encoder_Button(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Lichtschranke_4_Sig_Pull_Up_Pin GPIO_PIN_0
#define Lichtschranke_4_Sig_Pull_Up_GPIO_Port GPIOC
#define ENC_SW_Pin GPIO_PIN_2
#define ENC_SW_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_0
#define LCD_RST_GPIO_Port GPIOA
#define LCD_DC_Pin GPIO_PIN_1
#define LCD_DC_GPIO_Port GPIOA
#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOA
#define Lichtschranke_3_Sig_Pull_Up_Pin GPIO_PIN_6
#define Lichtschranke_3_Sig_Pull_Up_GPIO_Port GPIOA
#define Lichtschranke_2_Sig_Pull_Up_Pin GPIO_PIN_4
#define Lichtschranke_2_Sig_Pull_Up_GPIO_Port GPIOC
#define M1_SchieberOben_IN1_M1_DIR_Pin GPIO_PIN_0
#define M1_SchieberOben_IN1_M1_DIR_GPIO_Port GPIOB
#define M3_WelleRechts_IN1_Pin GPIO_PIN_2
#define M3_WelleRechts_IN1_GPIO_Port GPIOB
#define Encoder_A_Pin GPIO_PIN_6
#define Encoder_A_GPIO_Port GPIOC
#define ENCODER_B_Pin GPIO_PIN_7
#define ENCODER_B_GPIO_Port GPIOC
#define M2_WelleLinks_IN3_Pin GPIO_PIN_8
#define M2_WelleLinks_IN3_GPIO_Port GPIOA
#define M1_SchieberOben_IN2_M1_DIR_Pin GPIO_PIN_9
#define M1_SchieberOben_IN2_M1_DIR_GPIO_Port GPIOA
#define Lichtschranke_1_Sig_Pull_Up_Pin GPIO_PIN_10
#define Lichtschranke_1_Sig_Pull_Up_GPIO_Port GPIOA
#define M3_WelleRechts_IN2_Pin GPIO_PIN_11
#define M3_WelleRechts_IN2_GPIO_Port GPIOA
#define M2_WelleLinks_IN4_Pin GPIO_PIN_15
#define M2_WelleLinks_IN4_GPIO_Port GPIOA
#define M4_SchieberLinks_IN3_Pin GPIO_PIN_3
#define M4_SchieberLinks_IN3_GPIO_Port GPIOB
#define M5_SchieberRechts_IN1_Pin GPIO_PIN_4
#define M5_SchieberRechts_IN1_GPIO_Port GPIOB
#define M6_WelleUnten_IN3_Pin GPIO_PIN_5
#define M6_WelleUnten_IN3_GPIO_Port GPIOB
#define M7_SchieberUnten_IN1_Pin GPIO_PIN_6
#define M7_SchieberUnten_IN1_GPIO_Port GPIOB
#define STEPPER_IN1_Pin GPIO_PIN_7
#define STEPPER_IN1_GPIO_Port GPIOB
#define STEPPER_IN2_Pin GPIO_PIN_8
#define STEPPER_IN2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
