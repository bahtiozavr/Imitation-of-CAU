/*
 * GIROS.c
 *
 * Created: 15.11.2016 13:02:34
 *  Author: Andrey
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "asciim.h"
#include "I2CGF.h"


#define SET_B(x) |= (1<<x)
#define CLR_B(x) &=~(1<<x)
#define INV_B(x) ^=(1<<(x))


char sek_tim, Izmeren;
unsigned int count0, CountP, CountRez, TikTim2, TikTim1;



signed char TEMPER_h;
signed char TEMPER_l;
signed long int t;

unsigned char ACCEL_xh;
unsigned char ACCEL_xl;
unsigned char ACCEL_yh;
unsigned char ACCEL_yl;
unsigned char ACCEL_zh;
unsigned char ACCEL_zl;
long int ACCEL_X;
long int ACCEL_Y;
long int ACCEL_Z;

long int ACCEL_XANGLE;
long int ACCEL_YANGLE;
long int ACCEL_ZANGLE;

unsigned char GYRO_xh;
unsigned char GYRO_xl;
unsigned char GYRO_yh;
unsigned char GYRO_yl;
unsigned char GYRO_zh;
unsigned char GYRO_zl;
long int GYRO_X;
long int GYRO_Y;
long int GYRO_Z;
long int Rez_GYRO_Z;

long int GYRO_ANGLE_X=0;
long int GYRO_ANGLE_Y=0;
long int GYRO_ANGLE_Z=0;

unsigned char who;
float w, x, y, z, GIRO_X_ANGELL, GIRO_X_ANGELL, GIRO_X_ANGELL;

// ========================================================================================================================
// ========================================================================================================================
// MPU6050 - гироскоп и акселерометр

#define MPU6050_RA_XG_OFFS_TC 0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_YG_OFFS_TC 0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_ZG_OFFS_TC 0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_X_FINE_GAIN 0x03 //[7:0] X_FINE_GAIN
#define MPU6050_RA_Y_FINE_GAIN 0x04 //[7:0] Y_FINE_GAIN
#define MPU6050_RA_Z_FINE_GAIN 0x05 //[7:0] Z_FINE_GAIN
#define MPU6050_RA_XA_OFFS_H 0x06 //[15:0] XA_OFFS
#define MPU6050_RA_XA_OFFS_L_TC 0x07
#define MPU6050_RA_YA_OFFS_H 0x08 //[15:0] YA_OFFS
#define MPU6050_RA_YA_OFFS_L_TC 0x09
#define MPU6050_RA_ZA_OFFS_H 0x0A //[15:0] ZA_OFFS
#define MPU6050_RA_ZA_OFFS_L_TC 0x0B
#define MPU6050_RA_XG_OFFS_USRH 0x13 //[15:0] XG_OFFS_USR
#define MPU6050_RA_XG_OFFS_USRL 0x14
#define MPU6050_RA_YG_OFFS_USRH 0x15 //[15:0] YG_OFFS_USR
#define MPU6050_RA_YG_OFFS_USRL 0x16
#define MPU6050_RA_ZG_OFFS_USRH 0x17 //[15:0] ZG_OFFS_USR
#define MPU6050_RA_ZG_OFFS_USRL 0x18
#define MPU6050_RA_SMPLRT_DIV 0x19
#define MPU6050_RA_CONFIG 0x1A
#define MPU6050_RA_GYRO_CONFIG 0x1B
#define MPU6050_RA_ACCEL_CONFIG 0x1C
#define MPU6050_RA_FF_THR 0x1D
#define MPU6050_RA_FF_DUR 0x1E
#define MPU6050_RA_MOT_THR 0x1F
#define MPU6050_RA_MOT_DUR 0x20
#define MPU6050_RA_ZRMOT_THR 0x21
#define MPU6050_RA_ZRMOT_DUR 0x22
#define MPU6050_RA_FIFO_EN 0x23
#define MPU6050_RA_I2C_MST_CTRL 0x24
#define MPU6050_RA_I2C_SLV0_ADDR 0x25
#define MPU6050_RA_I2C_SLV0_REG 0x26
#define MPU6050_RA_I2C_SLV0_CTRL 0x27
#define MPU6050_RA_I2C_SLV1_ADDR 0x28
#define MPU6050_RA_I2C_SLV1_REG 0x29
#define MPU6050_RA_I2C_SLV1_CTRL 0x2A
#define MPU6050_RA_I2C_SLV2_ADDR 0x2B
#define MPU6050_RA_I2C_SLV2_REG 0x2C
#define MPU6050_RA_I2C_SLV2_CTRL 0x2D
#define MPU6050_RA_I2C_SLV3_ADDR 0x2E
#define MPU6050_RA_I2C_SLV3_REG 0x2F
#define MPU6050_RA_I2C_SLV3_CTRL 0x30
#define MPU6050_RA_I2C_SLV4_ADDR 0x31
#define MPU6050_RA_I2C_SLV4_REG 0x32
#define MPU6050_RA_I2C_SLV4_DO 0x33
#define MPU6050_RA_I2C_SLV4_CTRL 0x34
#define MPU6050_RA_I2C_SLV4_DI 0x35
#define MPU6050_RA_I2C_MST_STATUS 0x36
#define MPU6050_RA_INT_PIN_CFG 0x37
#define MPU6050_RA_INT_ENABLE 0x38
#define MPU6050_RA_DMP_INT_STATUS 0x39
#define MPU6050_RA_INT_STATUS 0x3A
#define MPU6050_RA_ACCEL_XOUT_H 0x3B
#define MPU6050_RA_ACCEL_XOUT_L 0x3C
#define MPU6050_RA_ACCEL_YOUT_H 0x3D
#define MPU6050_RA_ACCEL_YOUT_L 0x3E
#define MPU6050_RA_ACCEL_ZOUT_H 0x3F
#define MPU6050_RA_ACCEL_ZOUT_L 0x40
#define MPU6050_RA_TEMP_OUT_H 0x41
#define MPU6050_RA_TEMP_OUT_L 0x42
#define MPU6050_RA_GYRO_XOUT_H 0x43
#define MPU6050_RA_GYRO_XOUT_L 0x44
#define MPU6050_RA_GYRO_YOUT_H 0x45
#define MPU6050_RA_GYRO_YOUT_L 0x46
#define MPU6050_RA_GYRO_ZOUT_H 0x47
#define MPU6050_RA_GYRO_ZOUT_L 0x48
#define MPU6050_RA_EXT_SENS_DATA_00 0x49
#define MPU6050_RA_EXT_SENS_DATA_01 0x4A
#define MPU6050_RA_EXT_SENS_DATA_02 0x4B
#define MPU6050_RA_EXT_SENS_DATA_03 0x4C
#define MPU6050_RA_EXT_SENS_DATA_04 0x4D
#define MPU6050_RA_EXT_SENS_DATA_05 0x4E
#define MPU6050_RA_EXT_SENS_DATA_06 0x4F
#define MPU6050_RA_EXT_SENS_DATA_07 0x50
#define MPU6050_RA_EXT_SENS_DATA_08 0x51
#define MPU6050_RA_EXT_SENS_DATA_09 0x52
#define MPU6050_RA_EXT_SENS_DATA_10 0x53
#define MPU6050_RA_EXT_SENS_DATA_11 0x54
#define MPU6050_RA_EXT_SENS_DATA_12 0x55
#define MPU6050_RA_EXT_SENS_DATA_13 0x56
#define MPU6050_RA_EXT_SENS_DATA_14 0x57
#define MPU6050_RA_EXT_SENS_DATA_15 0x58
#define MPU6050_RA_EXT_SENS_DATA_16 0x59
#define MPU6050_RA_EXT_SENS_DATA_17 0x5A
#define MPU6050_RA_EXT_SENS_DATA_18 0x5B
#define MPU6050_RA_EXT_SENS_DATA_19 0x5C
#define MPU6050_RA_EXT_SENS_DATA_20 0x5D
#define MPU6050_RA_EXT_SENS_DATA_21 0x5E
#define MPU6050_RA_EXT_SENS_DATA_22 0x5F
#define MPU6050_RA_EXT_SENS_DATA_23 0x60
#define MPU6050_RA_MOT_DETECT_STATUS 0x61
#define MPU6050_RA_I2C_SLV0_DO 0x63
#define MPU6050_RA_I2C_SLV1_DO 0x64
#define MPU6050_RA_I2C_SLV2_DO 0x65
#define MPU6050_RA_I2C_SLV3_DO 0x66
#define MPU6050_RA_I2C_MST_DELAY_CTRL 0x67
#define MPU6050_RA_SIGNAL_PATH_RESET 0x68
#define MPU6050_RA_MOT_DETECT_CTRL 0x69
#define MPU6050_RA_USER_CTRL 0x6A
#define MPU6050_RA_PWR_MGMT_1 0x6B
#define MPU6050_RA_PWR_MGMT_2 0x6C
#define MPU6050_RA_BANK_SEL 0x6D
#define MPU6050_RA_MEM_START_ADDR 0x6E
#define MPU6050_RA_MEM_R_W 0x6F
#define MPU6050_RA_DMP_CFG_1 0x70
#define MPU6050_RA_DMP_CFG_2 0x71
#define MPU6050_RA_FIFO_COUNTH 0x72
#define MPU6050_RA_FIFO_COUNTL 0x73
#define MPU6050_RA_FIFO_R_W 0x74
#define MPU6050_RA_WHO_AM_I 0x75

#define MPU6050_R 0b11010001
#define MPU6050_W 0b11010000

// запись I2C
void I2C_write (unsigned char reg, unsigned char value) 
{

	i2c_start_cond();
	i2c_send_byte(MPU6050_W);
	i2c_send_byte(reg);
	i2c_send_byte(value);
	i2c_stop_cond();
}

// инициализация, конфигурация
void Init_6050 (void) 
{

	//Sets sample rate to 8000/1+7 = 1000Hz
	I2C_write (MPU6050_RA_SMPLRT_DIV, 0x07);

	//Disable FSync, 256Hz DLPF
	I2C_write (MPU6050_RA_CONFIG, 0x00);

	//Disable gyro self tests, scale of 2000 degrees/s
	I2C_write (MPU6050_RA_GYRO_CONFIG, 0b00011000);

	//Disable accel self tests, scale of +-2g, no DHPF
	I2C_write (MPU6050_RA_ACCEL_CONFIG,0b00000000);

	//Freefall threshold of |0mg|
	I2C_write (MPU6050_RA_FF_THR, 0x00);

	//Freefall duration limit of 0
	I2C_write (MPU6050_RA_FF_DUR, 0x00);

	//Motion threshold of 0mg
	I2C_write (MPU6050_RA_MOT_THR, 0x00);

	//Motion duration of 0s
	I2C_write (MPU6050_RA_MOT_DUR, 0x00);

	//Zero motion threshold
	I2C_write (MPU6050_RA_ZRMOT_THR, 0x00);

	//Zero motion duration threshold
	I2C_write (MPU6050_RA_ZRMOT_DUR, 0x00);

	//Disable sensor output to FIFO buffer
	I2C_write (MPU6050_RA_FIFO_EN, 0x00);



	//AUX I2C setup
	//Sets AUX I2C to single master control, plus other config
	I2C_write (MPU6050_RA_I2C_MST_CTRL, 0x00);
	//Setup AUX I2C slaves
	I2C_write (MPU6050_RA_I2C_SLV0_ADDR, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV0_REG, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV0_CTRL, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV1_ADDR, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV1_REG, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV1_CTRL, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV2_ADDR, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV2_REG, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV2_CTRL, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV3_ADDR, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV3_REG, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV3_CTRL, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV4_ADDR, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV4_REG, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV4_DO, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV4_CTRL, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV4_DI, 0x00);
	//MPU6050_RA_I2C_MST_STATUS //Read-only
	//Setup INT pin and AUX I2C pass through
	I2C_write (MPU6050_RA_INT_PIN_CFG, 0x00);
	//Enable data ready interrupt
	I2C_write (MPU6050_RA_INT_ENABLE, 0x00);


	//Slave out, dont care
	I2C_write (MPU6050_RA_I2C_SLV0_DO, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV1_DO, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV2_DO, 0x00);
	I2C_write (MPU6050_RA_I2C_SLV3_DO, 0x00);
	//More slave config
	I2C_write (MPU6050_RA_I2C_MST_DELAY_CTRL, 0x00);
	//Reset sensor signal paths
	I2C_write (MPU6050_RA_SIGNAL_PATH_RESET, 0x00);
	//Motion detection control
	I2C_write (MPU6050_RA_MOT_DETECT_CTRL, 0x00);
	//Disables FIFO, AUX I2C, FIFO and I2C reset bits to 0
	I2C_write (MPU6050_RA_USER_CTRL, 0x00);
	//Sets clock source to gyro reference w/ PLL
	I2C_write (MPU6050_RA_PWR_MGMT_1, 0b00000010);
	//Controls frequency of wakeups in accel low power mode plus the sensor standby modes
	I2C_write (MPU6050_RA_PWR_MGMT_2, 0x00);


/*
	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_W);       // передача адреса устройства, режим записи
	i2c_send_byte(0x19);	        // передача адреса памяти
	i2c_send_byte(0b00000000);      // Register 25
	i2c_send_byte(0b00000110);      // Register 26
	i2c_send_byte(0b11110000);      // Register 27
	i2c_send_byte(0b11110000);      // Register 28
	i2c_stop_cond();                // остановка i2c

	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_W);       // передача адреса устройства, режим записи
	i2c_send_byte(0x1F);	        // передача адреса памяти
	i2c_send_byte(250);      // Register 31
	i2c_stop_cond();                // остановка i2c

	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_W);       // передача адреса устройства, режим записи
	i2c_send_byte(0x6A);	        // передача адреса памяти
	i2c_send_byte(0b00000000);      // Register 106
	i2c_send_byte(0b00100000);      // Register 107
	i2c_send_byte(0b01000000);      // Register 108
	i2c_stop_cond();                // остановка i2c
*/
}

// акселерометр
void GET_ACCEL () 
{
	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_W);       // передача адреса устройства, режим записи
	i2c_send_byte(0x3B);	        // передача адреса памяти
	i2c_stop_cond();                // остановка i2c

	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_R);       // передача адреса устройства, режим чтения
	ACCEL_xh = i2c_get_byte(0);     // читаем данные
	ACCEL_xl = i2c_get_byte(0);
	ACCEL_yh = i2c_get_byte(0);
	ACCEL_yl = i2c_get_byte(0);
	ACCEL_zh = i2c_get_byte(0);
	ACCEL_zl = i2c_get_byte(1);
	i2c_stop_cond();                // остановка i2c

	ACCEL_X = ACCEL_xh*256 + ACCEL_xl;
	ACCEL_Y = ACCEL_yh*256 + ACCEL_yl;
	ACCEL_Z = ACCEL_zh*256 + ACCEL_zl;
}

// гироскоп
void GET_GYRO ()
 {
	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_W);       // передача адреса устройства, режим записи
	i2c_send_byte(0x43);	        // передача адреса памяти
	i2c_stop_cond();                // остановка i2c

	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_R);       // передача адреса устройства, режим чтения
	GYRO_xh = i2c_get_byte(0);      // читаем данные
	GYRO_xl = i2c_get_byte(0);
	GYRO_yh = i2c_get_byte(0);
	GYRO_yl = i2c_get_byte(0);
	GYRO_zh = i2c_get_byte(0);
	GYRO_zl = i2c_get_byte(1);
	i2c_stop_cond();                // остановка i2c

	GYRO_X = ((GYRO_xh) << 8) + GYRO_xl;
	GYRO_Y = ((GYRO_yh) << 8) + GYRO_yl;
	GYRO_Z = ((GYRO_zh) << 8) + GYRO_zl;
}

// температура
void GET_TEMPER ()
 {
	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_W);       // передача адреса устройства, режим записи
	i2c_send_byte(0x41);	        // передача адреса памяти
	i2c_stop_cond();                // остановка i2c

	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_R);       // передача адреса устройства, режим чтения
	TEMPER_h = i2c_get_byte(0);           // читаем данные
	TEMPER_l = i2c_get_byte(1);
	i2c_stop_cond();                // остановка i2c
}

// идентификатор (должно быть значение 104)
void WHO_AM_I () 
{
	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_W);       // передача адреса устройства, режим записи
	i2c_send_byte(0x75);	        // передача адреса памяти
	i2c_stop_cond();                // остановка i2c

	i2c_start_cond();               // запуск i2c
	i2c_send_byte(MPU6050_R);       // передача адреса устройства, режим чтения
	who = i2c_get_byte(1);
	i2c_stop_cond();                // остановка i2c
}

void PORT_Init (void)
{
	PORTA = 0x00;
	DDRA  = 0xFF;
	PORTB = 0x00;
	DDRB  = 0xFF;
	PORTC = 0x00; //m103 output only
	DDRC  = 0xFF;
	PORTD = 0x00;
	DDRD  = 0x00;
	PORTE = 0x00;
	DDRE  = 0xFF;
	PORTF = 0x00;
	DDRF  = 0xf0;
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

void TIMER0_Init (void)
{
	// режим таймера
	TCCR0 = 0x00; //stop
	ASSR  = 0x00; //set async mode
	TCNT0 = 0x00; //set count
	OCR0  = 0xF9;
	TCCR0 = (1<<CS02)|(1<<CS01)|(1<<CS00) | (1<<WGM01) | (0<<WGM00); //start timer делим на 1024 очищаем по совпадению
	TIMSK |= (1 << OCIE0); // разрешить прерывание таймера по совпадению
}

void TIMER1_Init(void)
{
	
	TCCR1B = 0x00; //stop
	TCNT1H = 0x00 /*INVALID SETTING*/; //setup
	TCNT1L = 0x00 /*INVALID SETTING*/;
	OCR1AH = 0x02 /*INVALID SETTING*/;
	OCR1AL = 0x71 /*INVALID SETTING*/;
	OCR1BH = 0x00 /*INVALID SETTING*/;
	OCR1BL = 0x00 /*INVALID SETTING*/;
	OCR1CH = 0x00 /*INVALID SETTING*/;
	OCR1CL = 0x00 /*INVALID SETTING*/;
	ICR1H  = 0x00 /*INVALID SETTING*/;
	ICR1L  = 0x00 /*INVALID SETTING*/;
	TCCR1A = 0x00;
	TCCR1B = (1<<CS12) | (0<<CS11) | (0<<CS10) | (1<<WGM12) | (0<<WGM13); //start Timer
	TIMSK |= (1 << OCIE1A);
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

ISR(TIMER0_COMP_vect)
{
	count0++;
	if (count0 == 30)
	{
		PORTC INV_B(5); 
		/*
		t=TEMPER_h*256;
		t=t+TEMPER_l+12412;
		t=t/340;
		//UART1_Transmit('T');
		ascii_8 ((t) >> 8);
		ascii_8 (t);
	    
		UART1_Transmit (CR);
		//UART1_Transmit('X');
		ascii_8 ((GYRO_X) >> 8);
		ascii_8 (GYRO_X);
		UART1_Transmit (CR);
		//UART1_Transmit('Y');
		ascii_8 ((GYRO_Y) >> 8);
		ascii_8 (GYRO_Y);
		UART1_Transmit (CR);
		//UART1_Transmit('Z');
		ascii_8 ((GYRO_Z) >> 8);
		ascii_8 (GYRO_Z);
		UART1_Transmit (CR);
		*/
		count0 = 0;
	}
}
ISR(TIMER1_COMPA_vect)
{
	TikTim1++;
	Izmeren = 1;
}


ISR (TIMER2_COMP_vect)
{
	sek_tim++;
	if (sek_tim > 124)
	{
		TikTim2 = TikTim1;
		TikTim1 = 0;
		Rez_GYRO_Z = GYRO_ANGLE_Z/CountP;
		sek_tim = 0;
		CountRez = CountP;
		CountP = 0;
	}
}

ISR(USART1_RX_vect) // прерывания для UART
{
	read_mass(UART1_Receive());
	PORTC INV_B(7); 
}

void CommandXA(void)//  функция чтения расчетного значения угла акселерометра ось Х
{
	ascii_8 ((ACCEL_XANGLE) >> 8);
	ascii_8 (ACCEL_XANGLE);
	UART1_Transmit (CR);
}
void CommandXB(void)//  функция 
{
	ascii_8 ((ACCEL_X) >> 8);
	ascii_8 (ACCEL_X);
	UART1_Transmit (CR);
}

void CommandXC(void)//  функция
{
	ascii_8 ((GYRO_X) >> 8);
	ascii_8 (GYRO_X);
	UART1_Transmit (CR);
}

void CommandXD(void)//  функция
{
	ascii_8 ((CountRez) >> 8);
	ascii_8 (CountRez);
	UART1_Transmit (CR);
}

void CommandYA(void)//   функция чтения расчетного значения угла акселерометра ось Y
{
	ascii_8 ((ACCEL_YANGLE) >> 8);
	ascii_8 (ACCEL_YANGLE);
	UART1_Transmit (CR);
}
void CommandYB(void)//  функция
{
	ascii_8 ((ACCEL_Y) >> 8);
	ascii_8 (ACCEL_Y);
	UART1_Transmit (CR);
}

void CommandYC(void)//  функция
{
	ascii_8 ((GYRO_Y) >> 8);
	ascii_8 (GYRO_Y);
	UART1_Transmit (CR);
}

void CommandYD(void)//  функция
{
	ascii_8 ((TikTim2) >> 8);
	ascii_8 (TikTim2);
	UART1_Transmit (CR);
}


void CommandZA(void) //   функция чтения расчетного значения угла акселерометра ось Z
{
	ascii_8 ((ACCEL_ZANGLE) >> 8);
	ascii_8 (ACCEL_ZANGLE);
	UART1_Transmit (CR);
}
void CommandZB(void)//  функция
{
	ascii_8 ((ACCEL_Z) >> 8);
	ascii_8 (ACCEL_Z);
	UART1_Transmit (CR);
}

void CommandZC(void)//  функция
{
	ascii_8 ((GYRO_Z) >> 8);
	ascii_8 (GYRO_Z);
	UART1_Transmit (CR);
}

void CommandZD(void)//  функция
{
	ascii_8 ((Rez_GYRO_Z) >> 8);
	ascii_8 (Rez_GYRO_Z);
	UART1_Transmit (CR);
	
}

void CommandTA(void) //  Функция чтения канала температуры
{
	t=TEMPER_h*256;
	t=t+TEMPER_l+12412;// выяснить 12412
	t=t/340;
	//UART1_Transmit('T');
	ascii_8 ((t) >> 8);
	ascii_8 (t);
	UART1_Transmit (CR);
}


void CommandRX(void)   // функция проверки третьего символа - номера канала X Акселерометра - Гироскопа 
{
	if (glavmas[2] == 'A')  //  Каналы чтения температуры
	{
		CommandXA(); // чтение канала X  угол акселерометра
	}
	else if (glavmas[2] == 'B')
	{
		CommandXB();
	}
	else if (glavmas[2] == 'C')
	{
		CommandXC();
	}
	else if (glavmas[2] == 'D')
	{
		CommandXD();
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
	
}


void CommandRY(void)   // функция проверки третьего символа - номера канала Y GIRO
{
	if (glavmas[2] == 'A')  //  Каналы чтения температуры
	{
		CommandYA(); // чтение канала Y Giro
	}
	else if (glavmas[2] == 'B')
	{
		CommandYB();
	}
	else if (glavmas[2] == 'C')
	{
		CommandYC();
	}
	else if (glavmas[2] == 'D')
	{
		CommandYD();
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
	
}


void CommandRZ(void)   // функция проверки третьего символа - номера канала Z GIRO
{
	if (glavmas[2] == 'A')  //  Каналы чтения температуры
	{
		CommandZA(); // чтение канала Z Giro
	}
	
	else if (glavmas[2] == 'B')
	{
		CommandZB();
	}
	else if (glavmas[2] == 'C')
	{
		CommandZC();
	}
	else if (glavmas[2] == 'D')
	{
		CommandZD();
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
	
}

void CommandRT(void)   // функция проверки третьего символа - номера канала температуры
{
	if (glavmas[2] == 'A')  //  Каналы чтения температуры
	{
		CommandTA(); // чтение канала температуры
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
	
}


void CommandR(void)  // Функция проверки второго сивола - команды чтения данных
{
	if (glavmas[1] == 'X')  //  Каналы чтения оси Х
	{
		CommandRX(); //
	}
	else if (glavmas[1] == 'Y') // Каналы чтения оси Y
	{
		CommandRY(); //
	}
	else if (glavmas[1] == 'Z') // Каналы чтения оси Z
	{
		CommandRZ(); //
	}
	else if (glavmas[1] == 'T') // каналы чтения температуры
	{
		CommandRT(); //
	}
	else
	{
		UART1_Transmit (Vp);
		Priem = 0;
	}
}


void CommandW(void) // Функция проверки второго сивола команды - чтения записи данных
{
	
}




int main(void)
{
	
	cli ();
	PORT_Init();
	TIMER0_Init();
	TIMER1_Init();
	TIMER2_Init();
	USART1_Init(12);//9600- 103. 76800-12
	_delay_ms(1000);
	_delay_ms(1000);
	i2c_init();
	_delay_ms(1000);
	_delay_ms(1000);
	Init_6050();
	_delay_ms(1000);
	Init_6050();
	sei(); // Разрешить прерывания
	
	float tmp1 = 0, tmp2 = 0;
	
    while(1)
	
    {
		
		CountP++;// числоциклов выполнения программы.
				
		
		
		
		if (Izmeren == 1)
		{
			GET_GYRO ();
			
			
			tmp1 = GYRO_Z;
			//tmp2 = Rez_GYRO_Z;
			tmp2 = tmp2 + (tmp1*0.061*0.01);
			
			Rez_GYRO_Z = tmp2;
			
			GET_ACCEL();
			GET_TEMPER();
			Izmeren = 0;
		}
		
		
		
	//	GET_GYRO();
		
	
		ACCEL_XANGLE = 57.29578*atan((float)-ACCEL_X/ sqrt(pow((float)ACCEL_Y,2)+pow((float)ACCEL_Z,2)))*10;
		ACCEL_YANGLE = 57.29578*atan((float)-ACCEL_Y/ sqrt(pow((float)ACCEL_X,2)+pow((float)ACCEL_Z,2)))*10;
		ACCEL_ZANGLE = 57.29578*atan((float)-ACCEL_Z/ sqrt(pow((float)ACCEL_X,2)+pow((float)ACCEL_Y,2)))*10;
		//if(ACCEL_XANGLE<0){  ACCEL_XANGLE=-(ACCEL_XANGLE);} 
	    //if(ACCEL_YANGLE<0){  ACCEL_YANGLE=-(ACCEL_YANGLE);} 
		//if(ACCEL_ZANGLE<0){  ACCEL_ZANGLE=-(ACCEL_ZANGLE);} 
			
		
		/*	*/	
		
		 if (Priem == 1)
		 {
			  
			 Priem = 0;
			 if (glavmas[0] == 'R')  // проверяем первый символ
			 {
				 CommandR();    // Группа команд чтения данных
				 
			 }
			 else if  (glavmas[0] == 'W') // проверяем первый символ
			 {
				 CommandW(); // Группа команд записи данных
			 }
			 else
			 {
				 UART1_Transmit (Vp);
				 Priem = 0;
			 } // {есть ошибка}
				
         //TODO:: Please write your application code 
         }
	
	
	}		 
	
}

