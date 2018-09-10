/*
 * MC74xxx_SPI.c
 *
 * Created: 02.11.2015 11:58:08
 *  Author: Weimer
 */ 

#include "MC74xxx_SPI.h"


void MC_Init(void)
{
	MC_595_Init();
	MC_165_Init();
	SPI_Init();
	MC_595_Clear();
}

void MC_ReadWrite(uint8_t len, uint8_t *buf_in,uint8_t * buf_out)
{
	PORTD &= ~(1<<PORTD7);
	_delay_us(10);
	PORTD|=(1<<PORTD7);
	
	SPI_SS_Off(SPI_PORT_SS_2,SPI_SS_2);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	SPI_SS_Off(SPI_PORT_SS_1,SPI_SS_1);
	SPI_WriteReadArray(len,buf_in,buf_out);
	SPI_SS_On(SPI_PORT_SS_1,SPI_SS_1);
	SPI_SS_On(SPI_PORT_SS_2,SPI_SS_2);//!!!!!!!!!!!!!!!!!!!!!!!!!
}

void MC_165_Init(void)
{
	DDRD|= (1<<DDD7);
	DDRC|= (1<<DDC3);
	PORTC|=(1<<PORTC3);
	PORTD|=(1<<PORTD7);
}

unsigned char MC_165_Read(void)
{
	unsigned char tmp;
	PORTD &= ~(1<<PORTD7);
	_delay_us(10);
	PORTD|=(1<<PORTD7);
	
	PORTC &= ~(1<<PORTC3);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	tmp = SPI_ReadByte(0x00);
	PORTC|=(1<<PORTC3);//!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	return tmp;
}

void MC_595_Init(void)
{
	DDRC|= (1<<DDC1)|(1<<DDC2);
	PORTC&=~(1<<PORTC1);
	PORTC|=(1<<PORTC2);
}

void MC_595_Clear(void)
{
	PORTC|=(1<<PORTC2);
	PORTC&=~(1<<PORTC1);
	SPI_SS_Off(SPI_PORT_SS_1,SPI_SS_1);
	_delay_us(10);
	SPI_SS_On(SPI_PORT_SS_1,SPI_SS_1);
	PORTC|=(1<<PORTC1);
	PORTC&=~(1<<PORTC2);
}

void MC_595_Write(uint8_t *buf, uint8_t len)
{
	SPI_SS_Off(SPI_PORT_SS_1,SPI_SS_1);
	SPI_WriteArray(len,buf);
	SPI_SS_On(SPI_PORT_SS_1,SPI_SS_1);
}