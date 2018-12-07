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

#define DELTA_COL_LIN 1021
#define DELTA_CHAR_H 1016
#define DELTA_CHAR_V 1027

#define MAX_POINT 1096

#define P_FRACTION 1.0     //Proportional factor of control loop 0.001 - 10.0 (1.0)
#define STEP_MARGIN 10     //10 - 1000 (1)

#define MIN_DUTYCYCLE 100   //0 - 255 (125)
#define MAX_DUTYCYCLE 127  //0 - 255 (255)


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

void initMotor(MotorControl_t* motor, GPIO_TypeDef* encoderA_port, uint16_t encoderA_pin, GPIO_TypeDef* encoderB_port, uint16_t encoderB_pin, TIM_HandleTypeDef timer_left, TIM_HandleTypeDef timer_right, uint16_t channel_left, uint16_t channel_right);
void atualizarEixo(MotorControl_t* motor,signed long setPoint);
double myABS(double num1);

#endif /* DCMOTORS_H_ */
