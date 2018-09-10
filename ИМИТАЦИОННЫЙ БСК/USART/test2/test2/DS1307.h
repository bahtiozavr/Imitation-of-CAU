/*
 * DS1307.h
 *
 * Created: 02.11.2015 12:07:13
 *  Author: Weimer
 */ 


#ifndef DS1307_H_
#define DS1307_H_

#include "twi.h"

#define DS1307_ADR				0xD0

#define BinaryToBCD(tmp)		((tmp/10) << 4)|(tmp%10)
#define BCDToBinary(tmp)		(((tmp & 0xF0) >> 4)*10)+(tmp & 0x0F)

typedef struct __DS1307_InitTypeDef{
	uint8_t	SECONDS;
	uint8_t MINUTES;
	uint8_t HOURS;
	uint8_t DAY;
	uint8_t DATE;
	uint8_t MONTH;
	uint8_t YEAR;
	uint8_t CONTROL;
} DS1307_InitTypeDef;

#define DS1307_ADDR_SECONDS		0x00
#define DS1307_ADDR_MINUTS		0x01
#define DS1307_ADDR_HOURS		0x02
#define DS1307_ADDR_DAY			0x03
#define DS1307_ADDR_DATE		0x04
#define DS1307_ADDR_MONTH		0x05
#define DS1307_ADDR_YEAR		0x06
#define DS1307_ADDR_CONTROL		0x07
#define DS1307_ADDR_RAM			0x08			//DS1307_ADDR_RAM_END - DS1307_ADDR_RAM + 1 = 0x40
#define DS1307_ADDR_RAM_END		0x3F

void	DS1307_Init				(uint32_t freq);

void	DS1307_Write			(DS1307_InitTypeDef *DS1307_InitDef);
void	DS1307_Read				(DS1307_InitTypeDef *DS1307_InitDef);

void	DS1307_WriteTime		(DS1307_InitTypeDef *DS1307_InitDef);
void	DS1307_ReadTime			(DS1307_InitTypeDef *DS1307_InitDef);

void	DS1307_WriteDate		(DS1307_InitTypeDef *DS1307_InitDef);
void	DS1307_ReadDate			(DS1307_InitTypeDef *DS1307_InitDef);

#endif /* DS1307_H_ */