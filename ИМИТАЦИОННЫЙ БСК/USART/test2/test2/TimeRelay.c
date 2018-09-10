/*
 * TimeRelay.c
 *
 * Created: 29.10.2015 17:25:43
 *  Author: Weimer
 */ 

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Задача:
// 

#include <avr/io.h>
#include <util/delay.h>


#include "Timer.h"
#include "MC74xxx_SPI.h"
#include "DS1307.h"
//#include "ee24cXX.h"

#define OUT_PORT		PORTD
#define OUT_DDR			DDRD
#define LED_OUT_P		PORTD2
#define OUT0_P			PORTD3
#define OUT1_P			PORTD4
#define OUT2_P			PORTD5
#define OUT3_P			PORTD6	

#define  ON_OUT(out)	OUT_PORT |=   (1<<out)
#define OFF_OUT(out)	OUT_PORT &= (~(1<<out))

static uint8_t	TIM_Cnt0	=	0,
				TIM_Cnt1	=	0,
				TIM_Cnt2	=	0;
				//COMM_En	=	0;
				//COMM_Cnt	=	0; 
				
DS1307_InitTypeDef DS1307_Struct;

typedef struct Timer_InitTypeDef{
	uint8_t	SECONDS;
	uint8_t MINUTES;
	uint8_t HOURS;
} Timer_InitTypeDef;

Timer_InitTypeDef Timer_Struct;

void DS1307_TimeInit(DS1307_InitTypeDef *DS1307_InitDef)
{
	DS1307_Init(100000);
	DS1307_Read(&DS1307_Struct);
	if(DS1307_InitDef->SECONDS == 80){
		DS1307_InitDef->SECONDS = 1;
		DS1307_InitDef->MINUTES = 0;
		DS1307_InitDef->HOURS	= 0;
		DS1307_InitDef->DAY		= 0;
		DS1307_InitDef->DATE	= 0;
		DS1307_InitDef->MONTH	= 0;
		DS1307_InitDef->YEAR	= 0;
		DS1307_InitDef->CONTROL = 0;
		DS1307_Write(DS1307_InitDef);
	}
}

static uint8_t NUM_IND[12] = {  0x28,	//	0
								0xeb,	//	1
								0x32,	//	2
								0xa2,	//	3
								0xe1,	//	4
								0xa4,	//	5
								0x24,	//	6
								0xea,	//	7
								0x20,	//	8
								0xa0,	//	9
								0xdf,	//	.
								0xff	// ' '
							 };
static uint8_t SEG_IND[4] = {	
								0xf8,	// S1
								0xf4,	// S2
								0xf2,	// S3
								0xf1	// S4
							};

#define		Menu_Max		4
uint16_t	Button		=	0x00;
uint16_t	Menu		=	0xff;
uint8_t		Menu_Inc	=	3;
uint8_t		Menu_T		=	0;

#define		Menu_A		0xff
#define		Menu_A1		0xef
#define		Menu_A2		0xdf
#define		Menu_A3		0xbf

#define		Menu_B		0x7f
#define		Menu_B1		0x6f
#define		Menu_B2		0x5f
#define		Menu_B3		0x3f


#define		Btn_ENTER		0x04
#define		Btn_ESCAPE		0x08	
#define		Btn_UP			0x01
#define		Btn_DOWN		0x02

uint16_t	IND_Buf[3];
uint16_t ind_tmp = 255;
uint8_t		IND_Cnt = 4;

uint8_t ON_Flag = 0;
					
void IND_Write(uint16_t *net)
{
	uint8_t buf[2];
	//uint8_t tmp = 0xff;
	//if(Menu > 0)	
		//tmp&=(~(1<<(Menu + 3 + Menu_T)));
	buf[0] = (SEG_IND[IND_Cnt - 1]) & Menu;
	buf[1] = (NUM_IND[net[0]%10]);
	if(IND_Cnt == 2) buf[1] &= NUM_IND[10];
	MC_595_Write(buf,2);
	net[0]/=10;
	IND_Cnt--;
	if(IND_Cnt <= 0){
		IND_Cnt = 4;
		net[0] = net[1];
	}	
}

void TIM_callback(void)
{
	TIM_Cnt0++;
	TIM_Cnt1++;
	TIM_Cnt2++;
	if(TIM_Cnt0 >= 1){//8
		IND_Write(IND_Buf);
		TIM_Cnt0 = 0;
	}
	if(TIM_Cnt1 >= 50  && Button == 0x00){
			Button = MC_165_Read();
			TIM_Cnt1 = 0;
	}
	if(Menu > 0x7f){
		if(TIM_Cnt2<=100) {
			NUM_IND[10] = 0xff;
		}
		else NUM_IND[10] = 0xdf;
		if(TIM_Cnt2 >= 200) {
			DS1307_Read(&DS1307_Struct);	
			TIM_Cnt2 = 0;
		}
	}
}

void Menu_SP()
{
	switch(Menu){
		case Menu_A: {
			Menu = 0x7f;
			break;
		}
		case Menu_A1:{
			Timer_Struct.SECONDS++;
			if(Timer_Struct.SECONDS >= 60 ) Timer_Struct.SECONDS = 0;
			break;
		}
		case Menu_A2:{
			Timer_Struct.MINUTES++;
			if(Timer_Struct.MINUTES >= 60) Timer_Struct.MINUTES = 0;

			break;
		}
		case Menu_A3:{
			Timer_Struct.HOURS++;
			if(Timer_Struct.HOURS >= 24) Timer_Struct.HOURS = 0;
			break;
		}
		case Menu_B1:{
			DS1307_Struct.SECONDS++;
			if(DS1307_Struct.SECONDS >= 60 ) DS1307_Struct.SECONDS = 0;
			break;
		}
		case Menu_B2:{
			DS1307_Struct.MINUTES++;
			if(DS1307_Struct.MINUTES >= 60) DS1307_Struct.MINUTES = 0;
			break;
		}
		case Menu_B3:{
			DS1307_Struct.HOURS++;
			if(DS1307_Struct.HOURS >= 24) DS1307_Struct.HOURS = 0;
			break;
		}
	}
}

void Menu_SM(void)
{
	switch(Menu){
		case Menu_B:{
			Menu = Menu_A;
			break;
		}
		case Menu_A1:{
			if(Timer_Struct.SECONDS == 0 ) Timer_Struct.SECONDS = 60;
			Timer_Struct.SECONDS--;
			break;
		}
		case Menu_A2:{
			if(Timer_Struct.MINUTES == 0) Timer_Struct.MINUTES = 60;
			Timer_Struct.MINUTES--;
			break;
		}
		case Menu_A3:{
			if(Timer_Struct.HOURS == 0) Timer_Struct.HOURS = 24;
			Timer_Struct.HOURS--;
			break;
		}
		case Menu_B1:{
			if(DS1307_Struct.SECONDS == 0 ) DS1307_Struct.SECONDS = 60;
			DS1307_Struct.SECONDS--;
			break;
		}
		case Menu_B2:{
			if(DS1307_Struct.MINUTES == 0) DS1307_Struct.MINUTES = 60;
			DS1307_Struct.MINUTES--;
			break;
		}
		case Menu_B3:{
			if(DS1307_Struct.HOURS == 0) DS1307_Struct.HOURS = 24;
			DS1307_Struct.HOURS--;
			break;
		}
	}
}

int main(void)
{
	DDRD = (1<<DDD0)|(1<<DDD1)|(1<<DDD2)|(1<<DDD3)|(1<<DDD4)|(1<<DDD5)|(1<<DDD6);
	PORTD = (1<<PORTD0)|(1<<PORTD1);
	MC_Init();
	DS1307_TimeInit(&DS1307_Struct);
	_delay_ms(1000);
	PORTD |= (1<<LED_OUT_P);
	TIM_Delay_Init();
	//uint16_t Button_		= 0x00;
	uint32_t sec = 0;
    while(1){	
		_delay_ms(13);
		if(ON_Flag == 1){
			uint32_t sec_t = DS1307_Struct.HOURS * 3600 + DS1307_Struct.MINUTES*60 + DS1307_Struct.SECONDS;
			uint8_t hr	= (sec - sec_t)/3600;
			uint8_t min = ((sec - sec_t)%3600)/60;
			
			if(hr == 0)	IND_Buf[1] = (min*100) + (sec - sec_t) - (min*60);
			else		IND_Buf[1] = (hr*100) + min;
			if((sec - sec_t) == 0) {
				ON_Flag = 0;
				OFF_OUT(OUT0_P);
			}
		}
		else {
			if(Menu == Menu_A || Menu == Menu_B) IND_Buf[1] = DS1307_Struct.HOURS*100 + DS1307_Struct.MINUTES;
			else if((Menu == Menu_A1) || (Menu == Menu_A2 && Timer_Struct.HOURS == 0)) IND_Buf[1] = Timer_Struct.MINUTES*100 + Timer_Struct.SECONDS;
			else if((Menu == Menu_A2 && Timer_Struct.HOURS != 0) || (Menu == Menu_A3)) IND_Buf[1] = Timer_Struct.HOURS*100 + Timer_Struct.MINUTES;
			else if( Menu == Menu_B1) IND_Buf[1] = DS1307_Struct.MINUTES*100 + DS1307_Struct.SECONDS;
			else if((Menu == Menu_B2) || (Menu == Menu_B3)) IND_Buf[1] = DS1307_Struct.HOURS*100 + DS1307_Struct.MINUTES;
		}
		//IND_Buf[1] = Menu;
		switch(Button){
			case 0xF1:{	
				Menu_SP();
				break;
			}
			case 0xF4:{
				Menu_SM();
				break;
			}
			case 0xF2:{
				if(Menu == Menu_A) Menu = Menu_A1;
				else if(Menu == Menu_A1) Menu = Menu_A2;
				else if(Menu == Menu_A2) Menu = Menu_A3;
				else if(Menu == Menu_A3) Menu = Menu_A;
				else if(Menu == Menu_B)  Menu = Menu_B1;
				else if(Menu == Menu_B1) Menu = Menu_B2;
				else if(Menu == Menu_B2) Menu = Menu_B3;
				else if(Menu == Menu_B3) Menu = Menu_B;
				break;
			}
			case 0xFC:{
				//ON_Flag = 0;
				break;
			}
			case 0xF8:{
				if(Menu > 0x7f){
					DS1307_Read(&DS1307_Struct);
					sec = ((Timer_Struct.HOURS   + DS1307_Struct.HOURS)   * 3600) + \
						  ((Timer_Struct.MINUTES + DS1307_Struct.MINUTES) * 60)   + \
						  ( Timer_Struct.SECONDS + DS1307_Struct.SECONDS);
					ON_Flag = 1;
					ON_OUT(OUT0_P);
					Menu = 0xff;
				}
				else {
					_delay_ms(500);
					DS1307_Write(&DS1307_Struct); 
					Menu = 0xff;
					_delay_ms(500);
					DS1307_Read(&DS1307_Struct);
				}
				break;
			}
			default: break;
		}
		Button = 0x00;
    }
}