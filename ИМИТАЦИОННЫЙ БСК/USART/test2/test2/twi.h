//***************************************************************************
//
//  Author(s)...: Павел Бобков  http://ChipEnable.Ru   
//
//  Target(s)...: mega16
//
//  Compiler....: IAR
//
//  Description.: Драйвер ведущего TWI устройства
//
//  Data........: 08.11.13
//
//***************************************************************************
#ifndef TWI_H
#define TWI_H

#include <avr/io.h>
#include <stdint.h>

/****************************************************************************
  Статусные коды TWI модуля 
****************************************************************************/

// Общие статусные коды                     
#define TWI_START                  0x08  // START has been transmitted  
#define TWI_REP_START              0x10  // Repeated START has been transmitted
#define TWI_ARB_LOST               0x38  // Arbitration lost

// Статусные коды ведущего передатчика                     
#define TWI_MTX_ADR_ACK            0x18  // SLA+W has been tramsmitted and ACK received
#define TWI_MTX_ADR_NACK           0x20  // SLA+W has been tramsmitted and NACK received 
#define TWI_MTX_DATA_ACK           0x28  // Data byte has been tramsmitted and ACK received
#define TWI_MTX_DATA_NACK          0x30  // Data byte has been tramsmitted and NACK received 

// Статусные коды ведущего приемника 
#define TWI_MRX_ADR_ACK            0x40  // SLA+R has been tramsmitted and ACK received
#define TWI_MRX_ADR_NACK           0x48  // SLA+R has been tramsmitted and NACK received
#define TWI_MRX_DATA_ACK           0x50  // Data byte has been received and ACK tramsmitted
#define TWI_MRX_DATA_NACK          0x58  // Data byte has been received and NACK tramsmitted

//пользовательские коды
#define TWI_SUCCESS                0xff

/****************************************************************************
  Пользовательские функции
****************************************************************************/

/*установить частоту SCL сигнала*/
uint8_t TWI_SetFreq(uint32_t fr);

/*послать данные из буфера. num - кол-во байтов*/
uint8_t TWI_SendData(uint8_t *buf, uint8_t num);

/*принять несколько байт в буфер. num - кол-во байтов*/
uint8_t TWI_ReadData(uint8_t *buf, uint8_t num);

#endif //TWI_H

