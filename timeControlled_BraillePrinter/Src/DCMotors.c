#include "DCMotors.h"
#include "tim.h"
#include "gpio.h"

void motorBegin(MotorControl_t* motor, GPIO_TypeDef* encoderA_port, uint16_t encoderA_pin, GPIO_TypeDef* encoderB_port, uint16_t encoderB_pin, TIM_HandleTypeDef timer_left, TIM_HandleTypeDef timer_right, uint16_t channel_left, uint16_t channel_right){}

double myABS(double num1){
	if(num1 < 0){
		num1 += -1;
	}

	return num1;
}

void en_updateAxis(MotorControl_t* motor,signed long setPoint){}

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

void updateAxis(MotorControl_Simple_t* motor, uint8_t direction){
	switch (direction){
		case RIGHT:
			motorForward(motor, TIME_HOR);
		break;
		case LEFT:
			motorBackward(motor, TIME_HOR);
		break;
		case DOWN:
			motorForward(motor, TIME_VER);
		break;
		case UP:
			motorBackward(motor, TIME_VER);
		break;
		case NEXT_DOT_RIGHT:
			motorForward(motor, TIME_NEXT_DOT);
		break;
		case NEXT_DOT_LEFT:
			motorBackward(motor, TIME_NEXT_DOT);
		break;
		case DOWN_NEXT_LINE:
			motorForward(motor, TIME_NEXT_DOT);
		break;
		case INIT:
			motorBackward(motor, TIME_INIT);
	}
}
