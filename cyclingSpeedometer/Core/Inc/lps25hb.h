#pragma once
#include "stm32l4xx.h"

extern volatile float minimum_h;
extern volatile float maximum_h;

// LPS25HB Initialization
// Wake up and auto measurement
// return - HAL_OK or HAL_ERROR
HAL_StatusTypeDef lps25hb_init(void);

// Read temperature
// return - temperature in C
float lps25hb_read_temp(void);

// Read pressure
// reuturn - pressure in hPa
float lps25hb_read_pressure(void);

// Sensor calibration
// value - 16-bit correction
void lps25hb_set_calib(uint16_t value);

//Trip mode height differnce calculation
float heightDiff(float h);

