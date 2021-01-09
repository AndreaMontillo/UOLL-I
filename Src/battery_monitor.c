#include "battery_monitor.h"
#include "usart.h"

/**
 * @brief Initializes the ADC in DMA Mode
 * @param hadc1: ADC_HandleTypeDef* STM32 ADC handler
 * @param AnalogValue: uint32_t value read from the STM32 ADC
 */
void ADCInit(ADC_HandleTypeDef* hadc1, uint32_t AnalogValue){
	HAL_UART_Transmit(&huart2,"Starting ADC\r\n",strlen("Starting ADC\r\n"),HAL_MAX_DELAY);
	HAL_ADC_Start_DMA(hadc1,AnalogValue,1);
}

/**
 * @brief Evaluates the average battery status given nValues of measurements to do
 * @param ADC_HandleTypeDef* hadc1: STM32 ADC handler
 * @param integer nValues: number of values to read from ADC
 * @retval float avg_val: average value computed
 */
float ADCBatteryValue(ADC_HandleTypeDef* hadc1, int nValues){

	float avg_val = 0.0;
	int i = 0;
	float v_battery;
	float v_value;
	uint32_t AnalogValue;
	char val[30];

	while(i<nValues){
		if(__HAL_ADC_GET_FLAG(hadc1, ADC_FLAG_EOC)){

			//Read ADC Register
			AnalogValue = HAL_ADC_GetValue(hadc1);
			sprintf(val,"Analog Value: %d\r\n",AnalogValue);
			HAL_UART_Transmit(&huart2,val,strlen(val),HAL_MAX_DELAY);

			//Vin to ADC pin
			v_value = (VREF*AnalogValue)/(RESOLUTION);
			sprintf(val,"Input Value: %0.2f\r\n",v_value);
			HAL_UART_Transmit(&huart2,val,strlen(val),HAL_MAX_DELAY);

			/*
			 * il valore 0.0125 è un range di errore di misura che aggiungiamo al v_value per avere una
			 * stampa quanto più possibile vicino al reale valore di v_battery
			 */
			//Effective Voltage of the battery
			v_battery = (v_value + 0.0125)*(R1+R2)/R2;
			sprintf(val,"Voltage Value: %0.2f\r\n",v_battery);
			HAL_UART_Transmit(&huart2,val,strlen(val),HAL_MAX_DELAY);
		}
		//Compute the average of the measurements
		avg_val = avg_val + (v_battery/nValues);
		i++;
	}
	return avg_val;
}

/**
 * @brief Stops the ADC in DMA Mode
 * @param ADC_HandleTypeDef* hadc1: STM32 ADC handler
 */
void ADCStop(ADC_HandleTypeDef* hadc1){
	HAL_UART_Transmit(&huart2,"Stopping ADC\r\n",strlen("Stopping ADC\r\n"),HAL_MAX_DELAY);
	HAL_ADC_Stop_DMA(hadc1);
}
