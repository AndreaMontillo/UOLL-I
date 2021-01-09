/*
 * lcd.c
 */

#include "i2c-lcd.h"
#include "utilities.h"
extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly

#define SLAVE_ADDRESS_LCD 0x40 // change this according to ur setup

/**
 * @brief Prints a string on the LCD display
 * @param char* string: String to be printed on display
 * @param int flag:
 * 				- 0: print on the first row of the display
 * 				- 1: print on the second row of the display
 */
void printOnDisplay(char* string, int flag){

	if (flag == 0){
		lcd_clear();

		//Allow to print on display
		lcd_send_cmd(0x80);
		lcd_send_string(string);
	}
	else{
		//Allow to print on the second row
		lcd_send_cmd(0xc0);
		lcd_send_string(string);
	}
}

/**
 * @brief Prints the weight measured by the HX711 sensor
 * @param HX711 data: struct of the weight sensor
 */
void printWeight(HX711 data){
	char v[16];
	unsigned long n_v = (data.offset - hx711Value(data))/444.445;

	sprintf(v,"%lu",n_v);
	printOnDisplay("Peso:", 0);
	printOnDisplay(v, 1);
}

/**
 * @brief Sends a command to LCD display
 * @param char cmd: command to be sent
 */
void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

/**
 * @brief Sends a datum to LCD display
 * @param char data: datum to be displayed
 */
void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

/**
 * @brief Clears the LCD display
 */
void lcd_clear (void)
{
	lcd_send_cmd (0x80);
	for (int i=0; i<70; i++)
	{
		lcd_send_data (' ');
	}
}

/**
 * @brief Initializes the LCD display
 */
void lcd_init (void)
{
	lcd_send_cmd (0x02);
	lcd_send_cmd (0x28);
	lcd_send_cmd (0x0c);
	lcd_send_cmd (0x80);
}

/**
 * @brief Sends a string to LCD display
 * @param char* str: string to be displayed
 */
void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}
