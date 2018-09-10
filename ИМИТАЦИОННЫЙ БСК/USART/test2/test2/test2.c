/*
 * test2.c
 *
 * Created: 12.12.2016 14:31:03
 *  Author: ден
 */ 


//#define F_CPU 16000000
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "ascii_m.h"
#include "asciim.h"
#include <avr/interrupt.h>

#include "i2cgf.h"

unsigned char outbuf[255];int outbyte=0, tekbyte=0;
void PORT_Init (void)
{
	
	PORTA = 0x00;
	DDRA  = 0x00;
	PORTB = 0x00;
	DDRB  = 0xFF;
	PORTC = 0x00; //m103 output only
	DDRC  = 0x00;
	PORTD = 0x00;
	DDRD  = 0xBB;
	PORTE = 0x00;
	DDRE  = 0x00;
	PORTF = 0x00;
	DDRF  = 0x00;
	
}
void USART1_Init( unsigned int baud )
{
	/* Set baud rate */
	UBRR1H = (unsigned char)(baud>>8);
	UBRR1L = (unsigned char)baud;
	/* Включаем приемник и передатчик УАРТ */ // разрешаем прерывание по приему.
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	/* Set frame format: 8data, 2stop bit */
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
}

void Trmass (char *buf,char buf1)
{
	
		int i=0,l=0;
		l=strlen(buf);
		for (i=0;i<l;i++)
		{
		
			UART1_Transmit(buf[i]);
			//buf[i]=0;
		}
	UART1_Transmit(buf1);
	
}



 ISR(USART1_RX_vect) // прерывания для UART
 {
	read_mass(UART1_Receive());
 }


// запись I2C
void I2C_write (unsigned char reg, unsigned char value)
{

	i2c_start_cond();
	i2c_send_byte(0x32);
	i2c_send_byte(reg);
	i2c_send_byte(value);
	i2c_stop_cond();
}

int8_t i2c_readBytes(uint8_t length, uint8_t *data)
{
	uint8_t i = 0;
	int8_t count = 0;
	if(length > 0)
	{
		//request register
		//i2c_start_cond();
		//i2c_send_byte(0x32);
		//i2c_send_byte(0x50);
		//_delay_us(10);
		//i2c_stop_cond();
		
		//_delay_ms(2);
		//read data
		i2c_start_cond();
		i2c_send_byte(0x33);
		for(i=0; i<length; i++)
		{
			count++;
			if(i==length-1)
				data[i] = i2c_get_byte(1);
			else
				data[i] = i2c_get_byte(0);
		}
		i2c_stop_cond();
		
	}
	return count;
}



int main(void)
{
   int Rec=0;
   uint8_t status=0;
   uint8_t buf[6];
   uint8_t heading1,heading2,pitch1,pitch2,roll1,roll2;
   
    PORT_Init();
	USART1_Init(103);
	i2c_init();
	_delay_ms(1000);
	//TWI_SetFreq(100000);
	//Trmass("hello12",CR);
	sei();
	
	//устанавливаем указатель DS1307 на нужный адрес//
	i2c_start_cond();
	_delay_ms(1);
	i2c_stop_cond();
	
	
	while(1)
    {
      //buf[0] = 0x19;
      //buf[1] = 0x33;
	  //ascii_8(status);
	  //UART1_Transmit(CR);
      UART1_Transmit('*');UART1_Transmit(CR);
	  //status = TWI_ReadData(buf, 6);
	  i2c_start_cond();
	  i2c_send_byte(0x32);
	  i2c_send_byte(0x50);
	  //i2c_send_byte(value);
	  i2c_stop_cond();
	  
	  
	  _delay_ms(10);
	  
	  
	  i2c_start_cond();
	  i2c_send_byte(0x33);_delay_ms(2);
	  buf[0]=i2c_get_byte(0);_delay_ms(2);
	  buf[1]=i2c_get_byte(0);_delay_ms(2);
	  buf[2]=i2c_get_byte(0);_delay_ms(2);
	  buf[3]=i2c_get_byte(0);_delay_ms(2);
	  buf[4]=i2c_get_byte(0);_delay_ms(2);
	  buf[5]=i2c_get_byte(1);
	  i2c_stop_cond();
	  ascii_8(buf[0]);ascii_8(buf[1]);UART1_Transmit(CR);
	  ascii_8(buf[2]);ascii_8(buf[3]);UART1_Transmit(CR);
	  ascii_8(buf[4]);ascii_8(buf[5]);UART1_Transmit(CR);
	  //i2c_restart_cond();
	  //i2c_send_byte(0x33);
	  //i2c_stop_cond();
	  //PORTB^=(1<<PORTB0);
	  _delay_ms(350);      //ascii_8(status);
	  
      //считываем время, если нет ошибок/
      /*if (status == TWI_SUCCESS){
	      
	      _delay_ms(10);
	      
	      TWI_ReadData(buf, 6);
	      heading1=buf[0];
	      heading2=buf[1];
	      pitch1=buf[2];
	      pitch2=buf[3];
	      roll1=buf[4];
	      roll2=buf[5];
	      Trmass("ok1)",CR);
      }
      
      for (Rec=0;Rec<6;Rec++)
      {
	      UART1_Transmit(buf[Rec]);
	      Trmass("ok2)",CR);
      }*/
	
    }
}