#pragma once

#include "tim.h"

extern volatile uint32_t old_time;
extern volatile uint32_t current_time;
extern volatile uint32_t period;
extern volatile float speed;

//Interrupt responsible for:
//detection period calculations
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);


//Timer interrupt responsible for:
//reseting the counter if 5 sec passed without hall_sensor detection
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

//Fucntion responsible for:
//speed calculations based on radius and detection period
uint32_t speedValue(uint32_t period, uint8_t wheelSize_inch);