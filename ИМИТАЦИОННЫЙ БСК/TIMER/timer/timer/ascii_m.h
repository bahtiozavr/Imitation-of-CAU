//************************************************************************//
//	HAL ASCII Convert
//************************************************************************//
#ifndef HAL_ascii_h
#define HAL_ascii_h

#define AVR_

#ifdef AVR_
#include <inttypes.h>
#endif

#define		CR							0x0D
#define 	ack							0x06
#define 	nak							0x15

void 		ascii_8_m 				(uint8_t F8,     \
									 uint8_t *buff,  \
									 uint8_t i);
void 		ascii_16_m 				(uint16_t F16,   \
									 uint8_t *buff,  \
									 uint8_t i);
void 		ascii_32_m 				(uint32_t F32,   \
									 uint8_t *buff,  \
									 uint8_t i);
uint8_t 	HexToU8 				(uint8_t buff[], \
									 uint8_t k);
uint16_t 	HexToU16 				(uint8_t buff[], \
									 uint8_t k);
uint32_t 	HexToU32 				(uint8_t buff[], \
									 uint8_t k);

#endif

