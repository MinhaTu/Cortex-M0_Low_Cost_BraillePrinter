
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define SENSOR_1_A_PORT           GPIOF
#define SENSOR_1_A_PIN            GPIO_PIN_0

#define SENSOR_1_B_PORT           GPIOF
#define SENSOR_1_B_PIN            GPIO_PIN_1

#define SENSOR_2_A_PORT           GPIOA
#define SENSOR_2_A_PIN            GPIO_PIN_2

#define SENSOR_2_B_PORT           GPIOA
#define SENSOR_2_B_PIN            GPIO_PIN_3

#define MOTOR_1_DIRECTION_A_PORT  GPIOA
#define MOTOR_1_DIRECTION_A_PIN   GPIO_PIN_4

#define MOTOR_1_DIRECTION_B_PORT  GPIOA
#define MOTOR_1_DIRECTION_B_PIN   GPIO_PIN_6

#define MOTOR_2_DIRECTION_A_PORT  GPIOA
#define MOTOR_2_DIRECTION_A_PIN   GPIO_PIN_7

#define MOTOR_2_DIRECTION_B_PORT  GPIOB
#define MOTOR_2_DIRECTION_B_PIN   GPIO_PIN_1

#define MOTOR_3_DIRECTION_A_PORT  GPIOA
#define MOTOR_3_DIRECTION_A_PIN   GPIO_PIN_9

#define MOTOR_3_DIRECTION_B_PORT  GPIOA
#define MOTOR_3_DIRECTION_B_PIN   GPIO_PIN_10

#define MAX_CARACTERES 31
#define MAX_LINHAS 27

#define P_FRACTION 1.0     //Proportional factor of control loop 0.001 - 10.0 (1.0)
#define STEP_MARGIN 1L     //10 - 1000 (1)

#define MIN_DUTYCYCLE 100   //0 - 255 (125)
#define MAX_DUTYCYCLE 127  //0 - 255 (255)

int stepStatusOld_1 = 0;
int stepStatusOld_2 = 0;

GPIO_PinState sensorStatus_1_A = GPIO_PIN_RESET;
GPIO_PinState sensorStatus_1_B = GPIO_PIN_RESET;

GPIO_PinState sensorStatus_2_A = GPIO_PIN_RESET;
GPIO_PinState sensorStatus_2_B = GPIO_PIN_RESET;

int dutyCycle = 0;

signed long setPoint_1 = 0;
signed long setPoint_2 = 0;

signed long actualPoint_1 = 0;
signed long actualPoint_2 = 0;
char pressedEnter = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM14_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM1_Init(void);                                    
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                
                                
                                

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void atualizarEixoX();
void atualizarEixoY();

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void atualizarEixoX(){
	 sensorStatus_1_A =  HAL_GPIO_ReadPin(SENSOR_1_A_PORT, SENSOR_1_A_PIN);
	 sensorStatus_1_B =  HAL_GPIO_ReadPin(SENSOR_1_B_PORT, SENSOR_1_B_PIN);

	  if(sensorStatus_1_A == GPIO_PIN_RESET && sensorStatus_1_B == GPIO_PIN_RESET){
		  if(stepStatusOld_1 == 3){
			  actualPoint_1++;
		  }else if(stepStatusOld_1 == 1){
			  actualPoint_1--;
		  }

		  stepStatusOld_1 = 0;
	  }

	  if(sensorStatus_1_A == GPIO_PIN_SET && sensorStatus_1_B == GPIO_PIN_RESET){
		  if(stepStatusOld_1 == 0){
			  actualPoint_1++;
		  }else if(stepStatusOld_1 == 2){
			  actualPoint_1--;
		  }

		  stepStatusOld_1 = 1;
	  }

	  if(sensorStatus_1_A == GPIO_PIN_SET && sensorStatus_1_B == GPIO_PIN_SET){
		  if(stepStatusOld_1 == 1){
			  actualPoint_1++;
		  }else if(stepStatusOld_1 == 3){
			  actualPoint_1--;
		  }

		  stepStatusOld_1 = 2;
	  }

	  if(sensorStatus_1_A == GPIO_PIN_SET && sensorStatus_1_B == GPIO_PIN_SET){
		  if(stepStatusOld_1 == 2){
			  actualPoint_1++;
		  }else if(stepStatusOld_1 == 3){
			  actualPoint_1--;
		  }

		  stepStatusOld_1 = 3;
	  }

	  /* Cálculo PWM */
	  dutyCycle = abs((double)(setPoint_1 - actualPoint_1)) * (double)P_FRACTION;

	  if(dutyCycle < MIN_DUTYCYCLE){
		dutyCycle = MIN_DUTYCYCLE;
	  }
	  if(dutyCycle > MAX_DUTYCYCLE){
		dutyCycle = MAX_DUTYCYCLE;
	  }

	  if(dutyCycle < MIN_DUTYCYCLE){
		dutyCycle = MIN_DUTYCYCLE;
	  }
	  if(dutyCycle > MAX_DUTYCYCLE){
		dutyCycle = MAX_DUTYCYCLE;
	  }
	  if(abs((double)(setPoint_1 - actualPoint_1)) < STEP_MARGIN){
			/* Desliga o motor pras duas direções */
			__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
	  }
	  else{
		if(actualPoint_1 < setPoint_1){
			/* Gira em uma direção */
			__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 255 - dutyCycle);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
		}
		if(actualPoint_1 > setPoint_1){
			/* Gira na outra direção */
			__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 255 - dutyCycle);
		}
	  }
}


void atualizarEixoY(){
	 sensorStatus_2_A =  HAL_GPIO_ReadPin(SENSOR_2_A_PORT, SENSOR_2_A_PIN);
	 sensorStatus_2_B =  HAL_GPIO_ReadPin(SENSOR_2_B_PORT, SENSOR_2_B_PIN);

	  if(sensorStatus_2_A == GPIO_PIN_RESET && sensorStatus_2_B == GPIO_PIN_RESET){
		  if(stepStatusOld_2 == 3){
			  actualPoint_2++;
		  }else if(stepStatusOld_2 == 1){
			  actualPoint_2--;
		  }

		  stepStatusOld_2 = 0;
	  }

	  if(sensorStatus_2_A == GPIO_PIN_SET && sensorStatus_2_B == GPIO_PIN_RESET){
		  if(stepStatusOld_2 == 0){
			  actualPoint_2++;
		  }else if(stepStatusOld_2 == 2){
			  actualPoint_2--;
		  }

		  stepStatusOld_2 = 1;
	  }

	  if(sensorStatus_2_A == GPIO_PIN_SET && sensorStatus_2_B == GPIO_PIN_SET){
		  if(stepStatusOld_2 == 1){
			  actualPoint_2++;
		  }else if(stepStatusOld_2 == 3){
			  actualPoint_2--;
		  }

		  stepStatusOld_2 = 2;
	  }

	  if(sensorStatus_2_A == GPIO_PIN_SET && sensorStatus_2_B == GPIO_PIN_SET){
		  if(stepStatusOld_2 == 2){
			  actualPoint_2++;
		  }else if(stepStatusOld_2 == 3){
			  actualPoint_2--;
		  }

		  stepStatusOld_1 = 3;
	  }

	  /* Cálculo PWM */
	  dutyCycle = abs((double)(setPoint_2 - actualPoint_2)) * (double)P_FRACTION;

	  if(dutyCycle < MIN_DUTYCYCLE){
		dutyCycle = MIN_DUTYCYCLE;
	  }
	  if(dutyCycle > MAX_DUTYCYCLE){
		dutyCycle = MAX_DUTYCYCLE;
	  }

	  if(dutyCycle < MIN_DUTYCYCLE){
		dutyCycle = MIN_DUTYCYCLE;
	  }
	  if(dutyCycle > MAX_DUTYCYCLE){
		dutyCycle = MAX_DUTYCYCLE;
	  }
	  if(abs((double)(setPoint_2 - actualPoint_2)) < STEP_MARGIN){
			/* Desliga o motor pras duas direções */
			__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
	  }
	  else{
		if(actualPoint_2 < setPoint_2){
			/* Gira em uma direção */
			__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 255 - dutyCycle);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
		}
		if(actualPoint_2 > setPoint_2){
			/* Gira na outra direção */
			__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 255 - dutyCycle);
		}
	  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_TIM14_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

  HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);

  /* Motor 1 - x*/
	__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);

	/* Motor 2 */
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);

	/* Motor 3 */
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);

	sensorStatus_1_A =  HAL_GPIO_ReadPin(SENSOR_1_A_PORT, SENSOR_1_A_PIN);
	sensorStatus_1_B =  HAL_GPIO_ReadPin(SENSOR_1_B_PORT, SENSOR_1_B_PIN);

	sensorStatus_2_A =  HAL_GPIO_ReadPin(SENSOR_2_A_PORT, SENSOR_2_A_PIN);
	sensorStatus_2_B =  HAL_GPIO_ReadPin(SENSOR_2_B_PORT, SENSOR_2_B_PIN);

	stepStatusOld_1 = 0;
	stepStatusOld_2 = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
 	  atualizarEixoX();

 	  /* Programa leitura do teclado */
 	  if(pressedEnter){
 		  setPoint_1 += 30;
 	  }

   }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM1 init function */
static void MX_TIM1_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 4;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 255;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim1);

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 4;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 255;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim3);

}

/* TIM14 init function */
static void MX_TIM14_Init(void)
{

  TIM_OC_InitTypeDef sConfigOC;

  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 4;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 255;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim14);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : PF0 PF1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
