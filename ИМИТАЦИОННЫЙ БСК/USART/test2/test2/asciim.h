
#ifndef ASCIIM_H//_INCLUDED
#define ASCIIM_H//_INCLUDED
//#include <inttype.h>
#include <stdint.h>
#define  CR   0x0D  
#define  Zv   0x2A
#define  Vp   0x3F
#define  Pr   0x7E       //волнистая линия
#define  Pl   0x2B       //плюс
#define  Mn   0x2D       //минус
#define  Rv   0x3D       //равно
#define  Sl   0x50       //слэш

#define BUFF_MAX 255 // максимальный рамер приемного буфера.

unsigned char glavmas [BUFF_MAX]; //массив для приема информации протокола ГФ ХХХ.ХХХ.ХХ
unsigned int Priem; // флаг приема



void UART1_Transmit (unsigned char data);
unsigned char UART1_Receive( void );
void RR (unsigned char TR); // функция передачи байта по UART в ASCii 
void ascii_8 (unsigned char FF);  //Функция преоброзования и передачи байта в ASCii формате
void ascii_16 (unsigned char FFFF); // функция преоброзования двух байт в ASCii формате
uint16_t hex (unsigned int i );         // преоброзование принятого байта (символа)
void read_mass ( unsigned char S);   //чтение символов в массив по протоколу ГФ
unsigned char mass (unsigned int i);




#endif 



/* Библиотека приема и передачи команд по UART в кодах ASCii 
*- Первый символ:
*- R - символ комады чтения (данных)
*- W - символ команды записи данных (команд)
*- Второй символ - вид команды -данных для чтения/записи:

*-Третий символ номер канала для чтения/записи в ASCii кодах (только буквы A, B, C ... a, b, c, ...  )
  в качестве символа номера канала запрещается использать буквы L , l ... это символ для чтения /записеи всех каналов данного типа.
  

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*- сиволом окончания кманды (данных) является символ перевода каретки те CR
*/