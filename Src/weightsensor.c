#include "weightsensor.h"

/**
 * @brief Initialize the weight sensor
 * @param GPIO_TypeDef* gpioSck: the struct of the clock source pin
 * @param GPIO_TypeDef* gpioData: the struct of the data pin
 * @param uint16_t pinSck: the clock source port identifier
 * @param uint16_t pinData: the data source port identifier
 * @retval HX711: the struct of the weight sensor
 */
HX711 hx711Init(GPIO_TypeDef* gpioSck,GPIO_TypeDef* gpioData,uint16_t pinSck, uint16_t pinData){
	HX711 data;
	data.gpioSck = gpioSck;
	data.gpioData = gpioData;
	data.pinSck = pinSck;
	data.pinData = pinData;
	data.gain = 1;
	return data;
}

/**
 * @brief Switch on the weight sensor, to do this it must keep the GPIO_PIN_SET signal for 50 milliseconds
 * 		  and set the data.gpioSck at GPIO_PIN_RESET
 * @param HX711 data: the struct of the weight sensor
 */
void hx711PowerUp(HX711 data){
	HAL_GPIO_WritePin(data.gpioSck,data.pinSck,GPIO_PIN_SET); // set data.gpioSck at GPIO_PIN_SET
	HAL_Delay(50); // keeps the signal high for 50 milliseconds
	HAL_GPIO_WritePin(data.gpioSck,data.pinSck,GPIO_PIN_RESET); // set data.gpioSck at GPIO_PIN_RESET
}

/**
 * @brief Switch off the weight sensor, to do this PD_SCK pin changes from low to high and stays at
 * 		  GPIO_PIN_SET for longer than 60µs
 * @param HX711 data: the struct of the weight sensor
 */
void hx711PowerDown(HX711 data){
	HAL_GPIO_WritePin(data.gpioSck,data.pinSck,GPIO_PIN_RESET); // set data.gpioSck at GPIO_PIN_RESET
	HAL_GPIO_WritePin(data.gpioSck,data.pinSck,GPIO_PIN_SET); // set data.gpioSck at GPIO_PIN_SET
}

/**
 * @brief Check if the weight sensor is ready for operation
 * @param HX711 data: the struct of the weight sensor
 * @retval int: 1 if data.gpioData is in GPIO_PIN_RESET, 0 if is in GPIO_PIN_SET
 */
int hx711IsReady(HX711 data){
	return HAL_GPIO_ReadPin(data.gpioData,data.pinData)==GPIO_PIN_RESET;
}

/**
 * @brief Gets the units from HX711
 * @param HX711 data: the struct of the weight sensor
 * @param uint8_t t: the number of measures to be taken on which to average
 * @retval unsigned long: the units of the HX711
 */
unsigned long hx711GetUnits(HX711 data, uint8_t t){
	return hx711GetValue(data,t)/data.scale;
}

/**
 * @brief Gets a single weight from the HX711
 * @param HX711 data: the struct of the weight sensor
 * @retval unsigned long: the weight value
 */
unsigned long hx711Value(HX711 data){
	unsigned long buffer = 0;

	// if HX711 is not ready, wait
	while(!hx711IsReady(data)){
		HAL_Delay(1000);
	}

	// perform 24 clock cycles in order to get the datum from the buffer
	for(int i = 0; i<24; i++){
		HAL_GPIO_WritePin(data.gpioSck,data.pinSck,GPIO_PIN_SET);
		buffer = buffer << 1;
		if(HAL_GPIO_ReadPin(data.gpioData,data.pinData)){
			buffer++;
		}
	    HAL_GPIO_WritePin(data.gpioSck,data.pinSck,GPIO_PIN_RESET);
	}

	// perform GAIN clock cycles to end the communication
	for(int i = 0; i < data.gain;i++){
		HAL_GPIO_WritePin(data.gpioSck,data.pinSck,GPIO_PIN_SET);
		HAL_GPIO_WritePin(data.gpioSck,data.pinSck,GPIO_PIN_RESET);
	}

	/*
	 * The output 24 bits of data is in 2’s complement format. When input differential signal goes out of
	 * the 24 bit range, the output data will be saturated at 800000h (MIN) or 7FFFFFh (MAX), until the
	 * input signal comes back to the input range. Thus, a bitwise XOR is performed to check that the buffer
	 * is not saturated.
	 */
	buffer = buffer ^ 0x800000;
	return buffer;
}

/**
 * @brief Gets an average value considering also the tare (data.offset)
 * @param HX711 data: the struct of the weight sensor
 * @param uint8_t t: the number of measures to be taken on which to average
 * @retval unsigned long: the average value
 */
unsigned long hx711GetValue(HX711 data, uint8_t t){
	return -hx711AverageValue(data,t)+data.offset;
}

/**
 * @brief Gets an average value
 * @param HX711 data: the struct of the weight sensor
 * @param uint8_t t: the number of measures to be taken on which to average
 * @retval unsigned long: the average value
 */
unsigned long hx711AverageValue(HX711 data, uint8_t t){
	unsigned long sum = 0;

	for(int i = 0; i < t; i++){
		sum += hx711Value(data);
	}
	return sum/t;
}

/**
 * @brief Gets the tare value and save it into the struct
 * @param HX711 data: the struct of the weight sensor
 * @param uint8_t t: the number of measures to be taken on which to average
 * @retval HX711: the modified struct of the weight sensor
 */
HX711 hx711Tare(HX711 data,uint8_t t){
	unsigned long sum = hx711AverageValue(data,t);
	data.offset = sum;
	return data;
}

/**
 * @brief Set the weight scale
 * @param HX711 data: the struct of the weight sensor
 * @param float scale: the weight scale
 * @retval HX711: the modified struct of the weight sensor
 */
HX711 HX711SetScale(HX711 data,float scale){
	data.scale = scale;
	return data;
}

/**
 * @brief Get the weight scale
 * @param HX711 data: the struct of the weight sensor
 * @retval float: the weight scale
 */
float HX711GetScale(HX711 data){
	return data.scale;
}

/**
 * @brief Get the tare
 * @param HX711 data: the struct of the weight sensor
 * @retval unsigned long: the tare
 */
unsigned long HX711GetOffset(HX711 data){
	return data.offset;
}
