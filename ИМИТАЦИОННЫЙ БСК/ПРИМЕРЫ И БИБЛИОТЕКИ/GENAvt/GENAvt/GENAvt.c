/*
 * GENAvt.c
 *
 * Created: 01.04.2016 7:40:37
 *  Author: Andrey
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "asciim.h"

#define SET_B(x) |= (1<<x)
#define CLR_B(x) &=~(1<<x)
#define INV_B(x) ^=(1<<(x))
 // обьявляем выход вида тока(постоянный или меандр 12.5 гЦ
#define  Syn 7

#define STB1 2
#define STB2 1
#define STB3 0

#define IN1 3
#define IN2 4
#define IN3 5
#define IN4 6

#define OnSTROB(reg, bit)       reg |= (1<<(bit))   //Выключить строб
#define OffSTROB(reg, bit)        reg &= (~(1<<(bit)))   //Включить строб

#define PORT_kl PORTC

unsigned int  k_toka = 145;
unsigned int  I_koef = 53, U_koef = 46;
 
 int ch, cv;
 unsigned char n, temp1,temp2,temp3, kod_kl, a1_ch, a2_ch, a3_ch, a4_ch, a5_ch, a6_ch ;
 unsigned char a7_ch, a8_ch, a9_ch, a10_ch,  a12_ch, a13_ch;
 unsigned char Tik_timer0, mode1, mode2, tip_toka, porog1, porog2, tok_zadan;
 unsigned char obnov, interval, zadergka, zamer;
 unsigned int  I_izm, U_izm, I_temp, U_temp, S_timer, K_timer, Zadergka_Tik;
 float tok_ust;
 unsigned char kod_kl1, kod_kl2, kod_kl3, kod_kl4, kod_kl5, kod_kl6, kod_kl7, kod_kl8, kod_kl9, kod_kl10;
 unsigned char  kod_kl11, kod_kl12,  kod_kl13;
 unsigned char regim_V_shet, nADC;
 unsigned int count12, a11_ch;
 unsigned char EEPROM_mass[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, countI[10];
 unsigned char i, j, a, b, c, d;
 unsigned char P_ZAP, flag_menu, flag_kalibrovki, Nep_toch; 
 
void PORT_Init (void)
{
	
	PORTA = 0x00;
	DDRA  = 0xFF;
	PORTB = 0x00;
	DDRB  = 0xFF;
	PORTC = 0x78; //m103 output only
	DDRC  = 0x87;
	PORTD = 0x00;
	DDRD  = 0x00;
	PORTE = 0x00;
	DDRE  = 0xFF;
	PORTF = 0x00;
	DDRF  = 0xf0;
	
}



void TIMER0_Init (void)
{
	// режим таймера
	TCCR0 = 0x00; //stop
	ASSR  = 0x00; //set async mode
	TCNT0 = 0x00; //set count
	OCR0  = 0x40;
	TCCR0 = /*(1<<CS02)|(1<<CS01)|(1<<CS00) |*/ (1<<WGM01) | (0<<WGM00); //start timer делим на 64 очищаем по совпадению
	TIMSK |= (1 << OCIE0); // разрешить прерывание таймера по совпадению
}

void TIMER2_Init (void)
{
	// режим таймера
	TCCR2 = 0x00; //stop
	TCNT2 = 0x00; //set count
	OCR2 = 0xF9;
	TCCR2 = (1<<CS22)|(0<<CS21)|(0<<CS20) | (1<<WGM21) | (0<<WGM20); //start timer делим на 64 очищаем по совпадению
	TIMSK |= (1 << OCIE2); // разрешить прерывание таймера по совпадению
}



///////////////////////////////////////////////////////////////////////  TIMER1
//TIMER1 initialize - prescale:1
void timer1_init(void)
{
	TCCR1B = 0x00; //stop
	TCNT1H = 0x00; //setupFC
	TCNT1L = 0x00;//18
	OCR1AH = 0x00;
	OCR1AL = 0xC8;
	OCR1BH = 0x00;
	OCR1BL = 0xC8;
	OCR1CH = 0x00;
	OCR1CL = 0xC8;
	ICR1H  = 0x00;
	ICR1L  = 0xFF;
	TCCR1A = 0xA8; //FC;
	TCCR1B = 0x11; //start Timer
}

///////////////////////////////////////////////////////////////////////  TIMER3
//TIMER3 initialize - prescale:64

void timer3_init(void)
{
	TCCR3B = 0x00; //stop
	TCNT3H = 0x00; //setup
	TCNT3L = 0x00;
	OCR3AH = 0x00;
	OCR3AL = 0x00;
	OCR3BH = 0x00;
	OCR3BL = 0x00;
	OCR3CH = 0x00;
	OCR3CL = 0xC8;
	ICR3H  = 0x00;
	ICR3L  = 0xFF;
	TCCR3A = 0xA8; //FC;
	TCCR3B = 0x11; //start Timer
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


void ADC_Init (void)
{
		//режим ADC
		ADCSRA |= (1 << ADEN) // разрешение АЦП
		|(0 << ADFR) // одиночный режим работы АЦП
		|(1 << ADPS2)|(0 << ADPS1)|(1 << ADPS0) // предделитель на 32 (частота АЦП 125kHz)
		|(1 << ADIE); // разрешение прерывания
		
		ADMUX |= (0 << REFS1)|(1 << REFS0) // Внешний ИОН к AVcc
		|(0 << MUX3)|(0 << MUX2)|(0 << MUX1)|(0 << MUX0); // вход ADC0
		
		ADCSRA |= (1 << ADSC); // запуск преобразования
		//DIDR0 |= (1 << ADC5D)|(1 << ADC4D); // Отключаем неиспользуемые цифровые входы
}


ISR(ADC_vect) //прерывания для АЦП
{
	switch (nADC)
	{
		case 0:
		I_temp = ADC;// измеряем 4 канал АЦП 
		c = ADC;
		ADMUX = 0x41;// переключаем АЦП на вход ADC5
		nADC = 1;
		break;
		case 1:
		U_temp = ADC;// измеряем 4 канал АЦП
		c = ADC;
		ADMUX = 0x40;// переключаем АЦП на вход ADC5
		nADC = 0;
		break;		
	}
	ADCSRA |= (1 << ADSC); // запуск преобразования
}


ISR (TIMER2_COMP_vect)
{
	count12++;
	if (count12 == 10)
	{   
		K_timer++;
		Zadergka_Tik ++;
		if (tip_toka == 3)
		{   PORTF INV_B(Syn);
			
		}
		else if (tip_toka == 1)
		{
			PORTF SET_B (Syn);
		}
		
		if (flag_kalibrovki != 0)
		{
			PORTF INV_B(5);
		}
		count12 = 0;
	}
	
	
}


ISR(TIMER0_COMP_vect)
{   
	
	
	Tik_timer0 = 1;
	ch++;
	interval++;
	flag_menu = 1;
	switch (n)
	{ 
	 case 0:
	 temp3 = PINC ;
	 OnSTROB (PORT_kl, STB3);	 
	 OffSTROB (PORT_kl, STB1);
	 
	 break;
	 case 1:
	 temp1 = PINC ;
	 OnSTROB(PORT_kl,STB1);
	 OffSTROB(PORT_kl,STB2);
	 
	 break;
	 case 2:
	 temp2 = PINC;
	 OnSTROB (PORT_kl, STB2);
	 OffSTROB (PORT_kl, STB3);
	 
	 break;
   }
   if ((n++) >= 2) n = 0;
	
	
	
}

ISR(USART1_RX_vect) // прерывания для UART
{
	read_mass(UART1_Receive());
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	while(EECR & (1<<EEWE));
	EEAR = uiAddress;
	EEDR = ucData;
	EECR |= (1<<EEMWE);
	EECR |= (1<<EEWE);
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
	while(EECR & (1<<EEWE));
	EEAR = uiAddress;
	EECR |= (1<<EERE);
	return EEDR;
}
/*
адреса EEROM
0x0001...0x000F ток
0x0011...0x001F азимут
0x0021...0x002F угол
0x0031...0x003F коэфициент тока ???
0x0041...0x004F коэфициент напряжения :???? 

*/

// запись массива из 10 разрядов в еепром
// данные, начальный адрес
void WRITE_EEPROM_MASS (unsigned char dannie, unsigned int ii)// ii = 1, 11, 21, 31, 41
{
	for (i = ii; i < ii + 10; i++)
	{
		EEPROM_write(i, dannie);
	}
}

// чтение массива из 10 разрядов из еепром
// выходной массив, начальный адрес
void READ_EEPROM_MASS (unsigned char *mass, unsigned int ii)// ii = 1, 11, 21, 31, 41
{
	for (i = ii; i < ii + 10; i++)
	{
		mass[i - ii] = EEPROM_read(i);
	}
	
}


void On_tok (void)
{
	
		tok_ust = (float)tok_zadan * ((float)k_toka/100.0);
		if (tok_ust > 255)
		{
			PORTA = tok_ust ;
			PORTC SET_B (7);
		}
		else
		{
			PORTA = tok_ust ;
			PORTC CLR_B (7);
		}
}



void Of_tok (void)
{
	PORTA = 0x00;
	PORTC CLR_B (7);
}	
	
	
	
	
	

void menu (void)

{
	 //первый столбец
	 // первая строка
	 if ((temp1 & 0x40) == 0)  // Кнопка программирования
	 {
		 
		 if (kod_kl1 != 1)
		 {
			 
			 a1_ch++;
			 if(a1_ch==200)
			 {
				 if (mode1 == 1)
				 {
					 mode1 = 2;
					 a1_ch=0;
					 kod_kl1 = 0x01;
					 Of_tok();
					 UART1_Transmit('R');
					 UART1_Transmit('O');
					 UART1_Transmit('K');
					 UART1_Transmit('A');
					 UART1_Transmit(CR);
					 
					 UART1_Transmit('R');
					 UART1_Transmit('I');
					 UART1_Transmit('B');
					 ascii_8(tok_zadan >> 8);
					 ascii_8(tok_zadan);
					 UART1_Transmit(CR);// отправили ток заданный
					  
				 }
				 else if ((mode1 == 2) )
				 {
					 mode1 = 3;
					 a1_ch=0;
			//		 PORTF INV_B(6);
					 kod_kl1 = 0x01;
					 UART1_Transmit('R');
					 UART1_Transmit('O');
					 UART1_Transmit('K');
					 UART1_Transmit('B');
					 UART1_Transmit(CR);
					 
					 UART1_Transmit('R');
					 UART1_Transmit('B');
					 UART1_Transmit('A');
					 ascii_8(porog1 >> 8);
					 ascii_8(porog1);
					 UART1_Transmit(CR);// отправили porog1 заданный
					 
					 
				 }
				 else if ((mode1 == 3) )
				 {
					 mode1 = 4;
					 a1_ch=0;
				//	 PORTF INV_B(6);
					 kod_kl1 = 0x01;
					 UART1_Transmit('R');
					 UART1_Transmit('O');
					 UART1_Transmit('K');
					 UART1_Transmit('C');
					 UART1_Transmit(CR);
					 
					  UART1_Transmit('R');
					  UART1_Transmit('B');
					  UART1_Transmit('B');
					  ascii_8(porog2 >> 8);
					  ascii_8(porog2);
					  UART1_Transmit(CR);// отправили porog2 заданный
				 }
				 else if ((mode1 == 4))
				 {
					 mode1 = 1;
					 a1_ch=0;
				//	 PORTF INV_B(6);
					 kod_kl1 = 0x01;
					  OCR3AH = 0x00;
					  OCR3AL = porog1;
					  OCR3BH = 0x00;
					  OCR3BL = porog1;
					  OCR3CH = 0x00;
					  OCR3CL = porog2;
					  OCR1AH = 0x00;
					  OCR1AL = porog2;
					  
					  On_tok();
					  
					 UART1_Transmit('R');
					 UART1_Transmit('O');
					 UART1_Transmit('K');
					 UART1_Transmit('D');
					 UART1_Transmit(CR);// рабочий режим
					 
					// posmotret prediduchi regim
					if (regim_V_shet == 0)
					{
						UART1_Transmit('R');
						UART1_Transmit('O');
						UART1_Transmit('A');
						UART1_Transmit(CR);// рабочий режим
					}
					else if (regim_V_shet == 1 )
					{
						UART1_Transmit('R');
						UART1_Transmit('O');
						UART1_Transmit('B');
						UART1_Transmit(CR);//
						
					}
					
					WRITE_EEPROM_MASS(tok_zadan, 200); // записали ток
					WRITE_EEPROM_MASS(porog1, 210);// записали порог
					WRITE_EEPROM_MASS(porog2, 220);// записали порог
				 }
			 }
		 }
	 }
	 else {a1_ch=0; kod_kl1 = 0x00;}
	 
	 
	 // Вторая строка
	 if ((temp1 & 0x20) == 0) // кнопка ввода
	 {
		 if (kod_kl2 != 1)
		 {
			a2_ch++;
			 if(a2_ch==200)
			 {
				 a2_ch=0;
				 if (mode1 != 1)
				 {
					 mode1 = 1;
			//		 PORTF INV_B(6);
					 kod_kl2 = 0x01;
					 
					 OCR3AH = 0x00;
					 OCR3AL = porog1;
					 OCR3BH = 0x00;
					 OCR3BL = porog1;
					 OCR3CH = 0x00;
					 OCR3CL = porog2;
					 OCR1AH = 0x00;
					 OCR1AL = porog2;
					
					 On_tok(); 
					
					 UART1_Transmit('R');
					 UART1_Transmit('O');
					 UART1_Transmit('K');
					 UART1_Transmit('D');
					 UART1_Transmit(CR);// рабочий режим
		
					// posmotret prediduchi regim 
					if (regim_V_shet == 0)
					{
					     UART1_Transmit('R');
					     UART1_Transmit('O');
					     UART1_Transmit('A');
					     UART1_Transmit(CR);// рабочий режим
					}
					else if (regim_V_shet == 1 )
					{
						 UART1_Transmit('R');
						 UART1_Transmit('O');
						 UART1_Transmit('B');
						 UART1_Transmit(CR);//
						
					}
		
					  WRITE_EEPROM_MASS(tok_zadan, 200); // записали ток
					  WRITE_EEPROM_MASS(porog1, 210);// записали порог
					  WRITE_EEPROM_MASS(porog2, 220);// записали порог
				
				 }
			 }
			 
		 }
	 }
	 else {a2_ch=0; kod_kl2 = 0x00;}
	
	 // третья строка
	 if ((temp1 & 0x10) == 0) // кнопка вверх
	 {
		 //if (kod_kl3 != 1)
		// {
			 
			 a3_ch++;
			 if(a3_ch==100)
			 {
				 a3_ch=0;
				 if (mode1 == 2)
				 {
					 
					 if (tok_zadan < 150)
					 {
						 tok_zadan++;
					 }
				//	 PORTF INV_B(6);
					 kod_kl3 = 0x01;
					  UART1_Transmit('R');
					  UART1_Transmit('I');
					  UART1_Transmit('B');
					  ascii_8(tok_zadan >> 8);
					  ascii_8(tok_zadan);
					  UART1_Transmit(CR);// заданный ток
				 }
				 else if (mode1 == 3)
				 {
					 
					 if (porog1 < 250)
					 {
						 porog1++;
					 }
				//	 PORTF INV_B(6);
					 kod_kl3 = 0x01;
					UART1_Transmit('R');
					UART1_Transmit('B');
					UART1_Transmit('A');
					ascii_8(porog1 >> 8);
					ascii_8(porog1);
					UART1_Transmit(CR);// отправили porog1 заданный
				 }
				 
				 else if (mode1 == 4)
				 {
					 
					 if (porog2 < 250)
					 {
						 porog2++;
					 }
				//	 PORTF INV_B(6);
					 kod_kl3 = 0x01;
					 UART1_Transmit('R');
					 UART1_Transmit('B');
					 UART1_Transmit('B');
					 ascii_8(porog2 >> 8);
					 ascii_8(porog2);
					 UART1_Transmit(CR);// отправили porog2 заданный
			
				 }
				 
				 
			 }
			 
		 //}
	 }
	 else {a3_ch=0; kod_kl3 = 0x00;}




	 // четвертая стока
	 if ((temp1 & 0x08) == 0) // кнопка вниз
	 {
		 //if (kod_kl4 != 1)
		 //{
			 
			 a4_ch++;
			 if(a4_ch==100)
			 {
				 a4_ch=0;
				 if (mode1 == 2)
				 {
					 
					 if (tok_zadan > 20)
					 {
						 tok_zadan--;
					 }
				//	 PORTF INV_B(6);
					 kod_kl4 = 0x01;
					UART1_Transmit('R');
					UART1_Transmit('I');
					UART1_Transmit('B');
					ascii_8(tok_zadan >> 8);
					ascii_8(tok_zadan);
					UART1_Transmit(CR);// заданный ток
					
				 }
				 else if (mode1 == 3)
				 {
					 
					 if (porog1 > 1)
					 {
						 porog1--;
					 }
				//	 PORTF INV_B(6);
					 kod_kl4 = 0x01;
					 UART1_Transmit('R');
					 UART1_Transmit('B');
					 UART1_Transmit('A');
					 ascii_8(porog1 >> 8);
					 ascii_8(porog1);
					 UART1_Transmit(CR);// отправили porog1 заданный
				 }
				 
				 else if (mode1 == 4)
				 {
					 
					 if (porog2 >1)
					 {
						 porog2--;
					 }
				//	 PORTF INV_B(6);
					 kod_kl4 = 0x01;
					 UART1_Transmit('R');
					 UART1_Transmit('B');
					 UART1_Transmit('B');
					 ascii_8(porog2 >> 8);
					 ascii_8(porog2);
					 UART1_Transmit(CR);// отправили porog2 заданный
				 }
				 
			 }
			 
		 //}
	 }
	 else {a4_ch=0; kod_kl4 = 0x00;}
	 	 
	 //второй стролбец
	 
	 if ((temp2 & 0x40) == 0)
	 {	
		 kod_kl6 = 0x00;
		 a6_ch = 0;
		 if (kod_kl5 != 1)
			 {
				 a5_ch++;
				 if(a5_ch >= 200)
				 {
					a5_ch = 0;	 
					kod_kl5 = 1;
						 regim_V_shet = 1;
						 UART1_Transmit('R');
						 UART1_Transmit('O');
						 UART1_Transmit('B');
						 UART1_Transmit(CR);// рабочий режим частотомера
			    }
		    }
    }
    else
	{	 
		 a5_ch=0;
		 kod_kl5 = 0x00;
		 if (kod_kl6 != 1)
		 {
		  a6_ch++;
		  if(a6_ch >= 200)
		  {
			  a6_ch = 0;
			  kod_kl6 = 1;
				  regim_V_shet = 0;
				  UART1_Transmit('R');
				  UART1_Transmit('O');
				  UART1_Transmit('A');
				  UART1_Transmit(CR);// рабочий режим вольтметра
				  PORTF CLR_B (6);
		  }
	  }
   }
	
	 if ((temp2 & 0x20) == 0)
	 {
		kod_kl8 = 0x00;
		a8_ch = 0;
		if (kod_kl7 != 1)
		{
			a7_ch++;
			if(a7_ch >= 200)
			{
				a7_ch = 0;
				kod_kl7 = 1;
				zadergka = 30;// задержка 30 секунд
			}
		}
	 }
	 else
	 { 
		  kod_kl7 = 0x00;
		  a7_ch=0;
		  if (kod_kl8 != 1)
		  {
			  a8_ch++;
			  if(a8_ch >= 200)
			  {
				  a8_ch = 0;
				  kod_kl8 = 1;
				  zadergka = 10;// задержка 10 секунд
			  }
		  }
	  }
		 
	 if ((temp2 & 0x10) == 0)
	 {
		kod_kl10 = 0x00;
		a10_ch = 0;
		if (kod_kl9 != 1)
			{
			a9_ch++;
			if(a9_ch >= 200)
				{
					a9_ch = 0;
					kod_kl9 = 1;
					UART1_Transmit('R');
					UART1_Transmit('O');
					UART1_Transmit('T');
					UART1_Transmit('P');
					UART1_Transmit(CR);// поточечный режим запуск по измерению
					Nep_toch = 0;
				
				}
			}
		 
	 }
	 else
	 {
		  kod_kl9 = 0x00;
		  a9_ch=0;
		  if (kod_kl10 != 1)
		  {
			  a10_ch++;
			  if(a10_ch >= 200)
			  {
				  a10_ch = 0;
				  kod_kl10 = 1;
				  UART1_Transmit('R');
				  UART1_Transmit('O');
				  UART1_Transmit('T');
				  UART1_Transmit('N');
				  UART1_Transmit(CR);// непрерывный режим (обзор)
				  Nep_toch = 1;
			  }
		  }
	  }
	 
	 
	 
	 
	 if ((temp2 & 0x08) == 0)
	 {
		 
	//	 PORTF INV_B(6);
		 
		 
	 }
	
	 // третий стролбец
	 if ((temp3 & 0x40) == 0) // запуск калибровки
	 {
		 
		  if (kod_kl11 != 1)
		  {
			  
			  a11_ch++;
			  if(a11_ch==400)
			  {
				  a11_ch=0;				  
				  flag_kalibrovki = 1;
				  
			  }
		  }
	 }
	  else {a11_ch=0; kod_kl11 = 0x00;}
		 
	
		 
		 
	
	 if (((temp3 & 0x20) == 0) && (regim_V_shet != 0) && (tip_toka != 3) && (Nep_toch == 0)) // запуск измерения
	 {
		 
		if (kod_kl12 != 1)
		{
		  
		  a12_ch++;
		  if(a12_ch==200)
		  {
			  a12_ch=0;
			  PORTF INV_B (6);
			  zamer = 1;
			  kod_kl12 = 1;
			  UART1_Transmit('R');
			  UART1_Transmit('O');
			  UART1_Transmit('T');
			  UART1_Transmit('S');
			  UART1_Transmit(CR);//старт измерений
			  
			  
		  }
	  }
	}
	else {a12_ch=0; kod_kl12 = 0x00;}
  
  
	 
	 if ((temp3 & 0x10) == 0) // включили ток
	 {
		if (kod_kl13 != 1)
		{
		
			a13_ch++;
			if(a13_ch==200)
			{	
				a13_ch=0;
				On_tok();
			}
		}
	}
	else {a13_ch=0; kod_kl13 = 0x00;}
		 
		
	 if ((temp3 & 0x08) == 0)
	 {
		 
	//	 PORTF INV_B(6);
		 
		 
	 }
	 Tik_timer0 = 0;
	 
 }
 

void cravnenie (unsigned char priznak)
{

	for (i = 0; i <= 9; i++)
	{
		for (j = i+1; j <= 9; j++)
		{
			if (EEPROM_mass[i] == EEPROM_mass[j])
			{
				countI [i]++;
			}
			
		}
	}
	a = countI[0];
	for(i = 0; i <= 9; i++)
	{
		if (countI[i] > a)
		{
			a = countI[i];
			b = i;
		}
	}
	if (countI[b] == 9 )
	{
		priznak = 0;
	}
	else
	{
		priznak = 1;
	}
		
}

int main(void)
{
	
	cli ();
	PORT_Init();
	TIMER0_Init();
	TIMER2_Init();
	timer1_init();
	timer3_init();
	USART1_Init(103);//9600
	ADC_Init();
	
	sei(); // Разрешить прерывания
	_delay_ms(1000);
	ch = 0;
	mode1 = 1;
	I_izm = 75;
	U_izm = 48;
	//zadergka = 10;
	tip_toka = 1;
	READ_EEPROM_MASS(EEPROM_mass, 200);
	cravnenie(P_ZAP);
	tok_zadan = EEPROM_mass[b];
	if ((tok_zadan > 150) || (tok_zadan < 20))
	{   
		
		tok_zadan = 20;
		WRITE_EEPROM_MASS(tok_zadan, 200);
		
	}
	if (P_ZAP != 0 )
	{
		WRITE_EEPROM_MASS(tok_zadan, 200);
	}
	 
	 
	READ_EEPROM_MASS(EEPROM_mass, 210);
	cravnenie(P_ZAP);
	porog1 = EEPROM_mass[b];
	if (P_ZAP != 0 )
	{
		WRITE_EEPROM_MASS(porog1, 210);
	}	
		
	READ_EEPROM_MASS(EEPROM_mass, 220);
	cravnenie(P_ZAP);           
	porog2 = EEPROM_mass[b];
	if (P_ZAP != 0 )
	{
		WRITE_EEPROM_MASS(porog2, 220);
	}	
	  OCR3AH = 0x00;
	  OCR3AL = porog1;
	  OCR3BH = 0x00;
	  OCR3BL = porog1;
	  OCR3CH = 0x00;
	  OCR3CL = porog2;
	  OCR1AH = 0x00;
	  OCR1AL = porog2;
	  TCCR0 |= (1<<CS02)|(1<<CS01)|(1<<CS00);
	   UART1_Transmit('R');
	   UART1_Transmit('O');
	   UART1_Transmit('B');
	   UART1_Transmit(CR);// рабочий режим частотомера
	
	while(1)
	
    {   
	
		
		if (flag_kalibrovki == 1)
		{
			Of_tok();
			flag_kalibrovki = 2;
			PORTF CLR_B (5);
			PORTF CLR_B (6);
			K_timer = 0;
		}
		else if (flag_kalibrovki ==2)
		{
			if ( K_timer >= 200 )
			{
				flag_kalibrovki = 3;
				tip_toka = 3;
				On_tok();
				K_timer = 0;
			}
		}
		else if (flag_kalibrovki ==3)
		{
			if ( K_timer >= 1000)
			{
				Of_tok();
				tip_toka = 1;
				flag_kalibrovki = 4;
				K_timer = 0;
			}
		}
		else if (flag_kalibrovki == 4)
		{
			if ( K_timer >= 200 )
			{
				On_tok();
				tip_toka = 1;
				flag_kalibrovki = 0;
				K_timer = 0;
				
			}	
			
		}
		else if (flag_kalibrovki == 0)
		{
			if ( K_timer >= 10 )
			{
				PORTF CLR_B (5);
				PORTF CLR_B (6);
				K_timer = 0;
			}
		}
		
	
		if (zamer == 1)
		{
			if (Zadergka_Tik >= 25 )
			{
				S_timer++;
				Zadergka_Tik =0;
				PORTF INV_B (6);
				Zadergka_Tik = 0;
			}
				if (S_timer >= zadergka  )
				{
				PORTF SET_B (6);
				zamer = 0;
				S_timer = 0;
				 UART1_Transmit('R');
				 UART1_Transmit('O');
				 UART1_Transmit('T');
				 UART1_Transmit('P');
				 UART1_Transmit(CR);// стоп измерений и вывод на экран
				}
		}
		
		
		
		_delay_ms(1);//
		if (Tik_timer0 == 1)
		{
			 menu();
		}
		I_izm = ((float)I_temp * (float)I_koef/100.0);
		U_izm = ((float)U_temp * (float)U_koef/100.0);
		
		if (mode1 == 1)
		  {
			if (interval >= 240)
			{
			 interval = 0 ;
		 
		
		   UART1_Transmit('R');
		   UART1_Transmit('I');
		   UART1_Transmit('A');
		   ascii_8(I_izm >> 8);
		   ascii_8(I_izm);
		   UART1_Transmit(CR);
		   UART1_Transmit('R');
		   UART1_Transmit('U');
		   UART1_Transmit('A');
		   ascii_8(U_izm >> 8);
		   ascii_8(U_izm);
		   UART1_Transmit(CR);		   
		  // передаем значение тока и напряжениия.
		  }
		  
		}
	
	}
}