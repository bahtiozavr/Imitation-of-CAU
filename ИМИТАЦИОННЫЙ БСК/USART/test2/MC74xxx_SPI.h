/*
 * MC74xxx_SPI.h
 *
 * Created: 02.11.2015 11:58:20
 *  Author: Weimer
 */ 

#include "spi.h"

#ifndef MC74XXX_SPI_H_
#define MC74XXX_SPI_H_

void MC_595_Init(void);
void MC_595_Clear(void);
void MC_595_Write(uint8_t *buf, uint8_t len);

void MC_165_Init(void);
unsigned char MC_165_Read(void);

void MC_Init(void);
void MC_ReadWrite(uint8_t len, uint8_t *,uint8_t *);

#endif /* MC74XXX_SPI_H_ */