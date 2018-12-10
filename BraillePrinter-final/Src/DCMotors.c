#include "DCMotors.h"
#include "tim.h"
#include "gpio.h"

void motorBegin(MotorControl_t* motor, GPIO_TypeDef* encoder_CE_port, uint16_t encoder_CE_pin, TIM_HandleTypeDef timer_left, TIM_HandleTypeDef timer_right, uint16_t channel_left, uint16_t channel_right){
	motor->dutyCycle = 0;

	__HAL_TIM_SET_COUNTER(&htim3,28000);
	motor->actualPoint = __HAL_TIM_GET_COUNTER(&htim3);
	motor->setPoint = motor->actualPoint;

	motor->stepStatusOld = 0;

	motor->encoder_CE_port = encoder_CE_port;
	motor->encoder_CE_pin = encoder_CE_pin;

	HAL_GPIO_WritePin(motor->encoder_CE_port, motor->encoder_CE_pin, GPIO_PIN_RESET);

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
		num1 *= -1;
	}

	return num1;
}


void updateAxis(MotorControl_t* motor,signed long setPoint){

	//Ativa encoder
	HAL_GPIO_WritePin(motor->encoder_CE_port, motor->encoder_CE_pin, GPIO_PIN_SET);

	// Atualiza as variaveis que vamos usar
	motor->setPoint = setPoint;
	__HAL_TIM_SET_COUNTER(&htim3,motor->actualPoint);

	// Cálculo PWM - Quando está próximo do destino, diminui a velocidade do motor
	motor->dutyCycle = myABS((double)(motor->setPoint - motor->actualPoint)) * (double)P_FRACTION;

	if(motor->dutyCycle < MIN_DUTYCYCLE){
		motor->dutyCycle = MIN_DUTYCYCLE;
	}else if(motor->dutyCycle > MAX_DUTYCYCLE){
		motor->dutyCycle = MAX_DUTYCYCLE;
	}

	//Verificar se é necesseário adicionar um loop para uma margem de erro
	while (myABS(motor->actualPoint - motor->setPoint) > 5 ){
		if(motor->actualPoint > motor->setPoint){
			// Ativa o motor para a esquerda até chegar na posição correta
			while(motor->actualPoint > motor->setPoint){
				__HAL_TIM_SET_COMPARE(&motor->timer_left, motor->channel_left, 255);
				__HAL_TIM_SET_COMPARE(&motor->timer_right, motor->channel_right, 0);
				motor->actualPoint = __HAL_TIM_GET_COUNTER(&htim3);
			}

		}else if(motor->actualPoint < motor->setPoint){
			// Ativa o motor para a direita até chegar na posição correta
			while(motor->actualPoint < motor->setPoint){
				__HAL_TIM_SET_COMPARE(&motor->timer_left, motor->channel_left, 0);
				__HAL_TIM_SET_COMPARE(&motor->timer_right, motor->channel_right, 255);
				motor->actualPoint = __HAL_TIM_GET_COUNTER(&htim3);
			}
		}

		// Desliga o motor pras duas direções
		__HAL_TIM_SET_COMPARE(&motor->timer_left, motor->channel_left, 0);
		__HAL_TIM_SET_COMPARE(&motor->timer_right, motor->channel_right, 0);

	}

	//Desliga o encoder
	HAL_GPIO_WritePin(motor->encoder_CE_port, motor->encoder_CE_pin, GPIO_PIN_RESET);

	//Atualiza posição atual
	motor->actualPoint = __HAL_TIM_GET_COUNTER(&htim3);

	return ;

}

void motorSimpleBegin(MotorControl_Simple_t* motor, GPIO_TypeDef* a_port, uint16_t a_pin, GPIO_TypeDef* b_port, uint16_t b_pin){
	motor->A_PORT = a_port;
	motor->A_PIN = a_pin;

	motor->B_PORT = b_port;
	motor->B_PIN = b_pin;

	HAL_GPIO_WritePin(motor->A_PORT, motor->A_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->B_PORT, motor->B_PIN, GPIO_PIN_RESET);
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
	HAL_Delay(2*time);

	HAL_GPIO_WritePin(motor->A_PORT, motor->A_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->B_PORT, motor->B_PIN, GPIO_PIN_RESET);

}
//Faz o motor da pin�a furar
void pierce(MotorControl_Simple_t* motor, uint32_t time){
	motorForward(motor, time);
	motorBackward(motor, time);
	HAL_Delay(200);
}
