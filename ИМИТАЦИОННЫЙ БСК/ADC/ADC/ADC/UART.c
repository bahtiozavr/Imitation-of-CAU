/*
 * UART.c
 *
 * Created: 30.06.2016 16:41:44
 *  Author: Weimer
 */ 

/******************************************************************************/
/**		
  *		Includes	
  *****************************************************************************/
#include "UART.h"

/******************************************************************************/
/**	
  *		Functions	
  *****************************************************************************/
/**
  *		@brief	Отправка байта в UART
  *		@param	UART: номер UART
				data: значение
  *		@retval not
  */
void UART_Send (uint8_t UART, uint8_t data) 
{
	if(UART == 0){
		while(!(UCSR0A & (1<<UDRE0)));
		UDR0 = data;
	}
	/*else if(UART == 1){
		while(!(UCSR1A & (1<<UDRE1)));
		UDR1 = data;
	}*/
}

/**
  *		@brief	Отправка строки в UART
  *		@param	UART: номер UART
				*str: адрес строки
  *		@retval not
  */
void UART_SendString(uint8_t UART, uint8_t *str)
{
	uint8_t i = 0;
	while(str[i]){
		UART_Send(UART, str[i]);
		i++;
	}
}

/**
  *		@brief	Отправка пакета в UART в формате hhhhkk<*>
  *		@param	UART: номер UART
				value: значение
  *		@retval not
  */
void UART_Send_Packet (uint8_t UART, uint32_t value)
{
	uint16_t CRC;
	
	//value *= 10;
	CRC = ((value >> 12) & 0x000F) + ((value >> 8) & 0x000F)+((value >> 4) & 0x000F) + (value & 0x000F);
	CRC = 0x00FF - CRC;
	
	uint8_t buf[10] = {0,0,0,0,0,0,0,0,0,0};
		
	ascii_16_m(value,buf,0);
	ascii_8_m(CRC,buf,4);
	UART_SendString(UART, buf);
	
	return;
}