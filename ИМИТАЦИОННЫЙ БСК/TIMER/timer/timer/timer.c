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


#include "asciim.h"
#include <avr/interrupt.h>
uint16_t cntA = 0,cntA1=0, cntB=0, cntB1=0, mas4,mas3,mas2,mas1,mas0,glub=0,metki=0;
unsigned int volt=0,amper=0,glubina=0,k=0,datmetki=0,datglub=0,cntAA=0,p=1,o=1;

unsigned int w=100,q=100;
unsigned char outbuf[10];
int j=0,i=0,ctrl,n=0;
void PORT_Init (void)
{
	
	PORTA = 0x00;
	DDRA  = 0xFF;
	PORTB = 0x00;
	DDRB  = 0x00;
	PORTC = 0x00; //m103 output only
	DDRC  = 0xFF;
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
	UCSR1B|= (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	/* Set frame format: 8data, 2stop bit */
	UCSR1C|= (0<<USBS1)|(3<<UCSZ10);
}

void ADC_init(void)
{
	ADCSRA |= (1 << ADEN) // разрешение АЦП
	|(0 << ADFR) // одиночный режим работы АЦП
	|(1 << ADPS2)|(1 << ADPS1)|(0 << ADPS0) // предделитель на 32 (частота АЦП 125kHz)
	|(1 << ADIE); // разрешение прерывания
	
	ADMUX |= (0 << REFS1)|(1 << REFS0) // Внешний ИОН к AVcc
	|(0 << MUX3)|(0 << MUX2)|(0 << MUX1)|(0 << MUX0); // вход ADC0
	
	ADCSRA |= (1 << ADSC);
	
}

void timer0_init(void)
{
	TCCR0|=(1<<WGM01) | (1<<COM01) | (0<<COM00) | (0<<WGM00) | (0<<CS02) | (0<<CS01)| (0<<CS00);
	TCNT0=0x00;
	OCR0=0x18;
	ASSR=0x00;
	TIMSK|=(1<<OCIE0)|(0<<TOIE0);
	//TIFR=(0<<OCF0)|(0<<TOV0);
	TCCR0|= (1<<WGM01) | (1<<COM01) | (0<<COM00) | (0<<WGM00) | (1<<CS02) | (0<<CS01)| (0<<CS00);
	
}


void timer1_init(void)
{
	TCCR1A|=(1<<COM1A1) | (0<<COM1A0) | (0<<WGM11) | (0<<WGM10);
	TCCR1B|=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H=0x00;
	ICR1L=0x00;
	OCR1AH=0x61;
	OCR1AL=0xA8;
	
	TCCR1B|=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (1<<WGM12) | (0<<CS12) | (1<<CS11) | (1<<CS10);
	TIMSK|=(0<<TICIE1)|(1<<OCIE1A);
	//TIFR=(0<<ICF1)|(1<<OCF1A)|(0<<OCF1B)|(0<<OCF1C);
}

void BUTTON(void)
{
	
	EICRA=0x00;
	EICRB=0xA0;
	EIMSK=0xC0;
	
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

void decrement(void)
{
	unsigned char sreg;
	unsigned int i=0;
	/* Save global interrupt flag */
	k--;
	sreg = SREG;
	/* Disable interrupts */
	cli();
	/* Set TCNTn to i */
	i=OCR1A;
	
	switch (k)
	{
		case 0:
		
		i=mas0;
		break;
		case 1:
		i=mas1;
		break;
		case 2:
		i=mas2;
		break;
		case 3:
		i=mas3;
		break;
		case 4:
		i=mas4;
		break;
		default:
		k=4;
		i=mas4;
		break;
	}
	OCR1A = i;
	/* Restore global interrupt flag */
	SREG = sreg;
	sei();
}

void increment(void)
{
	unsigned char sreg;
	unsigned int i=0;
	/* Save global interrupt flag */
	k++;
	sreg = SREG;
	/* Disable interrupts */
	cli();
	/* Set TCNTn to i */
	i=OCR1A;
	
	switch (k)
	{
		case 0:
		
		i=mas0;
		break;
		case 1:
		
		i=mas1;
		break;
		case 2:
		
		i=mas2;
		break;
		case 3:
		
		i=mas3;
		break;
		case 4:
		
		i=mas4;
		break;
		default:
		
		k=0;
		i=mas0;
		break;
	}
	OCR1A = i;
	/* Restore global interrupt flag */
	SREG = sreg;
	sei();
}

ISR(USART1_RX_vect) // прерывания для UART
{
	read_mass(UART1_Receive());
}

ISR (TIMER1_COMPA_vect)
{
	cntB++;
	
}

ISR(TIMER0_COMP_vect)
{
	if (glub==0){w=datglub;q=datmetki;metki=0;}
	cntAA++;
	if (cntAA==0){w=datglub;q=datmetki;}
	
	
	if (w==cntAA) {glub++;w=w+datglub;}
	if (q==glub) {metki++;q=q+datmetki;}
}

ISR (INT7_vect)
{
	
	//if (1<<PINE7)
	//{
	j++;
	
	if (j>2)
	{
		//UART1_Transmit('*');
		increment();
		//_delay_ms(10);
		j=0;
	}
	
	//}
}

ISR (INT6_vect)
{
	
	//if (1<<PINE6)
	//{
	i++;
	
	if (i>2)
	
	{
		//_delay_ms(500);
		//UART1_Transmit('*');
		decrement();
		//_delay_ms(10);
		i=0;
	}
	
	
	//}
}

ISR (ADC_vect)
{
	
	switch (n)
	{
		case 0:
		
		volt = ADC;// измеряем 0 канал АЦП
		volt=(volt*6);
		
		ADMUX =0x41;// переключаем АЦП на вход ADC1
		n = 1;
		break;
		
		case 1:
		
		amper = ADC;// измеряем 1 канал АЦП
		amper=(amper*26);
		
		ADMUX =0x42;// переключаем АЦП на вход ADC5
		n = 2;
		break;
		
		case 2:
		datglub=ADC;
		//a=(0.0130864*datglub*datglub*datglub-0.05629629*datglub*datglub+0.154320972*datglub+1)*500;
		datglub=datglub*10;
		ADMUX =0x43;// переключаем АЦП на вход ADC6
		n = 3;
		break;
		
		case 3:
		datmetki=ADC;
		//b=(0.0130864*datmetki*datmetki*datmetki-0.05629629*datmetki*datmetki+0.154320972*datmetki+1)*500;
		datmetki=datmetki*0.04+80;
		ADMUX =0x40;// переключаем АЦП на вход ADC0
		n = 0;
		break;
	}
	
	ADCSRA|=(1<<ADSC);
	
}




int main(void)
{
    mas0=0x61A8;
	mas1=0x09C4;
	mas2=0x00F9;
	mas3=0x0018;
	mas4=0x0004;
	
	cli();
    
    PORT_Init();
	
	//_delay_us(1000);
	USART1_Init(103);
	ADC_init();
	timer0_init();
	timer1_init();
	//timer3_init();
	BUTTON();
	
	//int i=0;
	
	//Trmass("hello",CR);
	
	sei();
	//_delay_us(1000);
	/*cntA1=cntA;
	cntB1=cntB;*/
	int Rec=0;
	metki=0;glub=0;
	
	while(1)
    {
     
		 
		_delay_us(100);
    
			
			if ((i<4)&&(0<<INTF6))
			{
				i=0;
			}
			if ((j<4)&&(0<<INTF7))
			{
				j=0;
			}
			if (Priem == 1)
			{
				//_delay_us(50);
			//UART1_Transmit('2');
				if (glavmas[Rec] == CR)
				{
					//UART1_Transmit('1');
					switch (glavmas[0])
					{
					case 'A':  // проверяем первый символ
							
							switch (glavmas[1])
							{
								case 'A':
								//UART1_Transmit('A');
								//UART1_Transmit('A');
								ascii_8(volt>>8);
								ascii_8(volt);
								//UART1_Transmit(' ');
								ctrl=((volt>>12)&(0x000F))+((volt>>8)&(0x000F))+((volt>>4)&(0x000F))+((volt)&(0x000F));
								ctrl=0xFF-ctrl;
								ascii_8(ctrl);
								UART1_Transmit('*');
								break;
								case 'E':
								//UART1_Transmit('A');
								//UART1_Transmit('E');
								ascii_8(volt>>8);
								ascii_8(volt);
								//UART1_Transmit(' ');
								ctrl=((volt>>12)&(0x000F))+((volt>>8)&(0x000F))+((volt>>4)&(0x000F))+((volt)&(0x000F));
								ctrl=0xFF-ctrl;
								ascii_8(ctrl);
								UART1_Transmit('*');
								break;
								case 'B':
								//UART1_Transmit('A');
								//UART1_Transmit('B');
								
								ascii_8(amper>>8);
								ascii_8(amper);
								//UART1_Transmit(' ');
								ctrl=((amper>>12)&(0x000F))+((amper>>8)&(0x000F))+((amper>>4)&(0x000F))+((amper)&(0x000F));
								ctrl=0xFF-ctrl;
								ascii_8(ctrl);
								UART1_Transmit('*');
								break;
								case 'F':
								//UART1_Transmit('A');
								//UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								//UART1_Transmit(' ');
								ctrl=((amper>>12)&(0x000F))+((amper>>8)&(0x000F))+((amper>>4)&(0x000F))+((amper)&(0x000F));
								ctrl=0xFF-ctrl;
								ascii_8(ctrl);
								UART1_Transmit('*');
								break;
								case 'C':
								UART1_Transmit('*');/*
								UART1_Transmit('A');
								ascii_8(volt>>8);
								ascii_8(volt);
								UART1_Transmit(CR);*/
								break;
								case 'D':
								UART1_Transmit('*');/*
								UART1_Transmit('A');
								ascii_8(volt>>8);
								ascii_8(volt);
								UART1_Transmit(CR);*/
								break;
								case 'I':
								UART1_Transmit('*');/*
								UART1_Transmit('A');
								ascii_8(volt>>8);
								ascii_8(volt);
								UART1_Transmit(CR);*/
								break;
								case 'U':
								UART1_Transmit('*');/*
								UART1_Transmit('A');
								ascii_8(volt>>8);
								ascii_8(volt);
								UART1_Transmit(CR);*/
								break;
								default: UART1_Transmit('?');break;
							}
					break;
					
					case 'B':
							switch (glavmas[1])
							{
								case 'A':
								UART1_Transmit('*');/*
								UART1_Transmit('A');
								ascii_8(volt>>8);
								ascii_8(volt);
								UART1_Transmit(CR);*/
								break;
								case 'E':
								//UART1_Transmit('B');
								//UART1_Transmit('E');
								
								cntB1=cntB;
								cntA1=cntAA;
								//ctrl=0xFF;
								ctrl=((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F));
								ctrl+=((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F));
								ctrl+=((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F));
								ctrl+=((cntA1>>12)&(0x000F))+((cntA1>>8)&(0x000F))+((cntA1>>4)&(0x000F))+((cntA1)&(0x000F));
								ctrl=0xFF-ctrl;
								ascii_8(cntB1>>8);
								ascii_8(cntB1);
								//UART1_Transmit(' ');
								ascii_8(cntB1>>8);
								ascii_8(cntB1);
								//UART1_Transmit(' ');
								ascii_8(cntB1>>8);
								ascii_8(cntB1);
								//UART1_Transmit(' ');
								ascii_8(cntA1>>8);
								ascii_8(cntA1);
								//UART1_Transmit(' ');
								
								ascii_8(ctrl);
								UART1_Transmit('*');
								//cntA1=cntA;
								//cntB1=cntB;
								break;
								case 'B':
								UART1_Transmit('*');/*
								UART1_Transmit('B');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;
								case 'F':
								cntB1=cntB;
								cntA1=cntAA;
								ascii_8(cntB1>>8);
								ascii_8(cntB1);
								//UART1_Transmit(' ');
								ascii_8(cntB1>>8);
								ascii_8(cntB1);
								//UART1_Transmit(' ');
								ascii_8(cntB1>>8);
								ascii_8(cntB1);
								//UART1_Transmit(' ');
								ascii_8(cntA1>>8);
								ascii_8(cntA1);
								//UART1_Transmit(' ');
								//ctrl=0xFF-(3*(((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F)))+((cntA1>>12)&(0x000F))+((cntA1>>8)&(0x000F))+((cntA1>>4)&(0x000F))+((cntA1)&(0x000F)));
								ctrl=((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F));
								ctrl+=((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F));
								ctrl+=((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F));
								ctrl+=((cntA1>>12)&(0x000F))+((cntA1>>8)&(0x000F))+((cntA1>>4)&(0x000F))+((cntA1)&(0x000F));
								ctrl=0xFF-ctrl;
								ascii_8(ctrl);
								UART1_Transmit('*');
								break;
								case 'H':
								cntB1=cntB;
								cntA1=cntAA;
								ascii_8(cntB1>>8);
								ascii_8(cntB1);
								//UART1_Transmit(' ');
								ascii_8(cntB1>>8);
								ascii_8(cntB1);
								//UART1_Transmit(' ');
								ascii_8(cntB1>>8);
								ascii_8(cntB1);
								//UART1_Transmit(' ');
								ascii_8(cntA1>>8);
								ascii_8(cntA1);
								//UART1_Transmit(' ');
								//ctrl=0xFF-(3*((cntB1&(0xF000))+(cntB1&(0x0F00))+(cntB1&(0x00F0))+(cntB1&(0x000F)))+(cntA1&(0xF000))+(cntA1&(0x0F00))+(cntA1&(0x00F0))+(cntA1&(0x000F)));
								ctrl=((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F));
								ctrl+=((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F));
								ctrl+=((cntB1>>12)&(0x000F))+((cntB1>>8)&(0x000F))+((cntB1>>4)&(0x000F))+((cntB1)&(0x000F));
								ctrl+=((cntA1>>12)&(0x000F))+((cntA1>>8)&(0x000F))+((cntA1>>4)&(0x000F))+((cntA1)&(0x000F));
								ctrl=0xFF-ctrl;
								ascii_8(ctrl);
								UART1_Transmit('*');
								break;
								case 'C':
								UART1_Transmit('*');/*
								UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;
								default: UART1_Transmit('?');break;
							}
					break;	
					case 'C':
							switch (glavmas[1])
							{
								case 'D':
								UART1_Transmit('*');/*
								UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;	
							}
							
					case 'D':
							switch (glavmas[1])
							{
								case 'A':
								
								if ((glub%2)==0)
								{
									
									UART1_Transmit(0x01);
									UART1_Transmit('*');
								}
								else 
								{
									
									UART1_Transmit(0x00);
									UART1_Transmit('*');
								}/*
								UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;
								case 'B':
								if ((metki%2)==0)
								{
									UART1_Transmit(0x01);
									UART1_Transmit('*');
								}
								else
								{
									UART1_Transmit(0x00);
									UART1_Transmit('*');
								}/*
								UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;
								case 'E':
								UART1_Transmit('*');/*
								UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;
								case 'C':
								glubina=glub;
								ascii_8(glubina>>8);
								ascii_8(glubina);
								ctrl=((glubina>>12)&(0x000F))+((glubina>>8)&(0x000F))+((glubina>>4)&(0x000F))+((glubina)&(0x000F));
								ctrl=0xFF-ctrl;
								ascii_8(ctrl);
								UART1_Transmit('*');/*
								UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;
								case 'D':
								glub=0;
								UART1_Transmit('*');/*
								UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;
								case 'F':
								//UART1_Transmit(0x0000);
								UART1_Transmit('*');/*
								UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;
								case 'G':
								//UART1_Transmit(0x0000);
								UART1_Transmit('*');/*
								UART1_Transmit('F');
								ascii_8(amper>>8);
								ascii_8(amper);
								UART1_Transmit(CR);*/
								break;
								default: UART1_Transmit('?');break;
							}
					break;
					default: UART1_Transmit('?');break;
					
					}
					Priem = 0;
					Rec = 0;
		
				}
				else Rec++;	
			}
	 
    
	}
}
	