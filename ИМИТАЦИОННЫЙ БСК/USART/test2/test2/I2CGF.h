
#ifndef I2CGF_H_
#define I2CGF_H_
//===============================================================================================
//    		 ����������� � ������� ��, ��������
//===============================================================================================
#define SDA			2		// ����� ������, SDA
#define SCL			0		// ����� ������������, SCL
#define I2C_PORT	PORTB	// ��� ������� ����, �������� PORTB ��� PORTC
#define I2C_PIN		PINB	// ��� ������� ����, �������� PINB ��� PINC
#define I2C_DDR		DDRB	// ��� ������� ����, �������� DDRB ��� DDRC
// ��� �������� ��� ������ ���� i2c, ��������� ��� ����� ������ ���� ������, ��� �������!!!
#define I2C_DELAY	_delay_us(40);	 // �� ��������� �������� 10 ����������� (������� ������ 1/10us = 100���)

/** defines the data direction (reading from I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_READ    1

/** defines the data direction (writing to I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_WRITE   0

//definitions
#define MPU6050_ADDR (0x68 << 1) //device address - 0x68 pin low (GND), 0x69 pin high (VCC)






//*************************************************************
//��������� �������
void i2c_init (void);									// ����������� ������ ����������������
void i2c_start_cond (void);								// ���������� ������� ������
void i2c_restart_cond (void);							// ���������� ������� ���������� ������
void i2c_stop_cond (void);								// ���������� ������� ����
unsigned char i2c_send_byte (unsigned char data);		// ���������� ACK, ���� = 1 ������ ������ �������� ���������, ���� = 0 ������ ������ �������� �������, data ��� ������������ ����
unsigned char i2c_get_byte (unsigned char last_byte);	// ���������� ��������� ����, last_byte = 1 �������� ��� ��������� ��������� ����, last_byte = 0 �������� ��� ����� ��������� ��� ����


#endif /*I2CGF*/
