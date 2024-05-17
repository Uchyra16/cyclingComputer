#include "hall_sensor.h"

volatile uint32_t old_time = 0;
volatile uint32_t current_time = 0;
volatile uint32_t period = 0;
volatile float speed = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == HALL_SENSOR_Pin) {
		//Zapisz aktualny czas
		current_time = HAL_GetTick();
		//Oblicz czas pomiędzy wykryciami magnesu (w ms)
		period = current_time - old_time;
		old_time = current_time;
		__HAL_TIM_SET_COUNTER(&htim6, 0);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim6) {
		//Wyzeruj predkosc
		current_time = 0;
		old_time = 0;
		period = 0;
	}
}

uint32_t speedValue(uint32_t period, uint8_t wheelSize_inch)
{
	float wheelSize_m = (float)wheelSize_inch * 0.0254;
	float radius_m = wheelSize_m / 2;
	float period_s = 0.001 * (float)period;

	if(period > 0)
	speed = (2.0*3.14*radius_m)/period_s;
	else speed = 0;

	return speed;
}
