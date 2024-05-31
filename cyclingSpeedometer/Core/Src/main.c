/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <wchar.h>

#include "hall_sensor.h"
#include "lps25hb.h"
#include "lcd.h"
#include "hagl.h"
#include "font9x18B-KOI8-R.h"




/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int __io_putchar(int ch)
{
	if(ch == '\n') {
		__io_putchar('\r');
	}

	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

	return 1;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi2)
		{
			lcd_transfer_done();
		}
}

volatile uint32_t timer_seconds = 0;
volatile uint32_t old_time = 0;
volatile uint32_t current_time = 0;
volatile uint32_t period = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim6) {
		current_time = 0;
		old_time = 0;
		period = 0;
	}
	if(htim == &htim7){
			timer_seconds++;
		}
}
volatile uint32_t hallCounter = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == HALL_SENSOR_Pin) {
		//Save time
		current_time = HAL_GetTick();
		//Count the period
		period = current_time - old_time;
		old_time = current_time;
		__HAL_TIM_SET_COUNTER(&htim6, 0);
		hallCounter++;
	}
}

void convert_time(uint32_t timer_seconds, uint32_t *hours, uint32_t *minutes, uint32_t *seconds)
{
	*seconds = timer_seconds % 60;
	*minutes = (timer_seconds/60) % 60;
	*hours = (timer_seconds/3600) % 24;
}

uint8_t wheelSize = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);
  lps25hb_init();
  lcd_init();
  //Variables
  uint8_t counter1 = 0;
  uint8_t counter2 = 0;
  uint8_t counter3 = 0;
  uint8_t whichButton = 0;
  uint32_t hours,minutes,seconds;
  float distance = 0;
  float heightDifference = 0;
  float h = 0;
  float p0 = lps25hb_read_pressure();

  while (1)
  {

	float temp = lps25hb_read_temp() + 273.15;
	float p = lps25hb_read_pressure();
	float temperature_C = lps25hb_read_temp();
	wheelSize = counter3;
	speed = speedValue(period, wheelSize);


	//DISPLAY PART

	hagl_clear_screen();

	wchar_t speed_display[16];
	wchar_t h_display[16];
	wchar_t temperature_display[16];
	wchar_t timer_display[16];
	wchar_t distance_display[16];
	wchar_t heightDiff_display[16];
	wchar_t wheelSize_display[16];

	swprintf(heightDiff_display, 16, L"%d m", (uint16_t)heightDifference);
	swprintf(temperature_display,16,L"%d°C", (uint8_t)temperature_C);
	swprintf(h_display,16, L"%.2f m", h);
	swprintf(speed_display,16, L"%.2f km/h", speed);
	swprintf(timer_display,16, L"%02lu:%02lu:%02lu", hours,minutes,seconds);
	swprintf(distance_display,16,L"%.2f km",(float)distance);
	swprintf(wheelSize_display,16,L"%Wheel Size: %d \n  (inch)", wheelSize);




	//BUTTON PART
	 if(HAL_GPIO_ReadPin(L_BUTTON_GPIO_Port, L_BUTTON_Pin) == GPIO_PIN_RESET) {
		 counter1++;
		 whichButton = 1;
		 timer_seconds = 0;
		 if(counter1 > 4){
			 counter1 = 0;
		 }
		 while(HAL_GPIO_ReadPin(L_BUTTON_GPIO_Port,L_BUTTON_Pin) == GPIO_PIN_RESET);
	 }
	 if(HAL_GPIO_ReadPin(M_BUTTON_GPIO_Port, M_BUTTON_Pin) == GPIO_PIN_RESET) {
		 counter2++;
		 whichButton = 2;
		 if(counter2 > 2) {
			 counter2 = 0;
		 }
		 while(HAL_GPIO_ReadPin(M_BUTTON_GPIO_Port,M_BUTTON_Pin) == GPIO_PIN_RESET);
	 }
	 if(HAL_GPIO_ReadPin(R_BUTTON_GPIO_Port, R_BUTTON_Pin) == GPIO_PIN_RESET) {
		 counter3++;
		 if(counter3 > 30){
			 counter3 = 0;
		 }
		 while(HAL_GPIO_ReadPin(R_BUTTON_GPIO_Port,R_BUTTON_Pin) == GPIO_PIN_RESET);
	 }

	 //INTERFACE PART
	 switch(whichButton) {
	 //1.Trip mode
	 case 1:
		 	if(counter1 == 1){
		 		hagl_put_text(L"   Press again \n  to start trip",0,45,WHITE,font9x18B_KOI8_R);
		 		lcd_copy();
		 	}
		 	else if(counter1 == 2){
				//TRIP MODE - displaying speed, distance and trip time
				distance = distanceCovered(hallCounter, 28);
				convert_time(timer_seconds, &hours, &minutes, &seconds);
				h = -29.271769 * temp * log(p / p0);
				heightDifference = heightDiff(h);
				//Display
				hagl_put_text(speed_display,40,55,WHITE, font9x18B_KOI8_R);
				hagl_put_text(distance_display,40,25,WHITE,font9x18B_KOI8_R);
				hagl_put_text(temperature_display, 0, 0, WHITE, font9x18B_KOI8_R);
				hagl_put_text(timer_display,40,85,WHITE,font9x18B_KOI8_R);
				lcd_copy();
		 	}
		 	else if(counter1 == 3){
		 		//TRIP RESULTS MODE - displaying total distance, time and hDiff
		 		hagl_put_text(L"Trip finished!",0,0,WHITE,font9x18B_KOI8_R);
		 		hagl_put_text(L"Dist: ",0,25,WHITE,font9x18B_KOI8_R);
		 		hagl_put_text(distance_display,70,25,WHITE,font9x18B_KOI8_R);
		 		hagl_put_text(L"Time: ",0,45,WHITE,font9x18B_KOI8_R);
		 		hagl_put_text(timer_display,60,45,WHITE,font9x18B_KOI8_R);
		 		hagl_put_text(L"H diff: ",0,65,WHITE,font9x18B_KOI8_R);
		 		hagl_put_text(heightDiff_display, 100,65,WHITE,font9x18B_KOI8_R);
		 		lcd_copy();
		 	}
		 	else if(counter1 == 4) {
		 		whichButton = 0;
		 		hallCounter = 0;
		 	}
			break;
	//2. Wheel size config mode
	 case 2:
		 	if(counter2 == 1) {
				hagl_put_text(wheelSize_display, 0,40,WHITE,font9x18B_KOI8_R);
				hagl_put_text(L"2.Accept",0,60,WHITE,font9x18B_KOI8_R);
				hagl_put_text(L"3.Change size ",0,80,WHITE,font9x18B_KOI8_R);
				lcd_copy();
		 	}
		 	else if(counter2 == 2) {
		 		whichButton = 0;
		 	}
		 	break;
	//Default - menu
	 default:
		 	hagl_put_text(L"Mode:\n 1.Trip mode\n 2.Wheel size",10,25,WHITE,font9x18B_KOI8_R);
		 	lcd_copy();
			break;
	 }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
