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

#define SENSOR_1_A_PORT           GPIOF
#define SENSOR_1_A_PIN            GPIO_PIN_0

#define SENSOR_1_B_PORT           GPIOF
#define SENSOR_1_B_PIN            GPIO_PIN_1

#define SENSOR_2_A_PORT           GPIOA
#define SENSOR_2_A_PIN            GPIO_PIN_2

#define SENSOR_2_B_PORT           GPIOA
#define SENSOR_2_B_PIN            GPIO_PIN_3

#define DELTA_COL_LIN 1021
#define DELTA_CHAR_H 1016
#define DELTA_CHAR_V 1027

#define MAX_POINT 1096

#define P_FRACTION 1.0     //Proportional factor of control loop 0.001 - 10.0 (1.0)
#define STEP_MARGIN 10     //10 - 1000 (1)

#define MIN_DUTYCYCLE 100   //0 - 255 (125)
#define MAX_DUTYCYCLE 127  //0 - 255 (255)

int dutyCycle;

signed long setPoint_1;
signed long setPoint_2;

signed long actualPoint_1;
signed long actualPoint_2;

int stepStatusOld_1;
int stepStatusOld_2;

GPIO_PinState sensorStatus_1_A;
GPIO_PinState sensorStatus_1_B;

GPIO_PinState sensorStatus_2_A;
GPIO_PinState sensorStatus_2_B;

void initMotors();
void atualizarEixoX();
void atualizarEixoY();
double myABS(double num1);

#endif /* DCMOTORS_H_ */
