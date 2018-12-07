void initMotors(){
	dutyCycle = 0;

	setPoint_1 = 0;
	setPoint_2 = 0;

	actualPoint_1 = 0;
	actualPoint_2 = 0;

	stepStatusOld_1 = 0;
	stepStatusOld_2 = 0;

	sensorStatus_1_A = GPIO_PIN_RESET;
	sensorStatus_1_B = GPIO_PIN_RESET;

	sensorStatus_2_A = GPIO_PIN_RESET;
	sensorStatus_2_B = GPIO_PIN_RESET;

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

	HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

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
}

double myABS(double num1){
	if(num1 < 0){
		num1 += -1;
	}

	return num1;
}


void atualizarEixoX(){

	 // Faz a leitura do estado do enconder - 00,01,10,11
	 sensorStatus_1_A =  HAL_GPIO_ReadPin(SENSOR_1_A_PORT, SENSOR_1_A_PIN);
	 sensorStatus_1_B =  HAL_GPIO_ReadPin(SENSOR_1_B_PORT, SENSOR_1_B_PIN);

	 // Caso 00: Os dois sensores, A e B, do encoder detectaram uma fita preta
	  if(sensorStatus_1_A == GPIO_PIN_RESET && sensorStatus_1_B == GPIO_PIN_RESET){
		  // Se o estado antigo é 3, incrementa a posição atual.
		  // Se o estado antigo é 1, decrementa a posição atual
		  if(stepStatusOld_1 == 3){
			  actualPoint_1++;
		  }else if(stepStatusOld_1 == 1){
			  actualPoint_1--;
		  }

		  //Seta o estado antigo para 0
		  stepStatusOld_1 = 0;
	  }

	  // Caso 10: sensor A do enconder, não detecta a fita preta, sensor B do encoder detecta a fita preta
	  if(sensorStatus_1_A == GPIO_PIN_SET && sensorStatus_1_B == GPIO_PIN_RESET){
		  // Se o estado antigo é 0, incrementa a posição atual
		  // Se o estado antigo é 2, decrementa a posição atual
		  if(stepStatusOld_1 == 0){
			  actualPoint_1++;
		  }else if(stepStatusOld_1 == 2){
			  actualPoint_1--;
		  }

		  //Seta o estado antigo para 1
		  stepStatusOld_1 = 1;
	  }

	  // Caso 11: Os dois sensores, A e B, do encoder não detectaram uma fita preta
	  if(sensorStatus_1_A == GPIO_PIN_SET && sensorStatus_1_B == GPIO_PIN_SET){
		  // Se o estado antigo é 1, incrementa a posição atual
		  // Se o estado antigo é 3, decrementa a posição atual
		  if(stepStatusOld_1 == 1){
			  actualPoint_1++;
		  }else if(stepStatusOld_1 == 3){
			  actualPoint_1--;
		  }

		  //Seta o estado antigo para 2
		  stepStatusOld_1 = 2;
	  }

	  // Caso 01: sensor A do enconder, detecta a fita preta, sensor B do encoder não detecta a fita preta
	  if(sensorStatus_1_A == GPIO_PIN_RESET && sensorStatus_1_B == GPIO_PIN_SET){
		  // Se o estado antigo é 2, incrementa a posição atual
		  // Se o estado antigo é 3, decrementa a posição atual
		  if(stepStatusOld_1 == 2){
			  actualPoint_1++;
		  }else if(stepStatusOld_1 == 3){
			  actualPoint_1--;
		  }

		  //Seta o estado antigo para 3
		  stepStatusOld_1 = 3;
	  }

	  // Cálculo PWM - Quando está do destino, diminui a velocidade do motor

	  dutyCycle = myABS((double)(setPoint_1 - actualPoint_1)) * (double)P_FRACTION;

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

	  // Caso a diferença do ponto atual e o ponto de destino esteja dentro da margem de erro, desabilita o motor
	  if(myABS((double)(setPoint_1 - actualPoint_1)) < (double)STEP_MARGIN){
			// Desliga o motor pras duas direções
			__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
			isDone = 1;
	  }
	  else{
		// Caso o ponto de destino seja maior que o ponto atual, liga o motor para uma direção
		if(actualPoint_1 < setPoint_1){
			// Gira em uma direção
			__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 255 - dutyCycle);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
		}

		// Caso o ponto de destino seja menor que o ponto atual, liga o mtor para a outra direção
		if(actualPoint_1 > setPoint_1){
			// Gira na outra direção
			__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 255 - dutyCycle);
		}
	  }
}

void atualizarEixoY(){
	 // Faz a leitura do estado do enconder - 00,01,10,11
	 sensorStatus_2_A =  HAL_GPIO_ReadPin(SENSOR_2_A_PORT, SENSOR_2_A_PIN);
	 sensorStatus_2_B =  HAL_GPIO_ReadPin(SENSOR_2_B_PORT, SENSOR_2_B_PIN);

	 // Caso 00: Os dois sensores, A e B, do encoder detectaram uma fita preta
	 if(sensorStatus_2_A == GPIO_PIN_RESET && sensorStatus_2_B == GPIO_PIN_RESET){
		 // Se o estado antigo é 3, incrementa a posição atual.
		 // Se o estado antigo é 1, decrementa a posição atual
		 if(stepStatusOld_2 == 3){
			  actualPoint_2++;
		  }else if(stepStatusOld_2 == 1){
			  actualPoint_2--;
		  }

		 //Seta o estado antigo para 0
		  stepStatusOld_2 = 0;
	  }

	 // Caso 10: sensor A do enconder, não detecta a fita preta, sensor B do encoder detecta a fita preta
	 if(sensorStatus_2_A == GPIO_PIN_SET && sensorStatus_2_B == GPIO_PIN_RESET){
		 // Se o estado antigo é 0, incrementa a posição atual
		 // Se o estado antigo é 2, decrementa a posição atual

		if(stepStatusOld_2 == 0){
		  actualPoint_2++;
		}else if(stepStatusOld_2 == 2){
		  actualPoint_2--;
		}

		//Seta o estado antigo para 1
		stepStatusOld_2 = 1;
	  }
	 // Caso 11: Os dois sensores, A e B, do encoder não detectaram uma fita preta
	 if(sensorStatus_2_A == GPIO_PIN_SET && sensorStatus_2_B == GPIO_PIN_SET){
		 // Se o estado antigo é 1, incrementa a posição atual
		 // Se o estado antigo é 3, decrementa a posição atual
		 if(stepStatusOld_2 == 1){
			  actualPoint_2++;
		  }else if(stepStatusOld_2 == 3){
			  actualPoint_2--;
		  }

		 //Seta o estado antigo para 2
		 stepStatusOld_2 = 2;
	  }

	 // Caso 01: sensor A do enconder, detecta a fita preta, sensor B do encoder não detecta a fita preta
	 if(sensorStatus_2_A == GPIO_PIN_RESET && sensorStatus_2_B == GPIO_PIN_SET){
		 // Se o estado antigo é 2, incrementa a posição atual
		 // Se o estado antigo é 3, decrementa a posição atual
		if(stepStatusOld_2 == 2){
		  actualPoint_2++;
		}else if(stepStatusOld_2 == 3){
		  actualPoint_2--;
		}

		 //Seta o estado antigo para 3
		 stepStatusOld_2 = 3;
	  }

	 // Cálculo PWM - Quando está do destino, diminui a velocidade do motor
	  dutyCycle = myABS((double)(setPoint_1 - actualPoint_1)) * (double) P_FRACTION;

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

	  // Caso a diferença do ponto atual e o ponto de destino esteja dentro da margem de erro, desabilita o motor
	  if(myABS((double)(setPoint_1 - actualPoint_1)) < (double)STEP_MARGIN){
		// Caso o ponto de destino seja maior que o ponto atual, liga o motor para uma direção
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
		isDone = 1;
	  }
	  else{
		// Caso o ponto de destino seja maior que o ponto atual, liga o motor para uma direção
		if(actualPoint_2 < setPoint_2){
			// Gira em uma direção
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 255 - dutyCycle);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
		}

		// Caso o ponto de destino seja menor que o ponto atual, liga o mtor para a outra direção
		if(actualPoint_2 > setPoint_2){
			// Gira na outra direção
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 255 - dutyCycle);
		}
	  }
}

