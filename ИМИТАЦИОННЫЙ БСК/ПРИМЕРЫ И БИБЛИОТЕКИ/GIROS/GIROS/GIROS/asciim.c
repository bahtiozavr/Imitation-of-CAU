/*
 * CFile1.c
 *
 * Created: 26.09.2014 9:08:12
 *  Author: Андрей
 */ 

#include "asciim.h"
#include <string.h>
#include <stdio.h>
#include <avr/io.h>



uint8_t Rec = 0;


void UART1_Transmit (unsigned char data)
{
	
	// при работе с ATmega88PA //
	while (!(UCSR1A & (1<<UDRE1)));  
	UDR1 = data;
	// при работе с SAM3 //  
}


unsigned char UART1_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)) )	;
	/* Get and return received data from buffer */
	return UDR1;
}


void RR (unsigned char TR) // функция передачи байта по UART в ASCii
{
	unsigned char R;
	if (TR >= 0x00) { if (TR <= 0x09) {  R = TR + 0x30;}}
	if (TR>= 0x0A) {if (TR <= 0x0F) { R = TR + 0x37;}}
	UART1_Transmit (R);//  передали в уарт UART (R);
}

void ascii_8 (unsigned char FF)  //Функция преоброзования и передачи байта в ASCii формате
{
	unsigned char F;
	F = FF & 0xF0;
	F = (F >> 4);
	RR (F);
	F = FF & 0x0F;
	RR (F);
}

void ascii_16 (unsigned char FFFF) // функция преоброзования двух байт в ASCii формате
{
	
}



//{----- преоброзование принятого байта (символа)  -----}
//{входной парметр i - номер позиции первого символа для преобразования}
//{выходное число в <FFFF>}
//


void hex (unsigned int i )
{
	uint16_t FFFF;
	
	FFFF = 0;
	
	for (i = 3; i <= 6; i++)
	{
		if (glavmas[i] >= '0')
		{ if (glavmas[i] <= '9')
			{
				FFFF = FFFF << 4;
				FFFF = FFFF | (glavmas[i] - '0');
			}
		}
		if (glavmas[i] >= 'A')
		{ if (glavmas[i] <= 'F')
			{
				FFFF = FFFF << 4;
				FFFF = FFFF | (glavmas[i] - 'A' + 10);
			}
		}
	}
	
	return FFFF;
}





// написать функцию

void read_mass ( unsigned char S) //чтение символов в массив по протоколу ГФ
{
	glavmas [Rec] = S; //читаем данные
	//	UART_TX(glavmas[Rec]);
	
	if (glavmas[Rec] == CR)
	{
		Priem = 1;
		
		Rec = 0;
	}
	else  { Rec++ ;}

}

void mass (unsigned int i)
{
	return glavmas[i];
}

