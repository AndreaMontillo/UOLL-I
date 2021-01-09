/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "utilities.h"

#include "weightsensor.h"
#include "i2c-lcd.h"
#include "tm_stm32f4_watchdog.h"
#include "servos.h"
#include "track.h"
#include "battery_monitor.h"
#include "hcsr04.h"
/*
 * Per la stampa di float:
 * Project\Properties\C/C++ Build\Settings\MCU GCC Linker\Linker flags
 * inserire -u _printf_float
 */
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
char val[30];
int count_pancia = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void undo(void);
void pickUp(HX711 data, HCSR04* hcsr04);

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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  lcd_init();


  initServo(&s1, &htim1, TIM_CHANNEL_1); //Timer PWM per Pancia
  initServo(&s2, &htim2, TIM_CHANNEL_1); //Timer 2 PWM per gomito sx
  initServo(&s3, &htim3, TIM_CHANNEL_1); //Timer 3 PWM per spalla sx
  initServo(&s4, &htim4, TIM_CHANNEL_1); //Timer 4 PWM per gomito rx
  initServo(&s5, &htim5, TIM_CHANNEL_1); //Timer 5 PWM per spalla rx

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//ENA
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	//ENB

  //Inizializzo la struttura del sensore di peso
  HX711 hx711;
  GPIO_TypeDef* gpioData = GPIOC;
  GPIO_TypeDef* gpioSck = GPIOC;
  uint16_t pinData = GPIO_PIN_4;
  uint16_t pinSck = GPIO_PIN_5;

  hx711 = hx711Init(gpioSck,gpioData,pinSck,pinData);

  hx711PowerUp(hx711);
  HAL_Delay(10);

  printOnDisplay("...Wait...", 0);
  printOnDisplay("Initializing", 1);

  hx711 = hx711Tare(hx711,10);

  printWeight(hx711);

  HAL_Delay(3000);

  printOnDisplay("I'm READY :D",0);

  //.......ATTIVARE SE SI VUOLE IL WATCH-DOG............
  //TM_WATCHDOG_Init(TM_WATCHDOG_Timeout_8s);

//  uint32_t AnalogValue;
//  float batteryVolts;
//
    HCSR04* hcsr04;
//  HCSR04_Init(hcsr04,GPIOA,GPIO_PIN_14,GPIOA,GPIO_PIN_15);
//  HAL_UART_Transmit(&huart2,"Init Ok\r\n",sizeof(char)*strlen("Init Ok\r\n"),HAL_MAX_DELAY);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  flagSELECT = 0;

	  HAL_UART_Receive_DMA(&huart6,msg,sizeof(char));

	  switch(msg[0]){
	  	  case 'u':
	  		printOnDisplay("Avanti",0);
	  		forward();
	  		msg[0] = 'z';
	  		break;
	  	  case 'd':
	  		printOnDisplay("Indietro",0);
	  		backward();
	  		msg[0] = 'z';
	  		break;
	  	  case 'r':
	  		printOnDisplay("Destra",0);
	  		right();
	  		msg[0] = 'z';
	  		break;
	  	  case 'l':
	  		printOnDisplay("Sinistra",0);
	  		left();
	  		msg[0] = 'z';
	  		break;
	  	  case 's':
	  		printOnDisplay("STOP",0);
	  		stop();
	  		msg[0] = 'z';
	  		break;
	  	  case 't':
	  		if(count_pancia == 0){
	  			printOnDisplay("Apro pancia",0);
	  			bellyRotation(115);
	  			count_pancia += 1;
	  		}else{
	  			printOnDisplay("Chiudo pancia",0);
	  			bellyRotation(0);
	  			count_pancia -=1;
	  		}
	  		msg[0] = 'z';
	  		break;
	  	  case 'q':
	  		printOnDisplay("Raccolgo",0);
	  		pickUp(hx711,hcsr04);
	  		msg[0] = 'z';
	  		break;
	  	  case 'c':
	  		printOnDisplay("Proviamoo!",0);
	  		armsMovement();
	  		msg[0] = 'z';
	  		break;
	  	  case 'x': //AGGIORNO PESO
	  		printWeight(hx711);
	  		msg[0] = ' ';
	  		break;
	  	  case 'z':
	  		HAL_Delay(1000);
	  		printOnDisplay("I'm READY :D",0);
	  		msg[0] = ' ';
	  		break;
	  	  case 'e':
			printOnDisplay("Emergency :O",0);
			flagSELECT = 0;
			undo();
			msg[0] = 'z';
			break;

    /* USER CODE BEGIN 3 */
	  	  }
  /* USER CODE END 3 */

	  //Misuro livello batteria
//	  ADCInit(&hadc1,AnalogValue);
//	  batteryVolts = ADCBatteryValue(&hadc1,5);
//	  printOnDisplay("Livello Batteria",0);
//	  sprintf(val,"%0.2f",batteryVolts);
//	  printOnDisplay(val,1);
//	  ADCStop(&hadc1);

	  HAL_Delay(1000);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void undo(void){
	if(msg_prev[0] == 'u' || msg_prev[0] == 'd' || msg_prev[0] == 'r' || msg_prev[0] == 'l') {
		stop();
	}
	else if(msg_prev[0] == 't') {
		if(count_pancia == 0){
			//devo chiudere la pancia
			bellyRotation(0);
		}else{
			//devo aprire la pancia
			bellyRotation(115);
		}
	}
	else if(msg_prev[0] == 'q') {
		//Fermare il cingolato
		stop();

		//riposizionare le braccia
		armsSmoothRotation(15,135,3,3);

		//riposizionare pancia
		if(count_pancia == 0){
			//devo chiudere la pancia
			bellyRotation(0);
		}else{
			//devo aprire la pancia
			bellyRotation(115);
		}
	}
	printOnDisplay("UN-D0", 0);
	HAL_Delay(1000);
}

//DA MODIFICARE in dipendenza della distanza che rileva il sensore ultrasuoni
void pickUp(HX711 data,HCSR04* hcsr04){

	HAL_UART_Transmit(&huart2,"Measuring...\r\n",sizeof(char)*strlen("Measuring...\r\n"),HAL_MAX_DELAY);
	float distance = HCSR04_Read(hcsr04);
	HAL_UART_Transmit(&huart2,"...Done\r\n",sizeof(char)*strlen("...Done\r\n"),HAL_MAX_DELAY);

	if(distance>=3 && distance<=400){
		sprintf(val,"Distance [cm]: %d\r\n",distance);
		HAL_UART_Transmit(&huart2,val,sizeof(char)*strlen(val),HAL_MAX_DELAY);
	}
	else{
		if(distance<3){
			HAL_UART_Transmit(&huart2,"Object Too Close\r\n",sizeof(char)*strlen("Object Too Close\r\n"),HAL_MAX_DELAY);
			backward();
		}else{
			if(distance>400){
				HAL_UART_Transmit(&huart2,"Object Too Far\r\n",sizeof(char)*strlen("Object Too Far\r\n"),HAL_MAX_DELAY);
				forward();
			}
		}
	}


	printOnDisplay("Apro Pancia", 0);
	bellyRotation(115);

	if(flagSELECT == 0) {
		count_pancia = 1;
	}

	//PARTE CON ENCODER
	HAL_Delay(1500);
	stop();
	//------------------

	armsMovement();

	printOnDisplay("Chiudo Pancia", 0);
	bellyRotation(0);
	if(flagSELECT == 0) {
		count_pancia = 0;
	}

	//Misuro Peso
	printWeight(data);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
