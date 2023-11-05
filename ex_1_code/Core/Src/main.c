/* USER CODE BEGIN Header */
#include "Button_software.h"
#include "Software_timer.h"
#include "Light_controller.h"

/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
#define  RED_TIME_RESET		12;
#define  GREEN_TIME_RESET	9;
#define  YELLOW_TIME_RESET	3;

const int red_time_reset = RED_TIME_RESET;
const int green_time_reset = GREEN_TIME_RESET;
const int yellow_time_reset = YELLOW_TIME_RESET;

int RED_TIME  =	RED_TIME_RESET;
int GREEN_TIME = GREEN_TIME_RESET;
int YELLOW_TIME = YELLOW_TIME_RESET;

#define timer_prop  1000

#define LIGHT_BLINK_TIME 2

const int oneSec = timer_prop*1;
const int ledRefreshTime = timer_prop*0.5;

//int red_time = RED_TIME*timer_prop;
//int green_time = GREEN_TIME*timer_prop;
//int yellow_time = YELLOW_TIME*timer_prop;

int light_blink_time = LIGHT_BLINK_TIME*timer_prop;

int countdown_0 = RED_TIME_RESET;
int countdown_1 = GREEN_TIME_RESET;

int seg1Buffer[2] = {0,0};
int seg2Buffer[2] = {0,0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
STATE status = INIT_1;
MODE mode = MODE_1;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	button_reading();
	for (int i = 0 ; i < TIMER_LIMIT; i++){
		timerRun(i);
	}
}

void mode1To2(){
	if (is_button_pressed(0) == 1){
		mode = MODE_2;
		status = RED_MAN;
		setTimer(light_blink_time, 2);
		countdown_0 = RED_TIME;
		countdown_1 = 2;
		turnOffAll();
	}
}

void mode2Run(){
	if (is_button_pressed(1) == 1){
		RED_TIME = (RED_TIME < 99) ? RED_TIME+1 : RED_TIME;
		GREEN_TIME = RED_TIME - YELLOW_TIME;
	}
	else if (is_button_pressed(2) == 1){
		RED_TIME = red_time_reset;
		GREEN_TIME = RED_TIME - YELLOW_TIME;
	}

	countdown_0 = RED_TIME;
	countdown_1 = 2;
}

void mode2To3(){
	if (is_button_pressed(0) == 1){
		mode = MODE_3;
		status = YELLOW_MAN;
		setTimer(light_blink_time, 2);
		countdown_0 = YELLOW_TIME;
		countdown_1 = YELLOW_TIME;
		turnOffAll();
	}
}

void mode3Run(){
	if (is_button_pressed(1) == 1){
		YELLOW_TIME = (YELLOW_TIME < 99) ? YELLOW_TIME+1 : YELLOW_TIME;
		GREEN_TIME = RED_TIME - YELLOW_TIME;
	}
	else if (is_button_pressed(2) == 1){
		YELLOW_TIME = yellow_time_reset;
		GREEN_TIME = RED_TIME - YELLOW_TIME;
	}

	countdown_0 = YELLOW_TIME;
	countdown_1 = 3;
}

void mode3To4(){
	if (is_button_pressed(0) == 1){
		mode = MODE_4;
		status = GREEN_MAN;
		setTimer(light_blink_time, 2);
		countdown_0 = GREEN_TIME;
		countdown_1 = GREEN_TIME;
		turnOffAll();
	}
}

void mode4Run(){
	if (is_button_pressed(1) == 1){
		GREEN_TIME = (GREEN_TIME < 99) ? GREEN_TIME+1 : GREEN_TIME;
		RED_TIME = GREEN_TIME + YELLOW_TIME;
	}
	else if (is_button_pressed(2) == 1){
		GREEN_TIME = green_time_reset;
		RED_TIME = GREEN_TIME + YELLOW_TIME;
	}

	countdown_0 = GREEN_TIME;
	countdown_1 = 4;
}

void mode4To1(){
	if (is_button_pressed(0) == 1){
		mode = MODE_1;
		status = INIT_1;
		turnOffAll();
	}
}

void updateSegLed(){
	seg1Buffer[0] = (countdown_0/10);
	seg1Buffer[1] = countdown_0 % 10;

	seg2Buffer[0] = (countdown_1/10);
	seg2Buffer[1] = countdown_1 % 10;

	if (timer_flag[0] == 1){
	  update7SEG(seg1Buffer, seg2Buffer);
	  setTimer(ledRefreshTime, 0);
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  setTimer(oneSec, 1);
  setTimer(ledRefreshTime, 0);

  int isYellow = 0;

  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /*
	   * -------------- FSM ----------------
	   */
	  switch(status){
		  case (INIT_1):
			status = RED_AUTO;
			countdown_0 = RED_TIME;
			countdown_1 = GREEN_TIME;
			isYellow = 0;
			break;

		  case (RED_AUTO):
			turnOnRed(0);
			if (countdown_1 <= 0 && isYellow == 0){
				isYellow = 1;
				countdown_1 = YELLOW_TIME;
				turnOnYellow(1);
			}
			else if (isYellow == 0){
				turnOnGreen(1);
			}

			if (countdown_0 <= 0){
				status = GREEN_AUTO;
				countdown_0 = GREEN_TIME;
				countdown_1 = RED_TIME;
			}
			mode1To2();
			break;

		  case (GREEN_AUTO):
			turnOnGreen(0);
		  	turnOnRed(1);
			if (countdown_0 <= 0){
				status = YELLOW_AUTO;
				countdown_0 = YELLOW_TIME;
			}
			mode1To2();
			break;

		  case (YELLOW_AUTO):
			turnOnYellow(0);
			turnOnRed(1);
			if (countdown_0 <= 0){
				status = RED_AUTO;
				countdown_0 = RED_TIME;
				countdown_1 = GREEN_TIME;
				isYellow = 0;
			}
			mode1To2();
			break;

		  case(RED_MAN):
			if (timer_flag[2] == 1){
				setTimer(light_blink_time, 2);
				toggleRed(0);
				toggleRed(1);
			}
		  	mode2Run();
		  	mode2To3();
			break;

		  case(YELLOW_MAN):
			if (timer_flag[2] == 1){
				setTimer(light_blink_time, 2);
				toggleYellow(0);
				toggleYellow(1);
			}
			mode3Run();
			mode3To4();
			break;

		  case(GREEN_MAN):
			if (timer_flag[2] == 1){
				setTimer(light_blink_time, 2);
				toggleGreen(0);
				toggleGreen(1);
			}
			mode4Run();
			mode4To1();
			break;

		  default:
			  status = INIT_1;
			  break;
	  }

	  /*
	   * -------------- TIMER THING ----------------
	   */
	  if (mode == MODE_1){
		  if (timer_flag[1] == 1){
			  setTimer(oneSec,1);
			  countdown_0--;
			  countdown_1--;
		  }
	  }
	  else{

	  }

	  updateSegLed();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RED_1_Pin|YELLOW_1_Pin|GREEN_1_Pin|RED_2_Pin
                          |YELLOW_2_Pin|GREEN_2_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG_1_1_Pin|SEG_1_2_Pin|SEG_1_3_Pin|SEG_2_4_Pin
                          |SEG_2_5_Pin|SEG_2_6_Pin|SEG_2_7_Pin|SEG_1_4_Pin
                          |SEG_1_5_Pin|SEG_1_6_Pin|SEG_1_7_Pin|SEG_2_1_Pin
                          |SEG_2_2_Pin|SEG_2_3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BUTTON_3_Pin BUTTON_1_Pin BUTTON_2_Pin */
  GPIO_InitStruct.Pin = BUTTON_3_Pin|BUTTON_1_Pin|BUTTON_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RED_1_Pin YELLOW_1_Pin GREEN_1_Pin RED_2_Pin
                           YELLOW_2_Pin GREEN_2_Pin EN0_Pin EN1_Pin
                           EN2_Pin EN3_Pin */
  GPIO_InitStruct.Pin = RED_1_Pin|YELLOW_1_Pin|GREEN_1_Pin|RED_2_Pin
                          |YELLOW_2_Pin|GREEN_2_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG_1_1_Pin SEG_1_2_Pin SEG_1_3_Pin SEG_2_4_Pin
                           SEG_2_5_Pin SEG_2_6_Pin SEG_2_7_Pin SEG_1_4_Pin
                           SEG_1_5_Pin SEG_1_6_Pin SEG_1_7_Pin SEG_2_1_Pin
                           SEG_2_2_Pin SEG_2_3_Pin */
  GPIO_InitStruct.Pin = SEG_1_1_Pin|SEG_1_2_Pin|SEG_1_3_Pin|SEG_2_4_Pin
                          |SEG_2_5_Pin|SEG_2_6_Pin|SEG_2_7_Pin|SEG_1_4_Pin
                          |SEG_1_5_Pin|SEG_1_6_Pin|SEG_1_7_Pin|SEG_2_1_Pin
                          |SEG_2_2_Pin|SEG_2_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
