//***************************************************************************
//
//  Author(s)...: Павел Бобков  http://ChipEnable.Ru   
//
//  Target(s)...: mega16
//
//  Compiler....: AS6
//
//  Description.: Драйвер ведущего TWI устройства
//
//  Data........: 08.11.13
//
//***************************************************************************
#include "twi.h"

#define TWSR_MASK          0xfc  
#define TWI_READ_BIT       0x01  
#define TWI_NO_NEED_ANSW   0x00
#define TWI_NEED_ANSW      0x01

static void TWI_WaitInt(void);
static uint8_t TWI_Start(void);
static void TWI_Stop(void);

/*предделители для установки скорости обмена twi модуля*/
uint8_t pre[4] = {2, 8, 32, 128};

/*установка скорости передачи*/
uint8_t TWI_SetFreq(uint32_t fr)
{
  uint8_t i;
  uint16_t twbrValue;
  
  for(i = 0; i<4; i++){
    twbrValue = (((F_CPU)/fr)-16)/pre[i];
    if ((twbrValue > 0)&& (twbrValue < 256)){
       TWBR = (uint8_t)twbrValue;
       TWSR = i;
       return TWI_SUCCESS;
    }
  }
  return 0;
}

/*ожидание установки TWINT*/
static void TWI_WaitInt(void)
{
   while (!(TWCR & (1<<TWINT))); 
} 

/*Сформировать состояние СТАРТ*/
static uint8_t TWI_Start(void)
{
   uint8_t status = TWI_SUCCESS;
   
   TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTA);
   TWI_WaitInt();
   status = TWSR & TWSR_MASK;
   if ((status != TWI_START) && (status != TWI_REP_START)){
      return status;  
   }
   return TWI_SUCCESS;
}  

/*Сформировать состояние СТОП*/
static void TWI_Stop(void)
{
   TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO);
}    

/*послать адрес*/
uint8_t TWI_SendAdr(uint8_t adr)
{
  uint8_t status;
  TWI_WaitInt();
  TWDR = adr;
  TWCR = (1<<TWINT)|(1<<TWEN);
  TWI_WaitInt();
  status = TWSR & TWSR_MASK;
  if ((status != TWI_MTX_ADR_ACK) && (status != TWI_MRX_ADR_ACK)){
     return status;   
  }
  return TWI_SUCCESS; 
}

/*послать байт данных*/
static uint8_t TWI_SendByte(uint8_t data)
{
  uint8_t status;
  
  TWI_WaitInt();
  TWDR = data;
  TWCR = (1<<TWINT)|(1<<TWEN);
  TWI_WaitInt();
  status = TWSR & TWSR_MASK;
  if (status != TWI_MTX_DATA_ACK){
     return status;   
  }
  return TWI_SUCCESS;   
}

/*послать данные из буфера*/
uint8_t TWI_SendData(uint8_t *buf, uint8_t num)
{
  uint8_t i;
  uint8_t status;
  
  status = TWI_Start();
  if (status == TWI_SUCCESS){
     status = TWI_SendAdr((buf[0]));
     if (status == TWI_SUCCESS){   
        for (i = 1; ((i < num)&&(status == TWI_SUCCESS)); i++){
           status = TWI_SendByte(buf[i]); 
        }
     }
     TWI_Stop();
  }
  return status;  
}


/*прочитать байт данных*/
uint8_t TWI_ReadByte(uint8_t *data, uint8_t ask)
{
  uint8_t status;
  
  TWI_WaitInt();
  TWCR = (1<<TWINT)|(1<<TWEN)|((ask & 1) << TWEA);
  TWI_WaitInt();
  *data = TWDR;
  status = TWSR & TWSR_MASK;
  
  if ((ask && (status != TWI_MRX_DATA_ACK))||(!ask && (status != TWI_MRX_DATA_NACK))){
     return status;   
  }
  return TWI_SUCCESS;   
}

/*прочитать данные в буфер*/
uint8_t TWI_ReadData(uint8_t *buf, uint8_t num)
{
  uint8_t i;
  uint8_t status;
  
  status = TWI_Start();
  if (status == TWI_SUCCESS){
     status = TWI_SendAdr((buf[0])|TWI_READ_BIT);
     if (status == TWI_SUCCESS){   
        for (i = 0; ((i < (num-1))&&(status == TWI_SUCCESS)); i++){
           status = TWI_ReadByte(&buf[i], TWI_NEED_ANSW);     
        }
        status = TWI_ReadByte(&buf[i], TWI_NO_NEED_ANSW);
        
     }
     TWI_Stop();
  }
  return status;  
}  

