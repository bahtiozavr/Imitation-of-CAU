
#ifndef ASCIIM_H//_INCLUDED
#define ASCIIM_H//_INCLUDED



#define  CR   0x0D  
#define  Zv   0x2A
#define  Vp   0x3F
#define  Pr   0x7E       //��������� �����
#define  Pl   0x2B       //����
#define  Mn   0x2D       //�����
#define  Rv   0x3D       //�����
#define  Sl   0x50       //����

#define BUFF_MAX 255 // ������������ ����� ��������� ������.

unsigned char glavmas [BUFF_MAX]; //������ ��� ������ ���������� ��������� �� ���.���.��
unsigned int Priem; // ���� ������
unsigned int FFFF;



void UART1_Transmit (unsigned char data);
unsigned char UART1_Receive( void );
void RR (unsigned char TR); // ������� �������� ����� �� UART � ASCii 
void ascii_8 (unsigned char FF);  //������� �������������� � �������� ����� � ASCii �������
void ascii_16 (unsigned char FFFF); // ������� �������������� ���� ���� � ASCii �������
void hex (unsigned int ii,int hh );         // �������������� ��������� ����� (�������)
void read_mass ( unsigned char S);   //������ �������� � ������ �� ��������� ��
void mass (unsigned int i);




#endif 



/* ���������� ������ � �������� ������ �� UART � ����� ASCii 
*- ������ ������:
*- R - ������ ������ ������ (������)
*- W - ������ ������� ������ ������ (������)
*- ������ ������ - ��� ������� -������ ��� ������/������:

*-������ ������ ����� ������ ��� ������/������ � ASCii ����� (������ ����� A, B, C ... a, b, c, ...  )
  � �������� ������� ������ ������ ����������� ���������� ����� L , l ... ��� ������ ��� ������ /������� ���� ������� ������� ����.
  

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*- ������� ��������� ������ (������) �������� ������ �������� ������� �� CR
*/