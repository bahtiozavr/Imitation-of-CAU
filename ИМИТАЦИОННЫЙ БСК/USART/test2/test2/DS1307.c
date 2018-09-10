/*
 * DS1307.c
 *
 * Created: 02.11.2015 12:07:03
 *  Author: Weimer
 */ 

#include "DS1307.h"

void DS1307_Init(uint32_t freq)
{
	TWI_SetFreq(freq);
}

void DS1307_Write(DS1307_InitTypeDef *DS1307_InitDef)
{
	uint8_t buf[10];
	//установка начального значения часов/
	buf[0] = DS1307_ADR;									//адрес часов
	buf[1] = DS1307_ADDR_SECONDS;												//адрес регистра
	buf[2] = BinaryToBCD(DS1307_InitDef->SECONDS);			
	buf[3] = BinaryToBCD(DS1307_InitDef->MINUTES);			
	buf[4] = BinaryToBCD(DS1307_InitDef->HOURS);			
	buf[5] = BinaryToBCD(DS1307_InitDef->DAY);				
	buf[6] = BinaryToBCD(DS1307_InitDef->DATE);
	buf[7] = BinaryToBCD(DS1307_InitDef->MONTH);
	buf[8] = BinaryToBCD(DS1307_InitDef->YEAR);
	buf[9] = BinaryToBCD(DS1307_InitDef->CONTROL);
	
	TWI_SendData(buf, 9);
}

void DS1307_Read(DS1307_InitTypeDef *DS1307_InitDef)
{
	uint8_t buf[8];
	//устанавливаем указатель DS1307 на нужный адрес//
	buf[0] = DS1307_ADR;
	buf[1] = DS1307_ADDR_SECONDS;
	uint8_t status = TWI_SendData(buf, 2);
	//считываем время, если нет ошибок/
	if (status == TWI_SUCCESS){
		//считываем время/
		TWI_ReadData(buf, 8);
		DS1307_InitDef->SECONDS = BCDToBinary(buf[0]);
		DS1307_InitDef->MINUTES = BCDToBinary(buf[1]);
		DS1307_InitDef->HOURS	= BCDToBinary(buf[2]);
		DS1307_InitDef->DAY		= BCDToBinary(buf[3]);
		DS1307_InitDef->DATE	= BCDToBinary(buf[4]);
		DS1307_InitDef->MONTH	= BCDToBinary(buf[5]);
		DS1307_InitDef->YEAR	= BCDToBinary(buf[6]);
		DS1307_InitDef->CONTROL = BCDToBinary(buf[7]);
	}
}

void	DS1307_WriteTime(DS1307_InitTypeDef *DS1307_InitDef)
{
	uint8_t buf[5];
	//установка начального значения часов/
	buf[0] = DS1307_ADR;									//адрес часов
	buf[1] = DS1307_ADDR_SECONDS;											//адрес регистра
	buf[2] = BinaryToBCD(DS1307_InitDef->SECONDS);
	buf[3] = BinaryToBCD(DS1307_InitDef->MINUTES);
	buf[4] = BinaryToBCD(DS1307_InitDef->HOURS);
	TWI_SendData(buf, 4);
}
void	DS1307_ReadTime(DS1307_InitTypeDef *DS1307_InitDef)
{
	uint8_t buf[3];
	//устанавливаем указатель DS1307 на нужный адрес//
	buf[0] = DS1307_ADR;
	buf[1] = DS1307_ADDR_SECONDS;
	uint8_t status = TWI_SendData(buf, 2);
	//считываем время, если нет ошибок/
	if (status == TWI_SUCCESS){
		//считываем время/
		TWI_ReadData(buf, 3);
		DS1307_InitDef->SECONDS = BCDToBinary(buf[0]);
		DS1307_InitDef->MINUTES = BCDToBinary(buf[1]);
		DS1307_InitDef->HOURS	= BCDToBinary(buf[2]);
	}
}

void	DS1307_WriteDate(DS1307_InitTypeDef *DS1307_InitDef)
{
	uint8_t buf[6];
	buf[0] = DS1307_ADR;									//адрес часов
	buf[1] = DS1307_ADDR_DAY;								//адрес регистра
	buf[2] = BinaryToBCD(DS1307_InitDef->DAY);
	buf[3] = BinaryToBCD(DS1307_InitDef->DATE);
	buf[4] = BinaryToBCD(DS1307_InitDef->MONTH);
	buf[5] = BinaryToBCD(DS1307_InitDef->YEAR);
	TWI_SendData(buf, 5);
}
void	DS1307_ReadDate(DS1307_InitTypeDef *DS1307_InitDef)
{
	uint8_t buf[4];
	//устанавливаем указатель DS1307 на нужнй адрес//
	buf[0] = DS1307_ADR;
	buf[1] = DS1307_ADDR_DAY;
	uint8_t status = TWI_SendData(buf, 2);
	//считываем время, если нет ошибок/
	if (status == TWI_SUCCESS){
		//считываем время/
		TWI_ReadData(buf, 4);
		DS1307_InitDef->DAY		= BCDToBinary(buf[0]);
		DS1307_InitDef->DATE	= BCDToBinary(buf[1]);
		DS1307_InitDef->MONTH	= BCDToBinary(buf[2]);
		DS1307_InitDef->YEAR	= BCDToBinary(buf[3]);
	}
}
