#include "hcsr04.h"
#include "dwt_stm32_delay.h"

extern UART_HandleTypeDef huart2;
/**
 * @brief Initializes HC-SR04 sensor
 * @param *hcsr04: pointer to empty structure to save initialization
 * @param *echo_port: pointer to GPIOx Port for Echo pin
 * @param echo_pin: GPIO Pin for Echo pin
 * @param *trig_port: pointer to GPIOx Port for Trigger pin
 * @param trig_pin: GPIO Pin for Trigger pin
 * @retval HC-SR04 status:
 * 				- 0: Device not detected
 * 				- >0: Device is ready to use
 */
uint8_t HCSR04_Init(HCSR04* hcsr04, GPIO_TypeDef* echo_port, uint16_t echo_pin, GPIO_TypeDef* trig_port, uint16_t trig_pin){
	//Inizializzo la libreria per delay in us
	HAL_UART_Transmit(&huart2,"Initializing DWT...\r\n",sizeof(char)*strlen("Initializing DWT...\r\n"),HAL_MAX_DELAY);
	DWT_Delay_Init();
	HAL_UART_Transmit(&huart2,"...Done\r\n",sizeof(char)*strlen("...Done\r\n"),HAL_MAX_DELAY);

	HAL_UART_Transmit(&huart2,"Initializing HCSR04...\r\n",sizeof(char)*strlen("Initializing HCSR04...\r\n"),HAL_MAX_DELAY);
	hcsr04->echo_pin = echo_pin;
	hcsr04->echo_port = echo_port;
	hcsr04->trig_pin = trig_pin;
	hcsr04->trig_port = trig_port;
	HAL_UART_Transmit(&huart2,"...Done\r\n",sizeof(char)*strlen("...Done\r\n"),HAL_MAX_DELAY);
	if(HCSR04_Read(hcsr04)>=0)
		return 1;
	return 0;
}

/**
 * @brief Starts sensor measurement and read data
 * @param *hcsr04: pointer to structure to save initialization data
 * @retval float: distance
 * 				- >0: Valid distance in cm (centimeters)
 * 				- -1: Error
 */
float HCSR04_Read(HCSR04* hcsr04){
	uint32_t local_time = 0;
	uint32_t timeout = HCSR04_TIMEOUT;
	float speed = 0.0332*4/3;	// 0.0332 velocità della luce (cm/us) e 4/3 fattore proporzionale (può essere cambiato);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
	DWT_Delay_us(2);	//wait 2 us

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
	DWT_Delay_us(10);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);

	//Wait a response
	while(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_14)){
		if(timeout-- == 0x00)
			return -1;
	}

	//Measuring distance
	while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_14)){
		local_time++;
		DWT_Delay_us(1);
	}
	hcsr04->distance = local_time*speed;
	return hcsr04->distance;
}

/**
 * @brief Gets the distance value in hcsr04 structure
 * @param *hcsr04: pointer to structure to get data
 * @retval float: distance
 * 				- >0: Valid distance in cm (centimeters)
 * 				- -1: Error
 */
int HCSR04_GetDistance(HCSR04* hcsr04){
	return hcsr04->distance;
}
