/*
 * UART.h
 *
 * Created: 30.06.2016 16:41:26
 *  Author: Weimer
 */ 


#ifndef UART_H_
#define UART_H_

/******************************************************************************/
/**		
  *		Includes	
  *****************************************************************************/
#include <avr/io.h>
#include <inttypes.h>

#include "ascii_m.h"

/******************************************************************************/
/**		
  *		Definitions	
  *****************************************************************************/
#define			UART0			0	// UART0
#define			UART1			1	// UART1

/******************************************************************************/
/**		
  *		Functions prototypes 
  *****************************************************************************/
void UART_Send (uint8_t UART, uint8_t data);
void UART_SendString (uint8_t UART, uint8_t *str);
void UART_Send_Packet (uint8_t UART, uint32_t value);

#endif /* UART_H_ */