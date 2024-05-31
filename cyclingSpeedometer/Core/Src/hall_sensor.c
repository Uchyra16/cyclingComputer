#include "hall_sensor.h"


float speed = 0;


float speedValue(uint32_t period, uint8_t wheelSize_inch)
{
	float wheelSize_m = (float)wheelSize_inch * 0.0254;
	float radius_m = wheelSize_m / 2;
	float period_s = 0.001 * (float)period;

	if(period > 0)
	speed = (2.0*3.14*radius_m)/period_s;
	else speed = 0.0;

	return speed*3600/1000;
}

float distanceCovered(uint32_t hallCounter, uint8_t wheelSize_inch)
{
	float wheelSize_m = (float)wheelSize_inch * 0.0254;
	float radius_m = wheelSize_m / 2;
	float distance_m = 2 * 3.14 * radius_m;

	return (float)hallCounter*(float)distance_m/1000;
}



