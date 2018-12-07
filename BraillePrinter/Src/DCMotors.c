#include "DCMotors.h"
#include "tim.h"
#include "gpio.h"

void motorBegin(MotorControl_t* motor, GPIO_TypeDef* encoderA_port, uint16_t encoderA_pin, GPIO_TypeDef* encoderB_port, uint16_t encoderB_pin, TIM_HandleTypeDef timer_left, TIM_HandleTypeDef timer_right, uint16_t channel_left, uint16_t channel_right){
	motor->dutyCycle = 0;

	motor->setPoint = 0;
	motor->actualPoint = 0;

	motor->stepStatusOld = 0;

	motor->encoderA_port = encoderA_port;
	motor->encoderA_pin = encoderA_pin;

	motor->encoderB_port = encoderB_port;
	motor->encoderB_pin = encoderB_pin;

	motor->timer_left = timer_left;
	motor->timer_right = timer_right;

	motor->channel_left = channel_left;
	motor->channel_right = channel_right;

	HAL_TIM_PWM_Start(&motor->timer_left, motor->channel_left);
	HAL_TIM_PWM_Start(&motor->timer_right, motor->channel_right);

	__HAL_TIM_SET_COMPARE(&motor->timer_left, motor->channel_left, 0);
	__HAL_TIM_SET_COMPARE(&motor->timer_right, motor->channel_right, 0);

}

double myABS(double num1){
	if(num1 < 0){
		num1 += -1;
	}

	return num1;
}


void atualizarEixo(MotorControl_t* motor,signed long setPoint){
	GPIO_PinState encoderA_status;
	GPIO_PinState encoderB_status;

	motor->setPoint = setPoint;

	while (1){
		// Faz a leitura do estado do enconder - 00,01,10,11
		encoderA_status =  HAL_GPIO_ReadPin(motor->encoderA_port, motor->encoderA_pin);
		encoderB_status =  HAL_GPIO_ReadPin(motor->encoderB_port, motor->encoderB_pin);

		// Caso 00: Os dois sensores, A e B, do encoder detectaram uma fita preta
		if(encoderA_status == GPIO_PIN_RESET && encoderB_status == GPIO_PIN_RESET){

			// Se o estado antigo é 3, incrementa a posição atual.
			// Se o estado antigo é 1, decrementa a posição atual
			if(motor->stepStatusOld == 3){
				motor->actualPoint++;
			}else if(motor->stepStatusOld == 1){
				motor->actualPoint--;
			}

			//Seta o estado antigo para 0
			motor->stepStatusOld = 0;
		}

		// Caso 10: sensor A do enconder, não detecta a fita preta, sensor B do encoder detecta a fita preta
		if(encoderA_status == GPIO_PIN_SET && encoderB_status == GPIO_PIN_RESET){
			// Se o estado antigo é 0, incrementa a posição atual
			// Se o estado antigo é 2, decrementa a posição atual
			if(motor->stepStatusOld == 0){
				motor->actualPoint++;
			}else if(motor->stepStatusOld == 2){
				motor->actualPoint--;
			}

			//Seta o estado antigo para 1
			motor->stepStatusOld = 1;
		}

		// Caso 11: Os dois sensores, A e B, do encoder não detectaram uma fita preta
		if(encoderA_status == GPIO_PIN_SET && encoderB_status == GPIO_PIN_SET){
			// Se o estado antigo é 1, incrementa a posição atual
			// Se o estado antigo é 3, decrementa a posição atual
			if(motor->stepStatusOld == 1){
				motor->actualPoint++;
			}else if(motor->stepStatusOld == 3){
				motor->actualPoint--;
			}

			//Seta o estado antigo para 2
			motor->stepStatusOld = 2;
		}

		// Caso 01: sensor A do enconder, detecta a fita preta, sensor B do encoder não detecta a fita preta
		if(encoderA_status == GPIO_PIN_RESET && encoderB_status == GPIO_PIN_SET){
			// Se o estado antigo é 2, incrementa a posição atual
			// Se o estado antigo é 3, decrementa a posição atual
			if(motor->stepStatusOld == 2){
				motor->actualPoint++;
			}else if(motor->stepStatusOld == 3){
				motor->actualPoint--;
			}

			//Seta o estado antigo para 3
			motor->stepStatusOld = 3;
		}

		// Cálculo PWM - Quando está próximo do destino, diminui a velocidade do motor
		motor->dutyCycle = myABS((double)(motor->setPoint - motor->actualPoint)) * (double)P_FRACTION;

		if(motor->dutyCycle < MIN_DUTYCYCLE){
			motor->dutyCycle = MIN_DUTYCYCLE;
		}
		if(motor->dutyCycle > MAX_DUTYCYCLE){
			motor->dutyCycle = MAX_DUTYCYCLE;
		}

		if(motor->dutyCycle < MIN_DUTYCYCLE){
			motor->dutyCycle = MIN_DUTYCYCLE;
		}
		if(motor->dutyCycle > MAX_DUTYCYCLE){
			motor->dutyCycle = MAX_DUTYCYCLE;
		}

		// Caso a diferença do ponto atual e o ponto de destino esteja dentro da margem de erro, desabilita o motor
		if(myABS((double)(motor->setPoint - motor->actualPoint)) < (double)STEP_MARGIN){

			// Desliga o motor pras duas direções
			__HAL_TIM_SET_COMPARE(&motor->timer_left, motor->channel_left, 0);
			__HAL_TIM_SET_COMPARE(&motor->timer_right, motor->channel_right, 0);
			return ;
		}
		else{

			// Caso o ponto de destino seja maior que o ponto atual, liga o motor para uma direção
			if(motor->actualPoint < motor->setPoint){

				// Gira em uma direção
				__HAL_TIM_SET_COMPARE(&motor->timer_left, motor->channel_left, 255 - motor->dutyCycle);
				__HAL_TIM_SET_COMPARE(&motor->timer_right, motor->channel_right, 0);
			}

			// Caso o ponto de destino seja menor que o ponto atual, liga o mtor para a outra direção
			if(motor->actualPoint > motor->setPoint){

				// Gira na outra direção
				__HAL_TIM_SET_COMPARE(&motor->timer_left, motor->channel_left, 0);
				__HAL_TIM_SET_COMPARE(&motor->timer_right, motor->channel_right, 255 - motor->dutyCycle);
			}
		}

	}

}

void motorSimpleBegin(MotorControl_Simple_t* motor, GPIO_TypeDef* a_port, uint16_t a_pin, GPIO_TypeDef* b_port, uint16_t b_pin){
	motor->A_PORT = a_port;
	motor->A_PIN = a_pin;

	motor->B_PORT = b_port;
	motor->B_PIN = b_pin;
}
//Faz o motor da pin�a ir pra frente
void motorForward(MotorControl_Simple_t* motor, uint32_t time){
	HAL_GPIO_WritePin(motor->A_PORT, motor->A_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(motor->B_PORT, motor->B_PIN, GPIO_PIN_RESET);
	HAL_Delay(time);

	HAL_GPIO_WritePin(motor->A_PORT, motor->A_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->B_PORT, motor->B_PIN, GPIO_PIN_RESET);

}
//Faz o motor da pin�a voltar para tr�s
void  motorBackward(MotorControl_Simple_t* motor, uint32_t time){
	HAL_GPIO_WritePin(motor->A_PORT, motor->A_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->B_PORT, motor->B_PIN, GPIO_PIN_SET);
	HAL_Delay(time);

	HAL_GPIO_WritePin(motor->A_PORT, motor->A_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->B_PORT, motor->B_PIN, GPIO_PIN_RESET);

}
//Faz o motor da pin�a furar
/*void pierce(MotorControl_Simple_t* motor, uint32_t time){
	motorForward(motor, time);
	motorBackward(motor, time);
}*/
