
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
#include "tim.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "PS2Keyboard.h"
#include "brailleMatrix.h"
#include "DCMotors.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define ENCODER_1_A_PORT           GPIOF
#define ENCODER_1_A_PIN            GPIO_PIN_0

#define ENCODER_1_B_PORT           GPIOF
#define ENCODER_1_B_PIN            GPIO_PIN_1

#define ENCODER_2_A_PORT           GPIOA
#define ENCODER_2_A_PIN            GPIO_PIN_2

#define ENCODER_2_B_PORT           GPIOA
#define ENCODER_2_B_PIN            GPIO_PIN_3


#define MAX_CARACTERES 2
#define MAX_LINHAS 27
#define POS_INI 0
#define POS_FIM 4000

char pressedEnter = 1;
char isEnd = 0;

Keyboard_TypeDef keyboard;

MotorControl_t motorX;
MotorControl_t motorY;


//unsigned char linhaBraille[MAX_CARACTERES] = {'A','B'};
unsigned char buffer_char[MAX_CARACTERES] = {'A', 'B'};
unsigned char buffer_braille[4];


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void interruption();

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void interruption(){
	ps2interrupt(&keyboard);
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
	initMotor(&motorX, ENCODER_1_A_PORT, ENCODER_1_A_PIN, ENCODER_1_B_PORT, ENCODER_1_B_PIN, htim14, htim3, TIM_CHANNEL_1,  TIM_CHANNEL_1);
	initMotor(&motorY, ENCODER_2_A_PORT, ENCODER_2_A_PIN, ENCODER_2_B_PORT, ENCODER_2_B_PIN, htim3, htim3, TIM_CHANNEL_2,  TIM_CHANNEL_4);

	memset(buffer_braille, 0, sizeof(buffer_braille));

	keyboardBegin(&keyboard, PS2_DATA_PORT, PS2_DATA_PIN, PS2_IQR_PORT, PS2_IQR_PIN);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)  {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		/* Faz a leitura do teclado e envia ao buffer_char */
		if(keyboardAvailable(&keyboard)){
			uint8_t c = keyboardRead(&keyboard);
			if(c == PS2_ENTER){
				pressedEnter = 1;
			}
			feedBuffer(buffer_char, MAX_CARACTERES, c);
		}
		/* Programa leitura do teclado */
		if(pressedEnter){

			for(int j=0;j<3;j++){

				if(j==1){
					//Linhas das matrizes
					for(int i=strlen(buffer_char)-1; i>=0; --i){

						// Recebe os pontos da linha para o caractere atual
						fillLineWithBraille(buffer_braille, j,buffer_char[i]);

						// Percorre os 4 bits, no máximo, para cada caractere
						for(int x = 3; x >=0; ++x){

							// Passa para a próxima iteração se o não tem ponto para furar
							if(buffer_braille[x] == '\0'){
								continue;
							}else if(buffer_braille[x] == 1){
								//Furar
							}

							// Decrementa posição do eixo x, espaçamento entre colunas
							atualizarEixo(&motorX, motorX.setPoint - DELTA_COL_LIN);
						}

						// Decrementa posição do eixo x, espaçamento entre char na horizontal
						atualizarEixo(&motorX, motorX.setPoint - DELTA_CHAR_H);

					}

					// Decrementa posição do eixo y, espaçamento entre linhas
					atualizarEixo(&motorY, motorY.setPoint + DELTA_COL_LIN);
				}else{

					//Linhas das matrizes
					for(int i=0; i<strlen(buffer_char); i++){

						//Recebe os pontos da linha para o caractere atual
						fillLineWithBraille(buffer_braille, j,buffer_char[i]);

						//Percorre os 4 bits, no máximo, para cada caractere
						for(int x = 0; x < 4; ++x){

							if(buffer_braille[x] == '\0'){
								break;
							}else if(buffer_braille[x] == 1){
								//Furar
							}

							// Incrementa posição do eixo x, espaçamento entre colunas
							atualizarEixo(&motorX, motorX.setPoint + DELTA_COL_LIN);
						}

						// Incrementa posição do eixo x, espaçamento entre char na horizontal
						atualizarEixo(&motorX, motorX.setPoint + DELTA_CHAR_H);

					}

					// Incrementa posição do eixo y, espaçamento entre linhas
					atualizarEixo(&motorY, motorY.setPoint + DELTA_COL_LIN);
				}
			}

			// Incrementa posição do eixo y, espaçamento entre char na vertical
			atualizarEixo(&motorY, motorY.setPoint + DELTA_CHAR_V - DELTA_COL_LIN);

			// Seta posição eixo x para inicial
			atualizarEixo(&motorX, POS_INI);

			pressedEnter = 0;
		}
		clearBuffer(buffer_char);

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
