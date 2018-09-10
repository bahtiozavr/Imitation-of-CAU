/*
 * test2.c
 *
 * Created: 12.12.2016 14:31:03
 *  Author: ден
 */ 


#define F_CPU 16000000
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "UART.h"
#include "ascii_m.h"
#include "asciim.h"
#include <avr/interrupt.h>
uint16_t cntA = 0,cntA1=0, cntB=0, cntB1=0;
unsigned int volt=0,amper=0;
unsigned char outbuf[10],n;
int j=1,i=0,ctrl;
void PORT_Init (void)
{
	
	PORTA = 0x00;
	DDRA  = 0xFF;
	PORTB = 0x00;
	DDRB  = 0xFF;
	PORTC = 0x00; //m103 output only
	DDRC  = 0xFF;
	PORTD = 0x00;
	DDRD  = 0xBB;
	PORTE = 0x00;
	DDRE  = 0xBF;
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

void ADC_init(void)
{
	ADCSRA=0xCD;
	ADMUX=0x40;
	
}

void timer1_init(void)
{
	TCCR1A=(1<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (0<<WGM11) | (0<<WGM10);
	TCCR1B|=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H=0x00;
	ICR1L=0x00;
	OCR1AH=0x06;
	OCR1AL=0x40;
	OCR1BH=0x00;
	OCR1BL=0x00;
	OCR1CH=0x00;
	OCR1CL=0x00;
	TCCR1B|=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (1<<CS10);
	TIMSK|=(0<<TICIE1)|(1<<OCIE1A)|(0<<OCIE1B)|(0<<OCIE1C);
	//TIFR=(0<<ICF1)|(0<<OCF1A)|(0<<OCF1B)|(0<<OCF1C);
}

void timer0_init(void)
{
	TCCR0|=(1<<WGM01) | (1<<COM01) | (0<<COM00) | (0<<WGM00) | (0<<CS02) | (0<<CS01)| (0<<CS00);
	TCNT0=0x00;
	OCR0=0xFF;
	ASSR=0x00;
	TIMSK|=(1<<OCIE0)|(0<<TOIE0);
	//TIFR=(0<<OCF0)|(0<<TOV0);
	TCCR0|= (1<<WGM01) | (1<<COM01) | (0<<COM00) | (0<<WGM00) | (0<<CS02) | (1<<CS01)| (1<<CS00);
}

void BUTTON(void)
{
	
	EICRA=0x00;
	EICRB=0xA0;
	EIMSK=0xC0;
	
}

ISR (ADC_vect)
{
	switch (n)
	{
		case 0:
		volt = ADC;// измеряем 4 канал АЦП
		//volt=(volt*6000)/(1024);
		//c = ADC;
		ADMUX = 0x41;// переключаем АЦП на вход ADC5
		n = 1;
		break;
		case 1:
		amper = ADC;// измеряем 4 канал АЦП
		//amper=amper*(0x1770)/(0x3FF);
		//c = ADC;
		ADMUX = 0x40;// переключаем АЦП на вход ADC5
		n = 0;
		break;
	}
	
	
	ADCSRA|=(1<<ADSC);
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

ISR (TIMER1_COMPA_vect)
{
	cntA++;
}

ISR (INT7_vect)
{
	
	if (1<<PINE7)
	{
		i++;
		
		if (i>5)
		{
			UART1_Transmit('*');
			_delay_ms(10);
			i=0;
		}
	}
}

ISR(TIMER0_COMP_vect)
{
	cntB++;
}

 ISR(USART1_RX_vect) // прерывания для UART
 {
	read_mass(UART1_Receive());
 }

void increment_OCR1AL(void)
{
	unsigned char sreg;
	unsigned int i=0,j=0;
	/* Save global interrupt flag */
	sreg = SREG;
	/* Disable interrupts */
	cli();
	/* Set TCNTn to i */
	i=OCR1AL;
	j=OCR1AH;
	i=i+0x10;
	OCR1AH = j;
	OCR1AL = i;
	/* Restore global interrupt flag */
	SREG = sreg;
}

void increment_OCR1AH(void)
{
	unsigned char sreg;
	unsigned int i=0,j=0;
	/* Save global interrupt flag */
	sreg = SREG;
	/* Disable interrupts */
	cli();
	/* Set TCNTn to i */
	i=OCR1AL;
	j=OCR1AH;
	j=j+0x01;
	OCR1AH = j;
	OCR1AL = i;
	/* Restore global interrupt flag */
	SREG = sreg;
}


int main(void)
{
   int Rec=0;
    PORT_Init();
	USART1_Init(103);
	ADC_init();
	//_delay_us(1000);
	 timer0_init();
	 timer1_init();
	 BUTTON();
	Trmass("hello",CR);
	
	sei();
	while(1)
    {
      
	_delay_us(100);
	 if (Priem == 1)
	 {
		//_delay_us(100);
				
		if (glavmas[Rec] == CR)
		{
			if (glavmas[0] == 'A')  // проверяем первый символ
			{
				
				switch (glavmas[1])
				{
					case 'A':
					UART1_Transmit('A');
					UART1_Transmit('A');
					ascii_8(volt>>8);
					ascii_8(volt);
					UART1_Transmit(CR);
					break;
					case 'E':
					UART1_Transmit('A');
					UART1_Transmit('E');
					ascii_8(volt>>8);
					ascii_8(volt);
					UART1_Transmit(CR);
					break;
					case 'B':
					UART1_Transmit('A');
					UART1_Transmit('B');
					ascii_8(amper>>8);
					ascii_8(amper);
					UART1_Transmit(CR);
					break;
					case 'F':
					UART1_Transmit('A');
					UART1_Transmit('F');
					ascii_8(amper>>8);
					ascii_8(amper);
					UART1_Transmit(CR);
					break;
					default: Trmass("?",CR);
				}	
			}
			else Trmass("?",CR);
			Priem = 0;
			Rec = 0;
		}
		else  { Rec++ ;}
		}
    }
}