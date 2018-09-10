/*
 * PICH.c
 *
 * Created: 04.04.2016 13:56:06
 *  Author: Andrey
 */ 


#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include "asciim.h"



#define SET_B(x) |= (1<<x)
#define CLR_B(x) &=(~(1<<x))
#define INV_B(x) ^=(1<<(x))

#define PORT_IND1 PORTA //
#define PORT_IND2 PORTD //

#define PORT_RELE PORTC//

#define OffSEGMENT(reg, bit)       reg |= (1<<(bit))   //Выключить сегмент
#define OnSEGMENT(reg, bit)        reg &= (~(1<<(bit)))   //Включить сегмент


// обьявляем разряды индикатора
#define  Seg1 3
#define  Seg2 2
#define  Seg3 1
#define  Seg4 0
#define  Seg5 1 
#define  Seg6 5
#define  Seg7 7
#define  Seg8 4


//-----------------0-----1-----2-----3-----4-----5-----6-----7-----8----9--
char CIFRA[ ] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // массив с цифрами
char SIMVOL[ ] = {0x73, 0x50, 0xFF, 0x77, 0x6E, 0x78, 0x00}; // P, г, A, Y, t 
		
char CIFRA1[4], CIFRA2[4], n, sek_tim;
int	disp1, disp2; 
unsigned char data1, obnov, count, priznak, start_izm, izmmeren, count_izm, flag_izm;
unsigned char regim, zadergka, tok, tok2, porog1, porog2, napr,  toch_nep, izmmeren_tik;	
unsigned int	ygol, azim, ygol_t, azim_t;
 float summa_ygol, summa_azim;
 

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
	DDRF  = 0xff;
	
}



void TIMER0_Init (void)
{
	// режим таймера
	TCCR0 = 0x00; //stop
	ASSR  = 0x00; //set async mode
	TCNT0 = 0x00; //set count
	OCR0  = 0x40;
	TCCR0 = (0<<CS02)|(1<<CS01)|(1<<CS00) | (1<<WGM01) | (0<<WGM00); //start timer делим на 64 очищаем по совпадению
	TIMSK |= (1 << OCIE0); // разрешить прерывание таймера по совпадению
}

void TIMER2_Init (void)
{
	// режим таймера
	TCCR2 = 0x00; //stop
	//ASSR  = 0x00; //set async mode
	TCNT2 = 0x00; //set count
	OCR2  = 0x7C;
	TCCR2 = (1<<CS22)|(0<<CS21)|(1<<CS20) | (1<<WGM21) | (0<<WGM20); //start timer делим на 1024 очищаем по совпадению
	TIMSK |= (1 << OCIE2); // разрешить прерывание таймера по совпадению
	
}
void TIMER1_Init(void)
{
	
		TCCR1B = 0x00; //stop
		TCNT1H = 0x00 /*INVALID SETTING*/; //setup
		TCNT1L = 0x00 /*INVALID SETTING*/;
		OCR1AH = 0x00 /*INVALID SETTING*/;
		OCR1AL = 0x00 /*INVALID SETTING*/;
		OCR1BH = 0x00 /*INVALID SETTING*/;
		OCR1BL = 0x00 /*INVALID SETTING*/;
		OCR1CH = 0x00 /*INVALID SETTING*/;
		OCR1CL = 0x00 /*INVALID SETTING*/;
		ICR1H  = 0x00 /*INVALID SETTING*/;
		ICR1L  = 0x00 /*INVALID SETTING*/;
		TCCR1A = 0x00;
		TCCR1B = 0x06; //start Timer
	
}

void TIMER3_Init(void)
{

		TCCR3B = 0x00; //stop
		TCNT3H = 0x00 /*INVALID SETTING*/; //setup
		TCNT3L = 0x00 /*INVALID SETTING*/;
		OCR3AH = 0x00 /*INVALID SETTING*/;
		OCR3AL = 0x00 /*INVALID SETTING*/;
		OCR3BH = 0x00 /*INVALID SETTING*/;
		OCR3BL = 0x00 /*INVALID SETTING*/;
		OCR3CH = 0x00 /*INVALID SETTING*/;
		OCR3CL = 0x00 /*INVALID SETTING*/;
		ICR3H  = 0x00 /*INVALID SETTING*/;
		ICR3L  = 0x00 /*INVALID SETTING*/;
		TCCR3A = 0x00;
		TCCR3B = 0x06; //start Timer
	
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





ISR(USART1_RX_vect) // прерывания для UART
{
	
	// data1 = UART1_Receive();
	read_mass(UART1_Receive());
	 
}




ISR(TIMER0_COMP_vect)
{
 
 
 PORTC = 0xFF; // Гасим все разряды
 switch (n)
 {
	 case 0:
	 OffSEGMENT (PORT_IND2, Seg8 );
	 PORTC = ~ CIFRA1[0];
	 OnSEGMENT (PORT_IND1, Seg1);
	 break;
	 case 1:
	 OffSEGMENT(PORT_IND1,Seg1);
	 PORTC = ~ CIFRA1[1];
	 OnSEGMENT(PORT_IND1,Seg2);
	 break;
	 case 2:
	 OffSEGMENT (PORT_IND1, Seg2);
	 PORTC = ~ CIFRA1[2];
	 OnSEGMENT (PORT_IND1, Seg3);
	 break;
	 case 3:
	 OffSEGMENT (PORT_IND1, Seg3 );
	 PORTC = ~ CIFRA1[3];
	 OnSEGMENT (PORT_IND1, Seg4);
	 break;
	 case 4:
	 OffSEGMENT(PORT_IND1,Seg4);
	 PORTC = ~ CIFRA2[0];
	 OnSEGMENT(PORT_IND2,Seg5);
	 break;
	 case 5:
	 OffSEGMENT (PORT_IND2, Seg5);
	 PORTC = ~ CIFRA2[1];
	 OnSEGMENT (PORT_IND2, Seg6);
	 break;
	 case 6:
	 OffSEGMENT (PORT_IND2, Seg6);
	 PORTC = ~ CIFRA2[2]; //~(CIFRA[disp3 % 10]);
	 OnSEGMENT (PORT_IND2, Seg7);
	 break;
	 case 7:
	  OffSEGMENT (PORT_IND2, Seg7);
	  PORTC = ~ CIFRA2[3]; //~(CIFRA[disp3 % 10]);
	  OnSEGMENT (PORT_IND2, Seg8);
	 break;
 }
 if ((n++) > 7) n = 0;
 
}

ISR (TIMER2_COMP_vect)
{

	sek_tim++;
	if (sek_tim > 124)
	{
		
		ygol = TCNT1;
		TCNT1 = 0;
		azim = TCNT3;
		TCNT3 = 0;
		sek_tim = 0;
		izmmeren_tik = 1;
		obnov = 1;
	}
}



void CommandRI(void)   // функция проверки третьего символа - номера канала Тока
{
	if (glavmas[2] == 'A')  //  
	{
		
		hex(3,6);
		tok = FFFF;  // чтение  канала тока
		obnov = 1;
		Priem = 0;
		    
	}
	else if (glavmas[2] == 'B')  //
	{
		
		hex(3,6);
		tok2 = FFFF;  // чтение  канала тока
		disp1 = tok2;
		obnov = 1;
		Priem = 0;
		
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
	
}

void CommandRU(void)   // функция проверки третьего символа - номера канала напряжения
{
	if (glavmas[2] == 'A')  //  
	{
		hex(3,6);
		napr = FFFF;  // чтение  канала напряжения
		obnov = 1;
		Priem = 0;
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
	
}

void CommandRB(void)   // функция проверки третьего символа - номера канала компарирования
{
	if (glavmas[2] == 'A')  //
	{
		hex(3,6);
		porog1 = FFFF;  // чтение  1  канала
		disp1 = porog1;
		obnov = 1;
		Priem = 0;
	}
	else if (glavmas[2] == 'B')  //
	{
		hex(3,6);
		porog2 = FFFF;  // чтение  2  канала
		disp1 = porog2;
		obnov = 1;
		Priem = 0;
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
	
}

void CommandRO(void)   // функция проверки третьего символа - ВИД режима работы
{
	if (glavmas[2] == 'A')  //
	{
		regim = 1; //режим вольт - ампер - метра
		Priem = 0;
		PORTF SET_B (5);
		PORTF SET_B (7);
		PORTF CLR_B (6);
		PORTF CLR_B (4);
		
	}
	else if (glavmas[2] == 'B')  //
	{
		regim = 2; // режим частотомера
		Priem = 0;
		 PORTF SET_B (6);
		 PORTF SET_B (4);
		 PORTF CLR_B (5);
		 PORTF CLR_B (7);
	}
	else if (glavmas[2] == 'Z')  //
	{
		zadergka = 30; //задержка 30 секунд
		Priem = 0;
	}
	else if (glavmas[2] == 'K')  //
	{
		regim = 3; //режим калибровки
		if (glavmas[3] == 'A')
		{
			priznak = 1;
			obnov = 1;
			Priem = 0;
		}
		else if (glavmas [3] == 'B')
		{
			priznak = 2;
			obnov = 1;
			Priem = 0;
		}
		else if (glavmas [3] == 'C' )
		{
			priznak = 3;
			obnov = 1;
			Priem = 0;
		}
		else if (glavmas [3] == 'D')
		{
			priznak = 0;
			obnov = 1;
			Priem = 0;
		}
		
	}
	else if (glavmas[2] == 'T')  // режимы измерений
	{
		if (glavmas[3] == 'P')
		{
			start_izm = 0;
			toch_nep = 1;
			obnov = 1;
			Priem = 0;
		}
		else if (glavmas[3] == 'N')
		{
			toch_nep = 0;
			obnov = 1;
			Priem = 0;
			
		}
		else if (glavmas[3] == 'S')
		{
			
			start_izm = 1;
			obnov = 1;
			Priem = 0;
			
		}
	
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
	
}


void CommandR(void)  // Функция проверки второго сивола - команды чтения данных
{
	if (glavmas[1] == 'I')  //  Каналы чтения тока
	{
		CommandRI(); //
	}
	else if (glavmas[1] == 'U') // каналы чтения напряжения
	{
		CommandRU(); //
	}	
	else if (glavmas[1] == 'B') // каналы чтения уровней компарирования
	{
		CommandRB(); //
	}
	else if (glavmas[1] == 'O') // режимы работы
	{
		CommandRO(); //
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
}

int main(void)
{
	unsigned int tmp;
	cli ();
	PORT_Init();
	TIMER0_Init();
	TIMER1_Init();
	TIMER2_Init();
	TIMER3_Init();
	USART1_Init(103);// 9600
	regim = 1;
	priznak = 0;

	sei ();
	
	
	disp1 = 0000;
	CIFRA1 [0] = CIFRA[(disp1 % 10000 / 1000)];
	CIFRA1 [1] = CIFRA[(disp1 % 1000 / 100)];
	CIFRA1 [2] = CIFRA[(disp1 % 100 / 10)];
	CIFRA1 [3] = CIFRA[(disp1 % 10)];
	
	CIFRA2 [0] = CIFRA[(disp2 % 10000 / 1000)];
	CIFRA2 [1] = CIFRA[(disp2 % 1000 / 100)];
	CIFRA2 [2] = CIFRA[(disp2 % 100 / 10)];
	CIFRA2 [3] = CIFRA[(disp2 % 10)];
	
	_delay_us(1000);
	
    while(1)
    {
		_delay_us(100);
	   if (obnov == 1)
       {
		   if (regim == 1)
		   {
			  disp2 = tok; 
			  disp1 = napr; 
		   }
		   
		   else if ((regim == 2) && (toch_nep == 0))
		   {
			   if ( azim < 1000)
			   {
				   disp2 = 0;
			   }
			   else if (azim > 4600)
			   {
				    disp2 = 3600; 
			   }
			   else
			   {
				 disp2 = (azim - 1000);  
			   }
			   
			   
			   if (ygol < 1000)
			   {
				   disp1 = 0;
			   }
			   else
			   {
				    disp1 = (ygol - 1000);
			   }
			   
			   
	
				   
			  
	  
		   }
		   else if ((regim == 2) && (toch_nep == 1) && (izmmeren == 1))
		   {
			     if ( azim_t < 1000)
			     {
				     disp2 = 0;
			     }
			     else if (azim_t > 4600)
			     {
				     disp2 = 3600;
			     }
			     else
			     {
				     disp2 = (azim_t - 1000);
			     }
			     
			     
			     if (ygol_t < 1000)
			     {
				     disp1 = 0;
			     }
			     else
			     {
				     disp1 = (ygol_t - 1000);
			     }
			    
			 
			   izmmeren = 0;
		   }
		   
		   
		   
		   
		 
		 if ((start_izm == 1) && (izmmeren_tik == 1))
		 {
			 flag_izm = 1;
			 izmmeren_tik = 0;
			 summa_azim = (summa_azim + azim);
			 summa_ygol = (summa_ygol +ygol);
			 count_izm++;
			
		 }
		 else if ((start_izm == 0) &&(flag_izm == 1))
		 {
			 izmmeren = 1;
			 flag_izm = 0;
			 azim_t = summa_azim/count_izm;
			 ygol_t = summa_ygol/count_izm;
			 summa_azim = 0;
			 summa_ygol = 0;
			 count_izm = 0;
			
			 
		 }
		   
			   
		
	
		if (priznak == 0)
		{
			if (regim == 1)
			{
				if( disp1 < 10 )
				{   tmp = disp1 * 1000;
					CIFRA1 [0] = CIFRA[(tmp % 10000 / 1000)] | 0x80;
					CIFRA1 [1] = CIFRA[(tmp % 1000 / 100)];
					CIFRA1 [2] = CIFRA[(tmp % 100 / 10)];
					CIFRA1 [3] = CIFRA[(tmp % 10)];
				}
				else if ((disp1 >= 10) && (disp1 < 100))
				{
					tmp = disp1 * 100;
					CIFRA1 [0] = CIFRA[(tmp % 10000 / 1000)] ;
					CIFRA1 [1] = CIFRA[(tmp % 1000 / 100)] | 0x80;
					CIFRA1 [2] = CIFRA[(tmp % 100 / 10)];
					CIFRA1 [3] = CIFRA[(tmp % 10)];
				}
				else if ((disp1 >= 100) && (disp1 < 1000))
				{
					tmp = disp1 * 10;
					CIFRA1 [0] = CIFRA[(tmp % 10000 / 1000)] ;
					CIFRA1 [1] = CIFRA[(tmp % 1000 / 100)] ;
					CIFRA1 [2] = CIFRA[(tmp % 100 / 10)] | 0x80;
					CIFRA1 [3] = CIFRA[(tmp % 10)];
				}
				
				if( disp2 < 10 )
				{   tmp = disp2 * 1000;
					CIFRA2 [0] = CIFRA[(tmp % 10000 / 1000)] | 0x80;
					CIFRA2 [1] = CIFRA[(tmp % 1000 / 100)];
					CIFRA2 [2] = CIFRA[(tmp % 100 / 10)];
					CIFRA2 [3] = CIFRA[(tmp % 10)];
				}
				else if ((disp2 >= 10) && (disp2 < 100))
				{
					tmp = disp2 * 100;
					CIFRA2 [0] = CIFRA[(tmp % 10000 / 1000)] ;
					CIFRA2 [1] = CIFRA[(tmp % 1000 / 100)] | 0x80;
					CIFRA2 [2] = CIFRA[(tmp % 100 / 10)];
					CIFRA2 [3] = CIFRA[(tmp % 10)];
				}
				else if ((disp2 >= 100) && (disp2 < 1000))
				{
					tmp = disp2 * 10;
					CIFRA2 [0] = CIFRA[(tmp % 10000 / 1000)] ;
					CIFRA2 [1] = CIFRA[(tmp % 1000 / 100)] ;
					CIFRA2 [2] = CIFRA[(tmp % 100 / 10)] | 0x80;
					CIFRA2 [3] = CIFRA[(tmp % 10)];
				}
				
			}
			
			else if (regim == 2 )
			{
				
				if (disp1 / 100 < 10)
				{
					tmp = disp1*10;
					CIFRA1 [0] = CIFRA[(tmp % 10000 / 1000)] | 0x80 ;
					CIFRA1 [1] = CIFRA[(tmp % 1000 / 100)];
					CIFRA1 [2] = CIFRA[(tmp % 100 / 10)];
					CIFRA1 [3] = CIFRA[(tmp % 10)];
				}
				else if ((disp1 / 100 >= 10) && (disp1 / 100 < 99))
				{
					CIFRA1 [0] = CIFRA[(disp1 % 10000 / 1000)];
					CIFRA1 [1] = CIFRA[(disp1 % 1000 / 100)] | 0x80;
					CIFRA1 [2] = CIFRA[(disp1 % 100 / 10)];
					CIFRA1 [3] = CIFRA[(disp1 % 10)];
				}
			
				if (disp2 / 10 < 10 )
				{
					tmp = disp2 * 100;
					CIFRA2 [0] = CIFRA[(tmp % 10000 / 1000)]  | 0x80;
					CIFRA2 [1] = CIFRA[(tmp % 1000 / 100)];
					CIFRA2 [2] = CIFRA[(tmp % 100 / 10)];
					CIFRA2 [3] = CIFRA[(tmp % 10)];
				}
				else if ((disp2 / 10 >= 10) && (disp2 / 10 < 100) )
				{
					tmp = disp2 * 10;
					CIFRA2 [0] = CIFRA[(tmp % 10000 / 1000)]  ;
					CIFRA2 [1] = CIFRA[(tmp % 1000 / 100)]| 0x80;
					CIFRA2 [2] = CIFRA[(tmp % 100 / 10)];
					CIFRA2 [3] = CIFRA[(tmp % 10)];
				}
				else if ((disp2 / 10 >= 100) && (disp2 / 10 < 1000) )
				{
					tmp = disp2 ;
					CIFRA2 [0] = CIFRA[(tmp % 10000 / 1000)]  ;
					CIFRA2 [1] = CIFRA[(tmp % 1000 / 100)] ;
					CIFRA2 [2] = CIFRA[(tmp % 100 / 10)] | 0x80;
					CIFRA2 [3] = CIFRA[(tmp % 10)];
				}
				
			}

		}
		else if (priznak == 1)
		{
			if( disp1 < 10 )
			{   tmp = disp1 * 1000;
				CIFRA1 [0] = CIFRA[(tmp % 10000 / 1000)] | 0x80;
				CIFRA1 [1] = CIFRA[(tmp % 1000 / 100)];
				CIFRA1 [2] = CIFRA[(tmp % 100 / 10)];
				CIFRA1 [3] = CIFRA[(tmp % 10)];
			}
			else if ((disp1 >= 10) && (disp1 < 100))
			{
				tmp = disp1 * 100;
				CIFRA1 [0] = CIFRA[(tmp % 10000 / 1000)] ;
				CIFRA1 [1] = CIFRA[(tmp % 1000 / 100)] | 0x80;
				CIFRA1 [2] = CIFRA[(tmp % 100 / 10)];
				CIFRA1 [3] = CIFRA[(tmp % 10)];
			}
			else if ((disp1 >= 100) && (disp1 < 1000))
			{
				tmp = disp1 * 10;
				CIFRA1 [0] = CIFRA[(tmp % 10000 / 1000)] ;
				CIFRA1 [1] = CIFRA[(tmp % 1000 / 100)] ;
				CIFRA1 [2] = CIFRA[(tmp % 100 / 10)] | 0x80;
				CIFRA1 [3] = CIFRA[(tmp % 10)];
			}
			
			CIFRA2 [0] = SIMVOL[0];
			CIFRA2 [1] = SIMVOL[1];
			CIFRA2 [2] = SIMVOL[6];
			CIFRA2 [3] = SIMVOL[5];
		}
		else if (priznak == 2)
		{
			 
			 tmp = disp1 * 10;
			 CIFRA1 [0] = CIFRA[(tmp % 10000 / 1000)] | 0x80 ;
			 CIFRA1 [1] = CIFRA[(tmp % 1000 / 100)];
			 CIFRA1 [2] = CIFRA[(tmp % 100 / 10)];
			 CIFRA1 [3] = CIFRA[(tmp % 10)];
			 
			CIFRA2 [0] = SIMVOL[0];
			CIFRA2 [1] = SIMVOL[1];
			CIFRA2 [2] = SIMVOL[6];
			CIFRA2 [3] = SIMVOL[3];
		}
		else if (priznak == 3)
		{
			 tmp = disp1 * 10;
			 CIFRA1 [0] = CIFRA[(tmp % 10000 / 1000)] | 0x80 ;
			 CIFRA1 [1] = CIFRA[(tmp % 1000 / 100)];
			 CIFRA1 [2] = CIFRA[(tmp % 100 / 10)];
			 CIFRA1 [3] = CIFRA[(tmp % 10)];
			
			CIFRA2 [0] = SIMVOL[0];
			CIFRA2 [1] = SIMVOL[1];
			CIFRA2 [2] = SIMVOL[6];
			CIFRA2 [3] = SIMVOL[4];
		}
		
		obnov = 0;
      }
        
	 if (Priem == 1)
	 {
		 Priem = 0;
		 if (glavmas[0] == 'R')  // проверяем первый символ
		 {
			 CommandR();    // Группа команд чтения данных
		 }
		 else
		 {
			 UART1_Transmit (Vp);
			 Priem = 0;
		 } // {есть ошибка}
	 }
		 	
		
		
		
		
		
		
		//TODO:: Please write your application code 
    }
}