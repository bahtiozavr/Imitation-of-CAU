
#ifndef I2CGF_H_
#define I2CGF_H_
//===============================================================================================
//    		 Подключение к выводам МК, задержки
//===============================================================================================
#define SDA			2		// ножка данных, SDA
#define SCL			0		// ножка тактирования, SCL
#define I2C_PORT	PORTB	// Тут указать порт, например PORTB или PORTC
#define I2C_PIN		PINB	// Тут указать порт, например PINB или PINC
#define I2C_DDR		DDRB	// Тут указать порт, например DDRB или DDRC
// все задержки при работе шины i2c, изменяйте это число только если знаете, что делаете!!!
#define I2C_DELAY	_delay_us(40);	 // по умолчанию задержки 10 микросекунд (частота работы 1/10us = 100кГц)

/** defines the data direction (reading from I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_READ    1

/** defines the data direction (writing to I2C device) in i2c_start(),i2c_rep_start() */
#define I2C_WRITE   0

//definitions
#define MPU6050_ADDR (0x68 << 1) //device address - 0x68 pin low (GND), 0x69 pin high (VCC)






//*************************************************************
//ПРОТОТИПЫ ФУНКЦИЙ
void i2c_init (void);									// настраивает выводы микроконтроллера
void i2c_start_cond (void);								// генерирует условие старта
void i2c_restart_cond (void);							// генерирует условие повторного старта
void i2c_stop_cond (void);								// генерирует условие стоп
unsigned char i2c_send_byte (unsigned char data);		// возвращает ACK, если = 1 значит данные переданы неуспешно, если = 0 значит данные переданы успешно, data это передаваемый байт
unsigned char i2c_get_byte (unsigned char last_byte);	// возвращает считанный байт, last_byte = 1 означает что считываем последний байт, last_byte = 0 означает что будем считывать ещё байт


#endif /*I2CGF*/
