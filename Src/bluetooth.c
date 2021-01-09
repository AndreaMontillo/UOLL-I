/*
 * bluetooth.c
 *
 *  Created on: 21 giu 2019
 *      Author: franc
 */
#include "utilities.h"
#include "usart.h"

/**
 * @brief Callback function for bluetooth module
 * @param UART_HandleTypeDef* huart: STM32 UART handler
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  	if(HAL_UART_GetState(&huart6) != HAL_UART_STATE_BUSY){
  		msg[0] = huart->pRxBuffPtr[0];
  		printOnDisplay("Comando Ricevuto",0);
  	    if(msg[0] == 'e'){ //Emergency
  	    	flagSELECT = 1;
  	    }
  	}else {
  		printOnDisplay("Error...",0);
  		printOnDisplay("try again", 1);
  		msg[0] = ' ';
	}
  	msg_prev[0] = msg[0];
}

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
//	if(huart->gState == HAL_UART_STATE_TIMEOUT){
//		printOnDisplay("Errore col bluetooth",0);
//		flagIDWG = 1;
//	}
//	else if(huart->pTxBuffPtr == 'a'){
//		flagIDWG = 0;
//	}
//}
