/*
 * DCMotors.h
 *
 *  Created on: 7 de dez de 2018
 *      Author: esteves
 */

#ifndef DCMOTORS_H_
#define DCMOTORS_H_

#include "DCMotors.h"
#include "stm32f0xx_hal.h"

#define DELTA_COL_LIN 		1021
#define DELTA_CHAR_H 		1016
#define DELTA_CHAR_V 		1027

#define MAX_POINT 			1096

#define P_FRACTION 			1.0     //Proportional factor of control loop 0.001 - 10.0 (1.0)
#define STEP_MARGIN 		10     	//10 - 1000 (1)

#define MIN_DUTYCYCLE 		100   	//0 - 255 (125)
#define MAX_DUTYCYCLE 		127  	//0 - 255 (255)

#define PIERCE_TIME			300		//Tempo para furar
#define TIME_HOR			1500		//Tempo para mover o cabeçote na horizontal
#define TIME_VER			1500		//Tempo para mover o cabeçote na vertical
#define TIME_NEXT_DOT		1500		//Tempo para mover para próxima coluna
#define TIME_INIT			4000	//Tempo para voltar para o início

#define RIGHT				0
#define LEFT				1
#define UP					3
#define DOWN				4
#define NEXT_DOT_RIGHT 		5
#define NEXT_DOT_LEFT		6
#define DOWN_NEXT_LINE		7
#define INIT				8

typedef struct{
	signed long setPoint;
	signed long actualPoint;
	uint8_t stepStatusOld;

	GPIO_TypeDef* encoderA_port;
	uint16_t encoderA_pin;

	GPIO_TypeDef* encoderB_port;
	uint16_t encoderB_pin;

	TIM_HandleTypeDef timer_left;
	TIM_HandleTypeDef timer_right;

	uint16_t channel_left;
	uint16_t channel_right;

	uint8_t dutyCycle;
}MotorControl_t;

typedef struct{
	GPIO_TypeDef* A_PORT;
	uint16_t A_PIN;

	GPIO_TypeDef* B_PORT;
	uint16_t B_PIN;
}MotorControl_Simple_t;

void motorBegin(MotorControl_t* motor, GPIO_TypeDef* encoderA_port, uint16_t encoderA_pin, GPIO_TypeDef* encoderB_port, uint16_t encoderB_pin, TIM_HandleTypeDef timer_left, TIM_HandleTypeDef timer_right, uint16_t channel_left, uint16_t channel_right);
void en_updateAxis(MotorControl_t* motor,signed long setPoint);
void motorBackward(MotorControl_Simple_t* motor, uint32_t time);
void motorForward(MotorControl_Simple_t* motor, uint32_t time);
void motorSimpleBegin(MotorControl_Simple_t* motor, GPIO_TypeDef* a_port, uint16_t a_pin, GPIO_TypeDef* b_port, uint16_t b_pin);
void updateAxis(MotorControl_Simple_t* motor, uint8_t direction);
//void pierce(MotorControl_Simple_t* motor, uint32_t time);
double myABS(double num1);

#endif /* DCMOTORS_H_ */
