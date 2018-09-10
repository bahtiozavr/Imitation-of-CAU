//***************************************************************************
//
//  Author(s)...: ����� ������  http://ChipEnable.Ru   
//
//  Target(s)...: mega16
//
//  Compiler....: IAR
//
//  Description.: ������� �������� TWI ����������
//
//  Data........: 08.11.13
//
//***************************************************************************
#ifndef TWI_H
#define TWI_H

#include <avr/io.h>
#include <stdint.h>

/****************************************************************************
  ��������� ���� TWI ������ 
****************************************************************************/

// ����� ��������� ����                     
#define TWI_START                  0x08  // START has been transmitted  
#define TWI_REP_START              0x10  // Repeated START has been transmitted
#define TWI_ARB_LOST               0x38  // Arbitration lost

// ��������� ���� �������� �����������                     
#define TWI_MTX_ADR_ACK            0x18  // SLA+W has been tramsmitted and ACK received
#define TWI_MTX_ADR_NACK           0x20  // SLA+W has been tramsmitted and NACK received 
#define TWI_MTX_DATA_ACK           0x28  // Data byte has been tramsmitted and ACK received
#define TWI_MTX_DATA_NACK          0x30  // Data byte has been tramsmitted and NACK received 

// ��������� ���� �������� ��������� 
#define TWI_MRX_ADR_ACK            0x40  // SLA+R has been tramsmitted and ACK received
#define TWI_MRX_ADR_NACK           0x48  // SLA+R has been tramsmitted and NACK received
#define TWI_MRX_DATA_ACK           0x50  // Data byte has been received and ACK tramsmitted
#define TWI_MRX_DATA_NACK          0x58  // Data byte has been received and NACK tramsmitted

//���������������� ����
#define TWI_SUCCESS                0xff

/****************************************************************************
  ���������������� �������
****************************************************************************/

/*���������� ������� SCL �������*/
uint8_t TWI_SetFreq(uint32_t fr);

/*������� ������ �� ������. num - ���-�� ������*/
uint8_t TWI_SendData(uint8_t *buf, uint8_t num);

/*������� ��������� ���� � �����. num - ���-�� ������*/
uint8_t TWI_ReadData(uint8_t *buf, uint8_t num);

#endif //TWI_H

